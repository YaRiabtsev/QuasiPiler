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

#include "grouper.hpp"
#include <gtest/gtest.h>

TEST(AstDump, ExampleAST) {
    // std::ostringstream path_out;
    // path_out << "test_data/sum.md";
    // std::ofstream out(path_out.str());

    for (int i = 0; i < 12; ++i) {
        try {
            std::stringstream idx;
            idx << std::setfill('0') << std::setw(2) << i;
            std::ostringstream path_in;
            path_in << "test_data/test" << idx.str() << ".qc";
            reader r(path_in.str());
            grouper g { r };
            auto res = g.parse_group();
            // out << "<details><summary>test" << idx.str()
            // << ".qc:</summary>\n\n ```\n";

            std::ostringstream path_out;
            path_out << "test_data/test" << idx.str() << ".dump";
            std::ofstream out(path_out.str());
            res->dump(out, "", true, true);
            // out << "```\n </details> \n\n";
        } catch (const std::runtime_error& e) {
            std::cout << "Error processing test case " << i << ": " << e.what()
                      << "\n\n";
        }
    }
}
