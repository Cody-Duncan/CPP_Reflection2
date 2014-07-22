#pragma once

#include <cstddef>

template <unsigned int... Is>
struct indices {};

template <unsigned int N, unsigned int... Is>
struct build_indices : build_indices<N - 1, N - 1, Is...> {};

template <unsigned int... Is>
struct build_indices<0, Is...> : indices<Is...>{};




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