/**
 * \file test/test_lexer.cpp
 *
 * \brief Unit tests for the lexer.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <iostream>
#include <minunit/minunit.h>
#include <minweb/lexer.h>
#include <sstream>
#include <tuple>

using namespace minweb;
using namespace std;

TEST_SUITE(lexer);

/**
 * It's possible to scan EOF.
 */
TEST(eof_token)
{
    stringstream in("");
    lexer scanner(&in);

    TEST_EXPECT(MINWEB_TOKEN_EOF == scanner.read());
}

/**
 * It's possible to scan < as passthrough.
 */
TEST(lt_passthrough)
{
    stringstream in("<");
    lexer scanner(&in);

    TEST_EXPECT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    TEST_EXPECT("<" == scanner.get_token_string());
    TEST_EXPECT(MINWEB_TOKEN_EOF == scanner.read());
}

/**
 * It's possible to scan << as passthrough.
 */
TEST(ltlt_passthrough)
{
    stringstream in("<<");
    lexer scanner(&in);

    TEST_EXPECT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    TEST_EXPECT("<<" == scanner.get_token_string());
    TEST_EXPECT(MINWEB_TOKEN_EOF == scanner.read());
}

/**
 * It's possible to scan <<. as passthrough.
 */
TEST(ltltdot_passthrough)
{
    stringstream in("<<.");
    lexer scanner(&in);

    TEST_EXPECT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    TEST_EXPECT("<<." == scanner.get_token_string());
    TEST_EXPECT(MINWEB_TOKEN_EOF == scanner.read());
}

/**
 * It's possible to scan <<.> as passthrough.
 */
TEST(ltltdotgt_passthrough)
{
    stringstream in("<<.>");
    lexer scanner(&in);

    TEST_EXPECT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    TEST_EXPECT("<<.>" == scanner.get_token_string());
    TEST_EXPECT(MINWEB_TOKEN_EOF == scanner.read());
}

/**
 * It's possible to scan a MACRO_REF.
 */
TEST(macro_ref_token)
{
    const auto s = string("<<foo>>");
    stringstream in(s);
    lexer scanner(&in);

    TEST_EXPECT(MINWEB_TOKEN_MACRO_REF == scanner.read());
    TEST_EXPECT(s == scanner.get_token_string());
    TEST_EXPECT(MINWEB_TOKEN_EOF == scanner.read());
}

/**
 * It's possible to scan a MACRO_START.
 */
TEST(macro_start_token)
{
    const auto s = string("<<foo>>=");
    stringstream in(s);
    lexer scanner(&in);

    TEST_EXPECT(MINWEB_TOKEN_MACRO_START == scanner.read());
    TEST_EXPECT(s == scanner.get_token_string());
    TEST_EXPECT(MINWEB_TOKEN_EOF == scanner.read());
}

/**
 * It's possible to scan MACRO_END.
 */
TEST(macro_end_token)
{
    stringstream in(">>@<<");
    lexer scanner(&in);

    TEST_EXPECT(MINWEB_TOKEN_MACRO_END == scanner.read());
    TEST_EXPECT(MINWEB_TOKEN_EOF == scanner.read());
}

/**
 * It's possible to scan % as passthrough.
 */
TEST(percent_passthrough)
{
    stringstream in("%");
    lexer scanner(&in);

    TEST_EXPECT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    TEST_EXPECT("%" == scanner.get_token_string());
    TEST_EXPECT(MINWEB_TOKEN_EOF == scanner.read());
}

/**
 * It's possible to scan %[ as passthrough.
 */
TEST(percentbracket_passthrough)
{
    stringstream in("%[");
    lexer scanner(&in);

    TEST_EXPECT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    TEST_EXPECT("%[" == scanner.get_token_string());
    TEST_EXPECT(MINWEB_TOKEN_EOF == scanner.read());
}

/**
 * It's possible to scan %[. as passthrough.
 */
TEST(percentbracketdot_passthrough)
{
    stringstream in("%[.");
    lexer scanner(&in);

    TEST_EXPECT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    TEST_EXPECT("%[." == scanner.get_token_string());
    TEST_EXPECT(MINWEB_TOKEN_EOF == scanner.read());
}

