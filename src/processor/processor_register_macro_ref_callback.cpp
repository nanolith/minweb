/**
 * \file processor/processor_register_macro_ref_callback.cpp
 *
 * \brief Register the macro reference callback.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <minweb/processor.h>

using namespace minweb;
using namespace std;

/**
 * \brief Register a callback for a macro reference.
 */
void minweb::processor::register_macro_ref_callback(
        function<void (const string&)> cb)
{
    macro_ref_callback = cb;
}
