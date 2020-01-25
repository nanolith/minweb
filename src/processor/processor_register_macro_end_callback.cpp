/**
 * \file processor/processor_register_macro_end_callback.cpp
 *
 * \brief Register the macro end callback.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <minweb/processor.h>

using namespace minweb;
using namespace std;

/**
 * \brief Register a callback for the end of a macro.
 */
void minweb::processor::register_macro_end_callback(function<void ()> cb)
{
    macro_end_callback = cb;
}
