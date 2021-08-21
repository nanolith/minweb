/**
 * \file lexer/lexer.cpp
 *
 * \brief Lexer constructor.
 *
 * \copyright Copyright 2020-2021 Justin Handville. All rights reserved.
 */
#include <minweb/lexer.h>

using namespace minweb;
using namespace std;

/**
 * This constructor builds a lexer from an input stream reference.
 *
 * \param input     Input stream from which tokens are read.
 * \param name      The name of the input stream.
 */
minweb::lexer::lexer(std::istream* input, const std::string& name)
    : in(input)
    , in_name(name)
    , curline(1)
    , curcol(0)
    , start_line(0)
    , start_col(0)
    , end_line(0)
    , end_col(0)
{
}
