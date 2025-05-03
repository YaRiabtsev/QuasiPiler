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

enum class token {
    eof,
    open_bracket,
    close_bracket,
    separator,
    keyword,
    string,
    whitespace,
    integer,
    floating,
    special_character
};

class reader {
public:
    explicit reader(
        const std::filesystem::path& path, std::streamsize buffer_size = 4096
    );
    explicit reader(std::string& str);

    ~reader();

    std::tuple<token, int, int> read_lexeme(std::string& lexeme);
    void jump_to(std::streamoff position, int line, int col);

private:
    std::ifstream ifs;
    std::string buffer;
    std::streamsize max_buffer_size {};
    std::streamoff offset { -1 };
    int col { -1 }, line { -1 }, shift { -1 };

    bool valid() const;
    char peek() const;
    char get();
    void next();

    void read_buffer();
    void parse_whitespace(std::string& word);
    void parse_keyword(std::string& word);
    void parse_string(std::string& value);
    token parse_number(std::string& number);

    std::runtime_error throw_message(
        const std::string& message,
        std::source_location location = std::source_location::current()
    ) const;
};

#endif // READER_HPP
