/**
 * \file lexer/lexer.cpp
 *
 * \brief Lexer constructor.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <minweb/lexer.h>

using namespace minweb;
using namespace std;

/**
 * This constructor builds a lexer from an input stream reference.
 *
 * \param input     Input stream from which tokens are read.
 */
minweb::lexer::lexer(std::istream* input)
    : in(input)
    , curline(1)
    , curcol(0)
    , start_line(0)
    , start_col(0)
    , end_line(0)
    , end_col(0)
{
}
