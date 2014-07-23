#include "MetaProgrammingTests.h"
#include "Indices.h"
#include <iostream>

void SelectParameterTest()
{
	int e = 1;
	double b = 2.2;
	float c = 3.3f;
	unsigned int d = 4;

	int c2 = select_parameter<3>(e, b, c, d);
	std::cout << 
		"Values of parameters in test function. Should be 1, 2.2, 3.3, 4 in" << std::endl <<
		"    int, double, float, unsigned int" << std::endl <<
		select_parameter<0>(e, b, c, d) << " " <<
		select_parameter<1>(e, b, c, d) << " " <<
		select_parameter<2>(e, b, c, d) << " " <<
		select_parameter<3>(e, b, c, d) 
		<< std::endl;
}

template <unsigned int... Is>
void expand(indices<Is...>)
{
	int arr[] = {Is...};

	for(int i = 0; i < sizeof...(Is); ++i)
	{
		std::cout << arr[i] << std::endl;
	}
}

void IndicesExpansionTest()
{
	expand(build_indices<7>{});
}


//functionality for calling a function on a singular type in a parameter list

template<typename T>
const char* getName()
{
	return "unknown type";
}
template<> const char* getName<int>() { return "int"; }
template<> const char* getName<char>() { return "char"; }
template<> const char* getName<double>() { return "double"; }
template<> const char* getName<bool>() { return "bool"; }
template<> const char* getName<float>() { return "float"; }

template<typename T>
const char* make()
{
	return getName<T>();
}

template<typename... Args>
const char* maker(int index)
{
	//expand to typed function pointers
	static const char* (* const makeFuncs [])() =
	{
		&make<Args>...
	};

	std::cout << "NumArgs: " << sizeof(makeFuncs) / sizeof(makeFuncs[0]) << std::endl;

	//check range
	if (index < 0 || sizeof...(Args) <= index) 
	{
        throw std::range_error("type index out of range");
    }

	//grab the function pointer by index
	return makeFuncs[index]();
}

template<typename... Args>
const char* varFunc(int i)
{
	return maker<Args...>(i);
}

void GetParamTest()
{
	std::cout << varFunc<int, double, int, char, float>(0) << std::endl;
}




template<typename T>
struct make2
{
	static const char* get() { return getName<T>(); }
};

template<typename ReturnType, template<typename> class TFunctor, typename... Args>
ReturnType maker2(int index)
{
	//expand to typed function pointers
	static ReturnType (* const makeFuncs [])() =
	{
		&TFunctor<Args>::get...
	};

	std::cout << "NumArgs: " << sizeof(makeFuncs) / sizeof(makeFuncs[0]) << std::endl;

	//check range
	if (index < 0 || sizeof...(Args) <= index) 
	{
        throw std::range_error("type index out of range");
    }

	//grab the function pointer by index
	return makeFuncs[index]();
}

template<typename ReturnType, template<typename> class TFunctor, typename... Args>
ReturnType varFunc2(int i)
{
	return maker2<ReturnType, TFunctor, Args...>(i);
}


void GetParamtest2()
{
	std::cout << varFunc2<const char*, make2, int, double, int, char, float>(0) << std::endl;
}