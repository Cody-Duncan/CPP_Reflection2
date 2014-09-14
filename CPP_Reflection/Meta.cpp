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

	const TypeData* Get_Name(std::string typeName)
	{
		unsigned int index = TypeData::GetTypeDataDictionary()->at(typeName);
		const TypeData* type = &TypeData::GetTypeDataStorage()->at(index);
		return type;
	}

	const TypeData* Get_Name(const char* typeName)
	{
		return Get_Name(std::string(typeName));
	}

	Member* TypeData::GetMember(std::string name)
	{
		return const_cast<Member*>(static_cast<const TypeData*>(this)->GetMember(name));
	}

	const Member* TypeData::GetMember(std::string name) const
	{
		auto result = std::find_if(m_members.begin(), m_members.end(), [&](const Member* a) -> bool
		{
			return a->GetNameStr() == name;
		});

		return result == m_members.end() ? nullptr : *result;
	}

	Method* TypeData::GetMethod(std::string name)
	{
		return const_cast<Method*>(static_cast<const TypeData*>(this)->GetMethod(name));
	}

	const Method* TypeData::GetMethod(std::string name) const
	{
		auto result = std::find_if(m_methods.begin(), m_methods.end(), [&](const Method* a) -> bool
		{
			return a->GetNameStr() == name;
		});

		return result == m_methods.end() ? nullptr : *result;
	}


}

meta_declare_primitive(int);
meta_declare_primitive(float);
meta_declare_primitive(char);
meta_declare_primitive(double);