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

#ifndef LEXER_HPP
#define LEXER_HPP

#include "reader.hpp"
#include <memory>
#include <sstream>
#include <vector>

/**
 * \class group
 * \brief A composite token that can contain other tokens or nested groups.
 *
 * Used to represent matched bracketed sections and their contents
 * as a tree of tokens/groups.
 */
class group final : public token {
public:
    /**
     * \brief Adds a child token or subgroup to this group.
     * \param tok Shared pointer to the token (or subgroup) to add.
     */
    void add_token(const std::shared_ptr<token>& tok) noexcept;

    /**
     * \brief Serializes this group and its children into a stream.
     *
     * Overrides token::dump to include indentation and braces.
     *
     * \param ss     Output stringstream to write into.
     * \param indent Number of indent levels (each level = two spaces).
     */
    void dump(std::stringstream& ss, size_t indent) const noexcept override;

    /**
     * \brief Checks if this group has reached its closing delimiter.
     * \return True if the last child is a closing bracket or EOF token.
     */
    [[nodiscard]] bool is_end() const noexcept;

private:
    std::vector<std::shared_ptr<token>>
        children_; ///< Contained tokens/sub-groups.
};

/**
 * \class lexer
 * \brief Recursively groups a flat token stream into nested groups.
 *
 * Uses a reader to pull tokens and builds a tree of \ref group instances,
 * matching open/close brackets and preserving other tokens verbatim.
 */
class lexer {
public:
    /**
     * \brief Constructs a lexer bound to a reader.
     * \param rdr Reference to a \ref reader from which to fetch tokens.
     */
    explicit lexer(reader& rdr) noexcept;

    /**
     * \brief Populates a root group with nested tokens and subgroups.
     *
     * Reads tokens from the associated reader until a matching
     * close-bracket or EOF is encountered, recursively handling
     * nested bracket pairs.
     *
     * \param root_group The top-level group to populate.
     */
    void group_lexemes(group& root_group);

private:
    reader& reader_ref_; ///< Underlying token source.
};

#endif // LEXER_HPP
