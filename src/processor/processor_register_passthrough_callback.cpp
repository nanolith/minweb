/**
 * \file processor/processor_register_passthrough_callback.cpp
 *
 * \brief Register the passthrough callback.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <minweb/processor.h>

using namespace minweb;
using namespace std;

/**
 * \brief Register a callback for passthrough data.
 *
 * Passthrough data is data that is unchanged by the processor.  When not in
 * a macro, this data becomes part of the document.  When in a macro, this
 * data should be appended to the current macro block.
 */
void minweb::processor::register_passthrough_callback(
        function<void (const string&)> cb)
{
    passthrough_callback = cb;
}
