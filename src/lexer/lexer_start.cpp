/**
 * \file lexer/lexer_start.cpp
 *
 * \brief Start a new string representation of a token.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <istream>
#include <minweb/lexer.h>

using namespace minweb;
using namespace std;

void minweb::lexer::start(int ch)
{
    start_line = curline;
    start_col = curcol;

    tokenbuf.clear();

    accept(ch);
}
