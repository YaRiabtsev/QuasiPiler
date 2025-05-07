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

void group::add_token(const std::shared_ptr<token>& tok) noexcept {
    children_.push_back(tok);
}

void group::dump(std::stringstream& ss, const size_t indent) const noexcept {
    ss << std::string(indent * 2, ' ') << "Group {\n";
    for (auto& ptr : children_) {
        if (const auto sub = std::dynamic_pointer_cast<group>(ptr)) {
            sub->dump(ss, indent + 1);
        } else {
            ptr->dump(ss, indent + 1);
        }
    }
    ss << std::string(indent * 2, ' ') << "}\n";
}

bool group::is_end() const noexcept {
    if (children_.empty())
        return false;
    const auto k = children_.back()->kind;
    return k == token_kind::close_bracket || k == token_kind::eof;
}

lexer::lexer(reader& rdr) noexcept
    : reader_ref_(rdr) { }

void lexer::group_lexemes(group& root_group) {
    token t;
    reader_ref_.next_token(t);
    while (t.kind != token_kind::eof && t.kind != token_kind::close_bracket) {
        if (t.kind == token_kind::open_bracket) {
            group subgrp;
            subgrp.add_token(std::make_shared<token>(t));
            group_lexemes(subgrp);
            root_group.add_token(std::make_shared<group>(subgrp));
        } else {
            root_group.add_token(std::make_shared<token>(t));
        }
        reader_ref_.next_token(t);
    }
    root_group.add_token(std::make_shared<token>(t));
}
