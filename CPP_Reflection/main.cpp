#include "expression.h"
#include "AnyTest.h"
#include "ExpressionTest.h"
#include "MetaTest.h"
#include "MetaProgrammingTests.h"

int main(int argc, const char* argv[])
{
	AnyTest::BasicTest();
	ExpressionTest::BasicTest();
	MetaTest::Test1();

	IndicesExpansionTest();
	GetParamtest2();

	return 0;
}