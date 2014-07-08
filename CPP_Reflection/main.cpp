#include "expression.h"
#include "AnyTest.h"
#include "ExpressionTest.h"
#include "MetaTest.h"

int main(int argc, const char* argv[])
{
	AnyTest::BasicTest();
	ExpressionTest::BasicTest();
	MetaTest::Test1();
	
	return 0;
}