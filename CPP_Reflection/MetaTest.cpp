#include "MetaTest.h"
#include "Meta.h"
#include <iostream>
#include <assert.h>
#include "MacroHelpers.h"
#include "AnyTest.h"
#include "Indices.h"


namespace MetaTest
{
	// a test class
	class A1
	{
		int a;
		float b;
		char c;

		int bar(float method) { return (int)(method * 0.5f); }
		float baz(double d, char is_const) { return d > (double)is_const ? (float)(d * 0.5) : 0.f; }

	public:
		A1() : a(0), b(0.0f), c(0) {}

		int getA() const { return a; }
		void setA(int _) { a = _; }

		float getB() const { return b; }

		void foo() { std::cout << "FOO()" << std::endl; a *= 3; }

		void stuff(int a, double b, char c) const { std::cout << "STUFF()" << std::endl; std::cout << a + b + c << std::endl; }

		//meta_declare(A1);

		struct TypeDataStaticHolder { static const meta::TypeData_Creator s_TypeData; };					\
		virtual const meta::TypeData* GetType() const { return TypeDataStaticHolder::s_TypeData.Get(); }

	};

	meta_define(A1)
		.member("a", &A1::a) // note that we can bind private m_Members with this style of Get
		.member("b", &A1::b)
		.member("c", &A1::c)
		.method("foo", &A1::foo)
		.method("bar", &A1::bar)
		.method("baz", &A1::baz)
		.method("stuff", &A1::stuff)
		.finish();


	void Test1()
	{
		meta::has_getType_function<A1>::value ;

		meta::has_meta<A1>::value ;

		A1 a;
		a.setA(12);


		

		//check A1 type
		const meta::TypeData* aInfo = meta::Get<A1>();
		std::cout << "Info on type: " <<  aInfo->GetName() << std::endl;
		assert(meta::Get<A1>()->GetSize() == sizeof (A1));


		if(aInfo->GetMethods().size() > 0 && aInfo->GetMethods()[1]->GetArity())
		{
			std::cout << "Getting " <<  aInfo->GetMethods()[1]->GetNameStr() << "'s first param type: ";
			std::cout <<  aInfo->GetMethods()[1]->GetParamType(0).m_type->GetNameStr() << std::endl;
		}


		//check A1 members
		for(unsigned int i = 0; i < aInfo->GetMembers().size(); ++i)
		{
			const meta::Member* m = aInfo->GetMembers()[i];
			std::cout << m->GetType()->GetName() << " " << m->GetName() << std::endl;
		}

		//Check A1 methods
		for(unsigned int i = 0; i < aInfo->GetMethods().size(); ++i)
		{
			const meta::Method* m = aInfo->GetMethods()[i];
			std::cout << m->GetReturnType().m_type->GetName() << " " << m->GetName() << "(";
			for(int k = 0; k < m->GetArity(); ++k)
			{
				auto typedata = m->GetParamType(k).m_type;
				std::cout << typedata->GetName();
				if(k != m->GetArity() - 1)
				{
					std::cout << ", ";
				}
			}
			std::cout << ")" << std::endl;
		}

		//Test function invocation.
		float arg = 4.0f;
		int expected = 2;
		Any result = meta::Invoke(aInfo->GetMethod("bar"), a, 4.0f);
		int resultValue = result.cast<int>();
		std::cout << "Ran A1::bar() with argument " << 4.0f << ". Result should be " << expected << ". Result is: " << resultValue << std::endl;

		//Retrieve via name
		const meta::TypeData* aInfoAgain = meta::Get_Name("A1");
		std::cout << "Retrieved type via name. Tried to get A1" << "; recieved " << aInfoAgain->GetNameStr() << std::endl;

	}
}