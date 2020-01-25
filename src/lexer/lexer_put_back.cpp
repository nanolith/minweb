/**
 * \file lexer/lexer_put_back.cpp
 *
 * \brief Put a character back for later reading.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <istream>
#include <minweb/lexer.h>

using namespace minweb;
using namespace std;

void minweb::lexer::put_back(int ch)
{
    putbackbuf.push_back(ch);
}
