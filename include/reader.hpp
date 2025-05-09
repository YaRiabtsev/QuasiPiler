/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Yaroslav Riabtsev <yaroslav.riabtsev@rwth-aachen.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef READER_HPP
#define READER_HPP

#include <filesystem>
#include <fstream>
#include <source_location>

/**
 * \enum token_kind
 * \brief Enumeration of lexical token types recognized by the reader.
 */
enum class token_kind {
    eof, ///< End of file reached.
    open_bracket, ///< Opening bracket: '(', '[', or '{'.
    close_bracket, ///< Closing bracket: ')', ']', or '}'.
    separator, ///< Separator character: ',', ';', or ':'.
    keyword, ///< Keyword or identifier (starts with letter or '_').
    string, ///< String literal (quoted).
    comment, ///< Single- or multi-line comment.
    whitespace, ///< Sequence of whitespace characters.
    integer, ///< Integer numeric literal.
    floating, ///< Floating-point numeric literal.
    special_character ///< Any other character not otherwise classified.
};

/**
 * \struct token
 * \brief Represents a lexical token with source metadata.
 */
struct token {
    token_kind kind; ///< Type of the token.
    int line; ///< Zero-based line number where token begins.
    int column; ///< Zero-based column number where token begins.
    std::streamoff file_offset; ///< Absolute byte offset in the input.
    std::string word; ///< Raw token text.

    virtual ~token();

    virtual void dump(
        std::ostream& os, const std::string& prefix, bool is_last
    ) const noexcept;

    void dump(std::ostream& os) const noexcept;
};

/**
 * \class reader
 * \brief Buffered character reader for lexing tokens from files or strings.
 *
 * Supports reading from a file or an in-memory string, with buffering,
 * and can lex keywords, numbers, strings, comments, whitespace, etc.
 */
class reader {
public:
    /**
     * \brief Constructs a reader that reads from a file.
     * \param path        Filesystem path of the file to read.
     * \param buffer_size Maximum buffer size in bytes (default = 4096).
     * \throws std::invalid_argument If the file cannot be opened.
     */
    explicit reader(
        const std::filesystem::path& path, std::streamsize buffer_size = 4096
    );

    /**
     * \brief Constructs a reader over an existing string buffer.
     * \param data String data to read from; the reader takes ownership via
     * move.
     */
    explicit reader(std::string& data) noexcept;

    /**
     * \brief Destructor. Closes the file stream if it is open.
     */
    ~reader();

    /**
     * \brief Reads the next lexical token from the input.
     * \param out Reference to a token struct to populate.
     * \throws std::runtime_error On malformed input (e.g., unterminated
     * string/comment).
     */
    void next_token(token& out);

    /**
     * \brief Jumps to a specific byte offset in the input stream.
     * \param position Absolute byte offset to seek to.
     * \param line     Zero-based line number to assign after seeking.
     * \param column   Zero-based column number to assign after seeking.
     * \throws std::runtime_error If the position is negative or out of range.
     */
    void jump_to_position(std::streamoff position, int line, int column);

private:
    std::ifstream ifs; ///< File stream if reading from a file.
    std::string buffer; ///< Buffer holding the current chunk of input.
    std::streamsize
        max_buffer_size {}; ///< Maximum number of bytes to read into buffer.
    std::streamoff
        file_offset {}; ///< Byte offset of the start of the buffer in file.
    int line { 0 }; ///< Current zero-based line number.
    int column { 0 }; ///< Current zero-based column number.
    size_t buffer_position { 0 }; ///< Current index into the buffer.

    /**
     * \brief Checks whether there is more input in the buffer.
     * \return True if there are unread characters in the buffer.
     */
    bool is_valid() const noexcept;

    /**
     * \brief Peeks at the next character without consuming it.
     * \return The character at the current buffer position.
     */
    char peek_char() const noexcept;

    /**
     * \brief Retrieves and consumes the current character.
     * \return The character that was at the buffer position.
     */
    char get_char();

    /**
     * \brief Advances the buffer position by one, refilling when needed.
     */
    void advance_char();

    /**
     * \brief Reloads the buffer from the input stream.
     *
     * Does nothing if the stream is closed or at end of file.
     */
    void reload_buffer();

    /**
     * \brief Reads and accumulates a run of whitespace characters.
     * \param into Output string to accumulate the whitespace.
     */
    void read_whitespace(std::string& into);

    /**
     * \brief Reads and accumulates an identifier (letters, digits, or '_').
     * \param into Output string to accumulate the identifier.
     */
    void read_keyword(std::string& into);

    /**
     * \brief Reads a quoted string literal, decoding escape sequences.
     * \param into Output string to accumulate the decoded content.
     * \throws std::runtime_error On invalid or unterminated string.
     */
    void read_string(std::string& into);

    /**
     * \brief Reads a comment (single- or multi-line) starting with '/'.
     * \param into Output string to accumulate the entire comment
     *             (must start with initial '/').
     * \throws std::runtime_error On unterminated multi-line comment.
     */
    void read_comment(std::string& into);

    /**
     * \brief Reads a numeric literal (integer or floating-point).
     * \param into Output string to accumulate the digits and symbols.
     * \return token_kind::integer or token_kind::floating.
     * \throws std::runtime_error On malformed numeric input.
     */
    token_kind read_number(std::string& into);

    /**
     * \brief Initializes a token with the reader’s current position.
     * \param t Token to initialize.
     */
    void init_token(token& t) const noexcept;

    /**
     * \brief Constructs a detailed runtime error with context.
     * \param message  Description of the problem.
     * \param location Source location for debugging (defaults to call site).
     * \return A std::runtime_error containing full diagnostic information.
     */
    [[nodiscard]] std::runtime_error make_error(
        const std::string& message,
        const std::source_location& location = std::source_location::current()
    ) const;
};

#endif // READER_HPP
