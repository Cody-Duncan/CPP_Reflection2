#include "expression.h"
#include "AnyTest.h"
#include "ExpressionTest.h"
#include "MetaTest.h"
#include "MetaProgrammingTests.h"

template<typename ReturnType>
ReturnType a() { printf("hello");  return; }

int main(int argc, const char* argv[])
{
	a<void>();

	//AnyTest::BasicTest();
	//ExpressionTest::BasicTest();
	MetaTest::Test1();

	//IndicesExpansionTest();
	
	//GetParamtest2();

	return 0;
}