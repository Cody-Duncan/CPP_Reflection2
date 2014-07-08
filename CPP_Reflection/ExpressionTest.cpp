#include "expressionTest.h"
#include "expression.h"
#include "MacroHelpers.h"		// STR() macro
#include <iostream>

int g_i;

namespace ExpressionTest
{
	void BasicTest()
	{
		typedef expr::expression<decltype(&g_i)> test;
		#define TEST_EXPR is_static & is_variable & is_nonvariadic & no_return & no_arguments & is_trivial
		const char* testString = STR(TEST_EXPR);

		// Evaluate expression information...
		static size_t const is_static      = expr::is_static      <test>::value;
		static size_t const is_member      = expr::is_member      <test>::value;
		static size_t const is_variable    = expr::is_variable    <test>::value;
		static size_t const is_functional  = expr::is_functional  <test>::value;
		static size_t const is_trivial     = expr::is_trivial     <test>::value;
		static size_t const is_nontrivial  = expr::is_nontrivial  <test>::value;
		static size_t const no_return      = expr::no_return      <test>::value;
		static size_t const has_return     = expr::has_return     <test>::value;
		static size_t const no_arguments   = expr::no_arguments   <test>::value;
		static size_t const has_arguments  = expr::has_arguments  <test>::value;
		static size_t const argument_count = expr::argument_count <test>::value;
		static size_t const is_variadic    = expr::is_variadic    <test>::value;
		static size_t const is_nonvariadic = expr::is_nonvariadic <test>::value;

		printf("%s: \n%s\n\n", TEST_EXPR ? "This type IS" : "This type IS NOT", testString);
	}
}