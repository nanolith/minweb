/**
 * \file processor/processor_register_special_directive_callback.cpp
 *
 * \brief Register the special directive callback.
 *
 * \copyright Copyright 2021 Justin Handville. All rights reserved.
 */
#include <minweb/processor.h>

using namespace minweb;
using namespace std;

/**
 * \brief Register a callback for a special directive.
 */
void minweb::processor::register_special_directive_callback(
        std::function<
            void (const std::pair<directive_type, std::string>&)> cb)
{
    special_directive_callback = cb;
}
