#pragma once

#include <vector>

///<summary> A vector with a defined starting capacity </summary
///<remarks> What is really needed for Meta.h is a vector that cannot reallocate, but I can use this as a scaffold and build that later</remarks>
template<typename T>
class static_vector : public std::vector<T>
{
public:
	static_vector(size_type max_size) : 
		std::vector<T>()
	{
		reserve(max_size);
	}
};