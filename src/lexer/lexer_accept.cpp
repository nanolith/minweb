/**
 * \file lexer/lexer_accept.cpp
 *
 * \brief Accept a character as part of the string representation of a token.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <istream>
#include <minweb/lexer.h>

using namespace minweb;
using namespace std;

void minweb::lexer::accept(int ch)
{
    end_line = curline;
    end_col = curcol;

    tokenbuf.push_back(ch);
}
