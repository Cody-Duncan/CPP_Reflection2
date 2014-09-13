#include "Meta.h"

namespace meta
{
	const TypeData internal::TypeDataHolder<void>::s_TypeData("void", 0);

	size_t Member::GetSize()
	{
		return m_type->GetSize();
	}

	static_vector<TypeData> TypeData::s_TypeDataStorage(256);

	std::unordered_map<std::string, unsigned int> TypeData::sTypeDictionary = [=]()-> std::unordered_map<std::string, unsigned int>
	{
		std::unordered_map<std::string, unsigned int> map;

		// Max load factor is 1.0, so 2.5 is big enough to allow TYPEDATA_CONTAINER_SIZE elements without resizing.
		typedef std::unordered_map<std::string, unsigned int>::size_type map_size_t;
		map_size_t reservedSize = (map_size_t)(TYPEDATA_CONTAINER_SIZE*2.5f);
		map.reserve(reservedSize);

		return std::move(map);
	}();
}

meta_declare_primitive(int);
meta_declare_primitive(float);
meta_declare_primitive(char);
meta_declare_primitive(double);