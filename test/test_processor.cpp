/**
 * \file test/test_lexer.cpp
 *
 * \brief Unit tests for the processor.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <iostream>
#include <minunit/minunit.h>
#include <minweb/processor.h>
#include <sstream>
#include <tuple>

using namespace minweb;
using namespace std;

TEST_SUITE(processor);

/**
 * When macro begin and end callbacks have been registered, they are called.
 */
TEST(macro_begin_end_callbacks)
{
    bool begin_callback_called = false;
    bool end_callback_called = false;
    stringstream in(
        R"TEST(
            <<SECTION:bar>>=
            >>@<<
        )TEST");
    processor p(&in, "test_input");

    p.register_macro_begin_callback(
        [&](const pair<macro_type, string>& p) {
            begin_callback_called = true;
        });

    p.register_macro_end_callback(
        [&]() {
            end_callback_called = true;
        });

    /* preconditions. */
    TEST_ASSERT(!begin_callback_called);
    TEST_ASSERT(!end_callback_called);

    /* process the stream. */
    p.run();

    /* postconditions. */
    TEST_EXPECT(begin_callback_called);
    TEST_EXPECT(end_callback_called);
}

/**
 * Macros can't be nested.
 */
TEST(nested_macro_begin)
{
    bool begin_callback_called = false;
    bool end_callback_called = false;
    stringstream in(
        R"TEST(
            <<SECTION:foo>>=
            <<SECTION:bar>>=
            >>@<<
        )TEST");
    processor p(&in, "test_input");

    p.register_macro_begin_callback(
        [&](const pair<macro_type, string>& p) {
            begin_callback_called = true;
        });

    p.register_macro_end_callback(
        [&]() {
            end_callback_called = true;
        });

    /* an exception should be thrown due to nested macro begins. */
    try
    {
        p.run();
        TEST_FAILURE();
    } catch (processor_error& e)
    {
        TEST_SUCCESS();
    }
}

/**
 * Macros must end.
 */
TEST(missing_macro_end)
{
    bool begin_callback_called = false;
    bool end_callback_called = false;
    stringstream in(
        R"TEST(
            <<SECTION:foo>>=
        )TEST");
    processor p(&in, "test_input");

    p.register_macro_begin_callback(
        [&](const pair<macro_type, string>& p) {
            begin_callback_called = true;
        });

    p.register_macro_end_callback(
        [&]() {
            end_callback_called = true;
        });

    /* an exception should be thrown due to a missing macro end. */
    try
    {
        p.run();
        TEST_FAILURE();
    } catch (processor_error& e)
    {
        TEST_SUCCESS();
    }
}

/**
 * A macro end must occur after a macro begin.
 */
TEST(dangling_macro_end)
{
    bool begin_callback_called = false;
    bool end_callback_called = false;
    stringstream in(
        R"TEST(
            >>@<<
        )TEST");
    processor p(&in, "test_input");

    p.register_macro_begin_callback(
        [&](const pair<macro_type, string>& p) {
            begin_callback_called = true;
        });

    p.register_macro_end_callback(
        [&]() {
            end_callback_called = true;
        });

    /* an exception should be thrown due to a dangling macro end. */
    try
    {
        p.run();
        TEST_FAILURE();
    } catch (processor_error& e)
    {
        TEST_SUCCESS();
    }
}

/**
 * A macro reference can be detected.
 */
TEST(macro_ref_callback)
{
    bool macro_ref_callback_called = false;
    string macro_ref;
    stringstream in(
        R"TEST(
            <<SECTION:bar>>=
                <<foo>>
            >>@<<
        )TEST");
    processor p(&in, "test_input");

    p.register_macro_ref_callback(
        [&](const string& ref) {
            macro_ref_callback_called = true;
            macro_ref = ref;
        });

    /* preconditions. */
    TEST_ASSERT(!macro_ref_callback_called);

    /* process the stream. */
    p.run();

    /* postconditions. */
    TEST_EXPECT(macro_ref_callback_called);
    TEST_EXPECT(string("foo") == macro_ref);
}

