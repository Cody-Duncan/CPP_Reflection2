#include "Meta.h"

namespace meta
{
	
	size_t Member::GetSize()
	{
		return m_type->GetSize();
	}

	std::vector<TypeData> TypeData::s_TypeDataStorage;
	std::unordered_map<std::string, unsigned int> TypeData::sTypeDictionary;
}