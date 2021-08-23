/**
 * \file lexer/lexer_accept.cpp
 *
 * \brief Accept a character as part of the string representation of a token.
 *
 * \copyright Copyright 2020-2021 Justin Handville. All rights reserved.
 */
#include <istream>
#include <minweb/lexer.h>

using namespace minweb;
using namespace std;

/**
 * \brief Accept a character into the current token value.
 *
 * \param ch        The character to accept.
 */
void minweb::lexer::accept(int ch)
{
    end_line = curline;
    end_col = curcol;

    tokenbuf.push_back(ch);
}
