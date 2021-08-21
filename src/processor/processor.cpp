/**
 * \file processor/processor.cpp
 *
 * \brief Processor constructor.
 *
 * \copyright Copyright 2020-2021 Justin Handville. All rights reserved.
 */
#include <minweb/processor.h>

using namespace minweb;
using namespace std;

/**
 * This constructor builds a processor from an input stream reference.
 *
 * \param input     Input stream to process.
 * \param name      Input stream name.
 */
minweb::processor::processor(std::istream* input, const std::string& name)
    : in(input, name)
{
}
