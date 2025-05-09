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

#include "lexer.hpp"

group::group() noexcept = default;

group::group(const token& t) {
    start = { t.line, t.column };
    if (t.kind != token_kind::open_bracket) {
        throw std::invalid_argument("token is not an opening bracket");
    }
    if (t.word == "[") {
        kind = group_kind::square;
    } else if (t.word == "(") {
        kind = group_kind::round;
    } else if (t.word == "{") {
        kind = group_kind::curly;
    } else {
        throw std::invalid_argument("token is not a valid opening bracket");
    }
}

void group::add_token(const std::shared_ptr<token>& tok) noexcept {
    children.push_back(tok);
}

void group::dump(
    std::ostream& os, const std::string& prefix, const bool is_last
) const noexcept {
    os << prefix;
    std::string child_prefix = prefix;
    if (kind != group_kind::file) {
        os << (is_last ? "`-" : "|-");
        child_prefix += +(is_last ? "  " : "| ");
    }
    os << "Group kind=" << static_cast<int>(kind) << " <" << start.first << ':'
       << start.second << "> <" << end.first << ':' << end.second << ">\n";

    auto b = children.begin();
    // if (!children.empty() && (*b)->kind == token_kind::open_bracket) {
    //     ++b;
    // }
    auto e = children.end();
    // if (e != b) {
    //     --e;
    // }
    for (auto it = b; it != e; ++it) {
        const bool last_child = (std::next(it) == e);
        (*it)->dump(os, child_prefix, last_child);
    }
}

bool group::is_end() const noexcept {
    if (children.empty())
        return false;
    const auto k = children.back()->kind;
    return k == token_kind::close_bracket || k == token_kind::eof;
}

void group::close(const token& token) {
    end = { token.line, token.column };
    if (token.kind == token_kind::eof && kind == group_kind::file) {
        return;
    }
    if (token.kind == token_kind::close_bracket) {
        if (token.word == "]" && kind == group_kind::square) {
            return;
        }
        if (token.word == ")" && kind == group_kind::round) {
            return;
        }
        if (token.word == "}" && kind == group_kind::curly) {
            return;
        }
    }

    std::ostringstream os;
    token.dump(os);
    throw std::runtime_error("unmatched closing bracket: " + os.str());
}

lexer::lexer(reader& rdr) noexcept
    : reader_(rdr) { }

void lexer::group_lexemes(group& root_group) {
    std::vector<std::shared_ptr<token>> tokens;
    token t;
    reader_.next_token(t);
    while (t.kind != token_kind::eof && t.kind != token_kind::close_bracket) {
        if (t.kind == token_kind::whitespace || t.kind == token_kind::comment) {
        } else if (t.kind == token_kind::open_bracket) {
            group subgroup(t);
            group_lexemes(subgroup);
            tokens.push_back(std::make_shared<group>(subgroup));
        } else {
            tokens.push_back(std::make_shared<token>(t));
        }
        reader_.next_token(t);
    }
    for (const auto& token : tokens) {
        root_group.add_token(token);
    }
    root_group.close(t);
}
