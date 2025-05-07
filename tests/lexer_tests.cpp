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

#include <gtest/gtest.h>

TEST(LexerTest, Group) {
    std::string str = "[1,2,3,4]";
    reader r { str };
    lexer l { r };
    group g;
    l.group_lexemes(g);
    std::stringstream ss;
    g.dump(ss, 0);
    EXPECT_EQ(
        ss.str(),
        "Group {\n  Group {\n    Token(\"[\") kind=1 @0:0 off=0\n    "
        "Token(\"1\") kind=8 @0:1 off=1\n    Token(\",\") kind=3 @0:2 off=2\n  "
        "  Token(\"2\") kind=8 @0:3 off=3\n    Token(\",\") kind=3 @0:4 "
        "off=4\n    Token(\"3\") kind=8 @0:5 off=5\n    Token(\",\") kind=3 "
        "@0:6 off=6\n    Token(\"4\") kind=8 @0:7 off=7\n    Token(\"]\") "
        "kind=2 @0:8 off=8\n  }\n  Token(\"\") kind=0 @0:9 off=9\n}\n"
    );
}