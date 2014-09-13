#pragma once

#include <cstddef>
#include <type_traits>

template <unsigned int... Is>
struct indices {};

template <unsigned int N, unsigned int... Is>
struct build_indices : build_indices<N - 1, N - 1, Is...> {};

template <unsigned int... Is>
struct build_indices<0, Is...> : indices<Is...>{};


template<unsigned int... Is>
indices<Is...> toIndices(indices< Is...>& ind)
{
	return ind;
}


template <std::size_t N, typename... Args> struct select 
{ 
	static_assert(N < sizeof...(Args), "select<N, Args...>, index out of bounds.");
	typedef void type; // declare typedef to avoid template barf, assert should catch first
};
 
template <std::size_t N, typename T, typename... Args> 
struct select<N, T, Args...> 
{ 
	typedef typename select<N - 1, Args...>::type type; 
  
	static inline const type& get(const T&, const Args&... args)
	{ 
		return select<N - 1, Args...>::get(args...); 
	}
};
 
template <typename T, typename... Args> 
struct select<0, T, Args...> 
{ 
	typedef T type; 
 
	static inline const type& get(const T& a, const Args&...)
	{ 
		return a; 
	}
};

template<std::size_t N, typename... Args>
typename select<N, Args...>::type select_parameter(Args... args) { return select<N, Args...>::get(args...); }




template <typename T>
class has_type_function 
{
	private:
		template<typename U> static auto test(void*) -> decltype(std::declval<U>().type(), std::true_type());
		template<typename>   static auto test(...)   -> decltype(std::false_type());
 
	public:
		static bool const value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
};


template<typename ReturnType, template<typename> class TFunctor, typename... Args>
ReturnType CallFunctor_type_ByVariadicIndex_Select(int index)
{
	static_assert(has_type_function<TFunctor<int>>::value, "Functor does not contain static type() function.") ;

	//expand to typed function pointers
	static ReturnType (*makeFuncs[])() =
	{
		&TFunctor<Args>::type...
	};

	//check range
	if (index < 0 || sizeof...(Args) <= index) 
	{
        throw std::range_error("type index out of range");
    }

	//grab the function pointer by index
	return makeFuncs[index]();
}

template<typename ReturnType, template<typename> class TFunctor, typename... Args>
ReturnType CallFunctor_type_ByVariadicIndex(int i)
{
	return CallFunctor_type_ByVariadicIndex_Select<ReturnType, TFunctor, Args...>(i);
}