/**
 * It's possible to scan %[.] as passthrough.
 */
TEST(percentbracketdotbracket_passthrough)
{
    stringstream in("%[.]");
    lexer scanner(&in);

    TEST_EXPECT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    TEST_EXPECT("%[.]" == scanner.get_token_string());
    TEST_EXPECT(MINWEB_TOKEN_EOF == scanner.read());
}

/**
 * It's possible to scan a TEXT_SUBSTITUTION.
 */
TEST(text_substitution_macro)
{
    const auto s = string("%[foo]%");
    stringstream in(s);
    lexer scanner(&in);

    TEST_EXPECT(MINWEB_TOKEN_TEXT_SUBSTITUTION == scanner.read());
    TEST_EXPECT(s == scanner.get_token_string());
    TEST_EXPECT(MINWEB_TOKEN_EOF == scanner.read());
}

/**
 * It's possible to parse a file macro type from a macro begin.
 */
TEST(file_macro_type)
{
    stringstream in("<<FILE:main.c>>=");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_MACRO_START == scanner.read());
    auto macro = lexer::macro_type_from_macro_begin(scanner.get_token_string());
    TEST_EXPECT(MINWEB_MACRO_TYPE_FILE == macro.first);
    TEST_EXPECT(string("main.c") == macro.second);
}

/**
 * It's possible to parse a section macro type from a macro begin.
 */
TEST(section_macro_type)
{
    stringstream in("<<SECTION:Foo Bar Baz>>=");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_MACRO_START == scanner.read());
    auto macro = lexer::macro_type_from_macro_begin(scanner.get_token_string());
    TEST_EXPECT(MINWEB_MACRO_TYPE_SECTION == macro.first);
    TEST_EXPECT(string("Foo Bar Baz") == macro.second);
}

/**
 * It's possible to parse a root macro type from a macro begin.
 */
TEST(root_macro_type)
{
    stringstream in("<<*>>=");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_MACRO_START == scanner.read());
    auto macro = lexer::macro_type_from_macro_begin(scanner.get_token_string());
    TEST_EXPECT(MINWEB_MACRO_TYPE_ROOT == macro.first);
    TEST_EXPECT(string("*") == macro.second);
}

/**
 * Any other value within the macro brackets leads to a default macro type.
 */
TEST(default_macro_type1)
{
    stringstream in("<<blah>>=");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_MACRO_START == scanner.read());
    auto macro = lexer::macro_type_from_macro_begin(scanner.get_token_string());
    TEST_EXPECT(MINWEB_MACRO_TYPE_DEFAULT == macro.first);
    TEST_EXPECT(string("blah") == macro.second);
}

/**
 * An unknown type designator results in a default macro type.
 */
TEST(default_macro_type2)
{
    stringstream in("<<foo:bar>>=");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_MACRO_START == scanner.read());
    auto macro = lexer::macro_type_from_macro_begin(scanner.get_token_string());
    TEST_EXPECT(MINWEB_MACRO_TYPE_DEFAULT == macro.first);
    TEST_EXPECT(string("foo:bar") == macro.second);
}

/**
 * A bad string size throws a lexer_error.
 */
TEST(macro_begin_bad_string_size)
{
    try
    {
        lexer::macro_type_from_macro_begin("");
        TEST_FAILURE();
    }
    catch (lexer_error& e)
    {
        TEST_SUCCESS();
    }
}

/**
 * A bad string throws a lexer_error.
 */
TEST(macro_begin_bad_string)
{
    try
    {
        lexer::macro_type_from_macro_begin("some random string");
        TEST_FAILURE();
    }
    catch (lexer_error& e)
    {
        TEST_SUCCESS();
    }
}

/**
 * It's possible to parse an assignment from a text substitution.
 */
TEST(assignment_substitution)
{
    stringstream in("%[password=xyzzy]%");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_TEXT_SUBSTITUTION == scanner.read());
    auto sub =
        lexer::substitution_type_from_text_substitution(
            scanner.get_token_string());
    TEST_EXPECT(MINWEB_SUBSTITUTION_TYPE_ASSIGNMENT == get<0>(sub));
    TEST_EXPECT(string("password") == get<1>(sub));
    TEST_EXPECT(string("xyzzy") == get<2>(sub));
}

