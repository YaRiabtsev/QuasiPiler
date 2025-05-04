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

/** \enum token_kind
 *  \brief Kinds of lexical tokens that the Reader can produce.
 */
enum class token_kind {
    eof, ///< End of file
    open_bracket, ///< '(', '[', or '{'
    close_bracket, ///< ')', ']', or '}'
    separator, ///< ',', ';', or ':'
    keyword, ///< Identifier starting with letter or underscore
    string, ///< Quoted string literal
    whitespace, ///< Sequence of whitespace characters
    integer, ///< Integer numeric literal
    floating, ///< Floating-point numeric literal
    special_character ///< Any other single character
};

/** \struct token
 *  \brief Represents a lexeme along with its source location.
 */
struct token {
    token_kind kind; ///< The category of the token
    int line; ///< Line number (0-based)
    int col; ///< Column number (0-based)
};

/** \class reader
 *  \brief A simple buffered character reader for lexing basic tokens.
 *
 *  The Reader reads from either a file or an in-memory string buffer,
 *  providing methods to retrieve the next lexeme, read lines, and
 *  control the read position.
 */
class reader {
public:
    /** \brief Construct a reader from a file path.
     *  \param path        Filesystem path to open for reading.
     *  \param buffer_size Maximum chunk size (in bytes) to read at once.
     *                     Defaults to 4096.
     *  \throws std::invalid_argument if the file cannot be opened.
     */
    explicit reader(
        const std::filesystem::path& path, std::streamsize buffer_size = 4096
    );
    /** \brief Construct a reader over an existing string buffer.
     *  \param str  Reference to a string containing input data.
     *              The reader takes ownership via move.
     */
    explicit reader(std::string& str) noexcept;

    /** \brief Destructor closes the input file if open. */
    ~reader();

    /** \brief Reads the next lexeme from the input.
     *  \param lexeme  Output parameter that receives the lexeme text.
     *  \return a token struct describing the lexeme and its location.
     *  \note If end-of-file is reached, returns token_kind::eof.
     */
    token next_token(std::string& lexeme);
    /** \brief Reads up to max_size characters or until a newline.
     *  \param data      Output buffer for the read characters.
     *  \param max_size  Maximum number of characters to read.
     *                   Defaults to 4096.
     *  \return A pair of (line, col) where the read began.
     */
    std::pair<int, int> readln(std::string& data, size_t max_size = 4096);

    /** \brief Jumps to an absolute position in the input stream.
     *  \param position  Byte offset from the beginning of input.
     *  \param line      Line number to set for subsequent reads.
     *  \param col       Column number to set for subsequent reads.
     *  \throws std::runtime_error if position is out of range.
     */
    void jump_to(std::streamoff position, int line, int col);

private:
    std::ifstream ifs; ///< Input file stream (if reading from file)
    std::string buffer; ///< Internal read buffer
    std::streamsize max_buffer_size {}; ///< Maximum size of buffer chunks
    std::streamoff offset { -1 }; ///< Current file offset for current buffer
    int col { -1 }, line { -1 }; ///< Current read position (line, col)
    size_t shift { 0 }; ///< Index within buffer for next char

    /** \brief Checks if the current buffer position is valid.
     *  \return true if shift is within [0, buffer.size()), false otherwise.
     */
    bool valid() const noexcept;
    /** \brief Peeks at the current character without consuming it.
     *  \pre valid() == true
     *  \return Current character in buffer.
     */
    char peek() const noexcept;

    /** \brief Consumes and returns the current character.
     *  \pre valid() == true
     *  \return The character at the current position.
     */
    char get();

    /** \brief Advances the buffer position by one character, refilling if
     * needed.
     */
    void next();

    /** \brief Refills the internal buffer from the input stream.
     *  Reads up to max_buffer_size bytes; resizes buffer to actual bytes read.
     */
    void refill_buffer();
    /** \brief Parses and consumes a sequence of whitespace characters.
     *  \param word  Output string to accumulate whitespace characters.
     */
    void read_whitespace(std::string& word);

    /** \brief Parses and consumes an identifier or keyword.
     *  \param word  Output string to accumulate identifier characters.
     */
    void read_keyword(std::string& word);

    /** \brief Parses and consumes a quoted string literal, handling escapes.
     *  \param value  Output string to accumulate unescaped characters.
     *  \throws std::runtime_error on invalid escape or missing quote.
     */
    void read_string(std::string& value);

    /** \brief Parses and consumes a numeric literal (int or float).
     *  \param number  Output string to accumulate numeric characters.
     *  \return token_kind::integer or token_kind::floating.
     *  \throws std::runtime_error on malformed number.
     */
    token_kind read_number(std::string& number);

    /** \brief Constructs a parse error with detailed context.
     *  \param message  Description of the error.
     *  \param location Source location from where the error was raised.
     *  \return A std::runtime_error containing full diagnostic text.
     */
    std::runtime_error throw_message(
        const std::string& message,
        std::source_location location = std::source_location::current()
    ) const;
};

#endif // READER_HPP
