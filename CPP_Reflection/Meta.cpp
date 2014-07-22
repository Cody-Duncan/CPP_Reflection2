#include "Meta.h"

namespace meta
{
	const TypeData internal::TypeDataHolder<void>::s_TypeData("void", 0);

	size_t Member::GetSize()
	{
		return m_type->GetSize();
	}

	std::vector<TypeData> TypeData::s_TypeDataStorage;
	std::unordered_map<std::string, unsigned int> TypeData::sTypeDictionary;
}

meta_declare_primitive(int);
meta_declare_primitive(float);
meta_declare_primitive(char);