/**
 * A macro ref can't occur outside of a macro.
 */
TEST(dangling_macro_ref)
{
    stringstream in(
        R"TEST(
            <<foo>>
        )TEST");
    processor p(&in, "test_input");

    /* an exception should be thrown due to a dangling macro ref. */
    try
    {
        p.run();
        TEST_FAILURE();
    } catch (processor_error& e)
    {
        TEST_SUCCESS();
    }
}

/**
 * A text substitution can be detected.
 */
TEST(text_substitution_callback)
{
    bool sub_called = false;
    substitution_type ty;
    string sub;
    stringstream in(
        R"TEST(
            %[xyzzy]%
        )TEST");
    processor p(&in, "test_input");

    p.register_text_substitution_callback(
        [&](const tuple<substitution_type, string, string>& s) {
            sub_called = true;
            ty = get<0>(s);
            sub = get<1>(s);
        });

    /* preconditions. */
    TEST_ASSERT(!sub_called);

    /* process the stream. */
    p.run();

    /* postconditions. */
    TEST_EXPECT(sub_called);
    TEST_EXPECT(MINWEB_SUBSTITUTION_TYPE_DEFAULT == ty);
    TEST_EXPECT(string("xyzzy") == sub);
}

/**
 * Passthrough data can be seen via callback.
 */
TEST(passthrough_callback)
{
    bool passthrough_called = false;
    string data;
    stringstream in(" 123 ");
    processor p(&in, "test_input");

    p.register_passthrough_callback(
        [&](const string& s) {
            passthrough_called = true;
            data += s;
        });

    /* preconditions. */
    TEST_ASSERT(!passthrough_called);

    /* process the stream. */
    p.run();

    /* postconditions. */
    TEST_EXPECT(passthrough_called);
    TEST_EXPECT(string(" 123 ") == data);
}

/**
 * Simple substitution hackery.
 */
TEST(substitution_fun)
{
    string data;
    stringstream in("Hello, %[place]%.");
    processor p(&in, "test_input");

    p.register_passthrough_callback(
        [&](const string& s) {
            data += s;
        });

    p.register_text_substitution_callback(
        [&](const tuple<substitution_type, string, string>& s) {
            data += "World";
        });

    /* process the stream. */
    p.run();

    /* postconditions. */
    TEST_EXPECT(string("Hello, World.") == data);
}

/**
 * The include special directive is visible via callback.
 */
TEST(include_special_directive_callback)
{
    bool special_called = false;
    directive_type special_type;
    string special_value;
    stringstream in("#[include=foo]");
    processor p(&in, "test_input");

    p.register_special_directive_callback(
        [&](const pair<directive_type, string>& d) {
            special_called = true;
            special_type = d.first;
            special_value = d.second;
        });

    /* process the stream. */
    p.run();

    /* postconditions. */
    TEST_EXPECT(special_called == true);
    TEST_EXPECT(special_type == MINWEB_DIRECTIVE_TYPE_INCLUDE);
    TEST_EXPECT(special_value == "foo");
}

/**
 * The language special directive is visible via callback.
 */
TEST(language_special_directive_callback)
{
    bool special_called = false;
    directive_type special_type;
    string special_value;
    stringstream in("#[language=bar]");
    processor p(&in, "test_input");

    p.register_special_directive_callback(
        [&](const pair<directive_type, string>& d) {
            special_called = true;
            special_type = d.first;
            special_value = d.second;
        });

    /* process the stream. */
    p.run();

    /* postconditions. */
    TEST_EXPECT(special_called == true);
    TEST_EXPECT(special_type == MINWEB_DIRECTIVE_TYPE_LANGUAGE);
    TEST_EXPECT(special_value == "bar");
}

/**
 * An unsupported directive type throws an exception.
 */
TEST(unsupported_special_directive_callback)
{
    stringstream in("#[pragma=something]");
    processor p(&in, "test_input");

    /* process the stream. */
    try
    {
        p.run();
        TEST_FAILURE();
    }
    catch (processor_error& e)
    {
        TEST_SUCCESS();
    }
}
