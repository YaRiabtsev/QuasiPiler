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

#include "reader.hpp"

#include <algorithm>
#include <cassert>

token reader::parse_number(std::string& number) {
    if (!number.empty()) {
        number.clear();
    }
    bool is_float = false;
    if (valid() && peek() == '0') {
        number += get();
        if (valid() && std::isdigit(peek())) {
            throw throw_message(
                "invalid number format: leading zeros are not allowed"
            );
        }
    } else if (valid() && std::isdigit(peek())) {
        do {
            number += get();
        } while (valid() && std::isdigit(peek()));
    } else {
        throw throw_message("invalid number format: expected digit");
    }
    if (valid() && peek() == '.') {
        is_float = true;
        number += get();
        if (!valid() || !std::isdigit(peek())) {
            throw throw_message(
                "invalid number format: expected digit after decimal"
            );
        }
        while (valid() && std::isdigit(peek())) {
            number += get();
        }
    }
    if (valid() && (peek() == 'e' || peek() == 'E')) {
        is_float = true;
        number += get();
        if (valid() && (peek() == '+' || peek() == '-')) {
            number += get();
        }
        if (!valid() || !std::isdigit(peek())) {
            throw throw_message(
                "invalid number format: expected digit after exponent"
            );
        }
        while (valid() && std::isdigit(peek())) {
            number += get();
        }
    }
    assert(!number.empty() && "number is not empty");
    return is_float ? token::floating : token::integer;
}

std::runtime_error reader::throw_message(
    const std::string& message, const std::source_location location
) const {
    std::ostringstream oss;
    oss << "[Parser-Error] " << message << ". ";
#ifndef NDEBUG
    if (!ifs.is_open()) {
        oss << "No input stream open. ";
    }
    if (!valid()) {
        oss << "Position is out of range. Line: " << (line + 1)
            << ", position: " << (col + 1) << " exceeds available input. ";
    } else {
        const char current_char = peek();
        oss << "Character '" << current_char
            << "' (ASCII: " << static_cast<int>(current_char)
            << ") was found at line " << (line + 1) << ", position "
            << (col + 1) << ". ";
    }
    oss << "In file: " << location.file_name() << '(' << location.line() << ':'
        << location.column() << ") `" << location.function_name() << "`";
    oss << std::endl << buffer;
#endif
    return std::runtime_error(oss.str());
}

reader::reader(const std::filesystem::path& path, std::streamsize buffer_size)
    : max_buffer_size(buffer_size) {
    ifs.open(path, std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
        throw std::invalid_argument("failed to open file: " + path.string());
    }
    ifs.seekg(0, std::ios::beg);
    offset = ifs.tellg();
    read_buffer();
}

reader::reader(std::string& str)
    : buffer(std::move(str)) {
    if (!buffer.empty()) {
        line = 0;
        col = 0;
    }
}

reader::~reader() {
    if (ifs.is_open()) {
        ifs.close();
    }
}

bool reader::valid() const {
    return shift >= 0 && shift < static_cast<int>(buffer.size());
}

char reader::peek() const { return buffer[shift]; }

char reader::get() {
    const char current = peek();
    next();
    return current;
}

void reader::next() {
    assert(shift >= 0 && "stream is not empty");
    ++shift;
    if (shift >= static_cast<int>(buffer.size())) {
        read_buffer();
    }
}

void reader::read_buffer() {
    if (ifs.eof()) {
        return;
    }
    offset = ifs.tellg();
    ifs.read(buffer.data(), max_buffer_size);
    if (const auto read_bytes = ifs.gcount(); read_bytes < max_buffer_size) {
        buffer.resize(read_bytes);
    }
}

void reader::parse_whitespace(std::string& word) {
    if (!word.empty()) {
        word.clear();
    }
    while (valid() && std::isspace(peek())) {
        if (peek() == '\n') {
            ++line;
            col = -1;
        }
        word += get();
    }
}

void reader::parse_keyword(std::string& word) {
    assert(
        valid() && (std::isalpha(peek()) || peek() == '_')
        && "expected keyword starting with a letter"
    );
    if (!word.empty()) {
        word.clear();
    }
    do {
        word += get();
    } while (valid() && (std::isalnum(peek()) || peek() == '_'));
}

void reader::parse_string(std::string& value) {
    assert(
        valid() && (peek() == '"' || peek() == '\'') && "expected opening quote"
    );
    if (!value.empty()) {
        value.clear();
    }
    const char quote = get();
    bool is_backslash = false;
    do {
        const char current = peek();
        if (is_backslash) {
            switch (current) {
            case '\"':
                value += '\"';
                break;
            case '\\':
                value += '\\';
                break;
            case '/':
                value += '/';
                break;
            case 'b':
                value += '\b';
                break;
            case 'f':
                value += '\f';
                break;
            case 'n':
                value += '\n';
                break;
            case 'r':
                value += '\r';
                break;
            case 't':
                value += '\t';
                break;
            case 'u': {
                std::string hex;
                for (int i = 0; i < 4; ++i) {
                    next();
                    if (!valid() || !isxdigit(peek())) {
                        throw throw_message(
                            "invalid Unicode escape sequence in JSON string"
                        );
                    }
                    hex += peek();
                }
                const int codepoint = std::stoi(hex, nullptr, 16);
                std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>
                    converter;
                value += converter.to_bytes(static_cast<char32_t>(codepoint));
                break;
            }
            default:
                throw throw_message("invalid escape sequence in JSON string");
            }
            is_backslash = false;
        } else if (current == '\\') {
            is_backslash = true;
        } else if (current == '\"') {
            break;
        } else {
            value += current;
        }
        next();
    } while (valid());
    if (!valid() || peek() != quote) {
        throw throw_message("invalid JSON string: Missing closing quote");
    }
    next();
}

void reader::jump_to(
    const std::streamoff position, const int line, const int col
) {
    if (position < 0) {
        throw throw_message("position is out of range");
    }
    if (!ifs.is_open()) {
        if (position > buffer.size()) {
            throw throw_message("position is out of range");
        }
        shift = static_cast<int>(position);
    } else {
        ifs.seekg(position, std::ios::beg);
        read_buffer();
    }
    this->line = line;
    this->col = col;
}

std::tuple<token, int, int> reader::read_lexeme(std::string& lexeme) {
    if (!lexeme.empty()) {
        lexeme.clear();
    }
    const std::pair position = { line, col };
    auto t = token::special_character;
    if (!valid()) {
        t = token::eof;
    } else {
        if (peek() == '(' || peek() == '[' || peek() == '{') {
            t = token::open_bracket;
        } else if (peek() == ')' || peek() == ']' || peek() == '}') {
            t = token::close_bracket;
        } else if (peek() == ',' || peek() == ';' || peek() == ':') {
            t = token::separator;
        } else if (std::isalpha(peek()) || peek() == '_') {
            parse_keyword(lexeme);
            t = token::keyword;
        } else if (std::isdigit(peek())) {
            t = parse_number(lexeme);
        } else if (peek() == '\'' || peek() == '"') {
            parse_string(lexeme);
            t = token::string;
        } else if (peek() == ' ' || peek() == '\t' || peek() == '\n') {
            parse_whitespace(lexeme);
            t = token::whitespace;
        }
        if (lexeme.empty()) {
            lexeme = get();
        }
    }
    return { t, position.first, position.second };
}