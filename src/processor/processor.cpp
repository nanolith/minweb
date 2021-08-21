/**
 * \file processor/processor.cpp
 *
 * \brief Processor constructor.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <minweb/processor.h>

using namespace minweb;
using namespace std;

/**
 * This constructor builds a processor from an input stream reference.
 *
 * \param input     Input stream to process.
 */
minweb::processor::processor(istream* input)
    : in(input)
{
}
