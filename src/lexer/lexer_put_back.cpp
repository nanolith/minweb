/**
 * \file lexer/lexer_put_back.cpp
 *
 * \brief Put a character back for later reading.
 *
 * \copyright Copyright 2020-2021 Justin Handville. All rights reserved.
 */
#include <istream>
#include <minweb/lexer.h>

using namespace minweb;
using namespace std;

/**
 * \brief Put a character back into the stream.
 *
 * \param ch        The character to put back.
 */
void minweb::lexer::put_back(int ch)
{
    putbackbuf.push_back(ch);
}