/**
 * It's possible to parse a default substitution from a text substitution.
 */
TEST(default_substitution)
{
    stringstream in("%[xyzzy]%");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_TEXT_SUBSTITUTION == scanner.read());
    auto sub =
        lexer::substitution_type_from_text_substitution(
            scanner.get_token_string());
    TEST_EXPECT(MINWEB_SUBSTITUTION_TYPE_DEFAULT == get<0>(sub));
    TEST_EXPECT(string("xyzzy") == get<1>(sub));
    TEST_EXPECT(string("") == get<2>(sub));
}

/**
 * A bad string size throws a lexer_error.
 */
TEST(text_substitution_bad_string_size)
{
    try
    {
        lexer::substitution_type_from_text_substitution("");
        TEST_FAILURE();
    }
    catch (lexer_error& e)
    {
        TEST_SUCCESS();
    }
}

/**
 * A bad string throws a lexer_error.
 */
TEST(text_substitution_bad_string)
{
    try
    {
        lexer::macro_type_from_macro_begin("some random string");
        TEST_FAILURE();
    }
    catch (lexer_error& e)
    {
        TEST_SUCCESS();
    }
}

/**
 * The value of a macro reference can be decoded.
 */
TEST(decode_macro_ref)
{
    stringstream in("<<blah>>");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_MACRO_REF == scanner.read());
    auto macro = lexer::decode_macro_ref(scanner.get_token_string());
    TEST_EXPECT(string("blah") == macro);
}

/**
 * An end of line terminates a macro reference search.
 */
TEST(macro_ref_endline)
{
    stringstream in("cat << EOF \n>>@<<");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    auto passthrough = scanner.get_token_string();
    TEST_EXPECT(string("c") == passthrough);
    TEST_ASSERT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    passthrough = scanner.get_token_string();
    TEST_EXPECT(string("a") == passthrough);
    TEST_ASSERT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    passthrough = scanner.get_token_string();
    TEST_EXPECT(string("t") == passthrough);
    TEST_ASSERT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    passthrough = scanner.get_token_string();
    TEST_EXPECT(string(" ") == passthrough);
    TEST_ASSERT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    passthrough = scanner.get_token_string();
    TEST_EXPECT(string("<< EOF \n") == passthrough);
    TEST_ASSERT(MINWEB_TOKEN_MACRO_END == scanner.read());
}

/**
 * It's possible to scan a special directive.
 */
TEST(special_directive_include)
{
    stringstream in("#[include=stdio.h]");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_SPECIAL_DIRECTIVE == scanner.read());
    auto special = scanner.get_token_string();
    TEST_EXPECT(string("#[include=stdio.h]") == special);
}

/**
 * A single hash is a passthrough.
 */
TEST(hash_passthrough)
{
    stringstream in("#");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    auto special = scanner.get_token_string();
    TEST_EXPECT(string("#") == special);
}

/**
 * #[ by itself is a passthrough.
 */
TEST(hash_bracket_passthrough)
{
    stringstream in("#[");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    auto special = scanner.get_token_string();
    TEST_EXPECT(string("#[") == special);
}

/**
 * #[= is a passthrough.
 */
TEST(hash_bracket_equals_passthrough)
{
    stringstream in("#[=");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    auto special = scanner.get_token_string();
    TEST_EXPECT(string("#[=") == special);
}

/**
 * #[something= is a passthrough.
 */
TEST(hash_bracket_something_equals_passthrough)
{
    stringstream in("#[xxx=");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    auto special = scanner.get_token_string();
    TEST_EXPECT(string("#[xxx=") == special);
}

/**
 * #[something=something is a passthrough.
 */
TEST(hash_bracket_something_equals_something_passthrough)
{
    stringstream in("#[xxx=xxx");
    lexer scanner(&in);

    TEST_ASSERT(MINWEB_TOKEN_PASSTHROUGH == scanner.read());
    auto special = scanner.get_token_string();
    TEST_EXPECT(string("#[xxx=xxx") == special);
}
