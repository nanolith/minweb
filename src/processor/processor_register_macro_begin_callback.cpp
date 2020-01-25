/**
 * \file processor/processor_register_macro_begin_callback.cpp
 *
 * \brief Register the macro begin callback.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <minweb/processor.h>

using namespace minweb;
using namespace std;

/**
 * \brief Register a callback for the beginning of a macro.
 */
void minweb::processor::register_macro_begin_callback(
        function<void (const pair<macro_type, string>&)> cb)
{
    macro_begin_callback = cb;
}
