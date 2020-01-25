/**
 * \file processor/processor_register_text_substitution_callback.cpp
 *
 * \brief Register the text substitution callback.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <minweb/processor.h>

using namespace minweb;
using namespace std;

/**
 * \brief Register a callback for a text substitution.
 */
void minweb::processor::register_text_substitution_callback(
        function<void (const tuple<substitution_type, string, string>&)> cb)
{
    text_substitution_callback = cb;
}
