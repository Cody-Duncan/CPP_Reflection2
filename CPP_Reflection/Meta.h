#pragma once

#include <vector>
#include <unordered_map>

#define TYPEDATA_CONTAINER_SIZE 256

namespace meta
{
	class TypeData;

	namespace internal
	{
		template <typename T>
		struct TypeDataHolder;

		template<>
		struct TypeDataHolder<void>
		{
			static const TypeData s_TypeData;
		};
	}

	class TypeRecord
	{
		enum Qualifier
		{
			Q_Void,
			Q_Value,
			Q_Reference,
			Q_Pointer
		};

		TypeData* m_type;
		Qualifier m_qualifier;

		TypeRecord(TypeData* type, Qualifier qualifier) : m_type(type), m_qualifier(qualifier) {}
		TypeRecord() : m_type(nullptr), m_qualifier(Q_Void){}
	};

	class Member
	{
	private:
		const char* m_name;
		size_t m_size;
		TypeData* m_owner;
		TypeData* m_type;

	public:
		Member() : m_name(""), m_size(0) {}
		Member(const char* name, size_t size) : m_name(name), m_size(size) {}
		~Member() {}

		void SetOwner(TypeData* owner) { m_owner = owner; }
		TypeData* GetOwner() { return m_owner; }

		TypeData* GetType() { return m_type; }
		const char* GetTypeName() const;
		std::string GetTypeNameStr() const;

		const char* GetName() { return m_name; }
		std::string GetNameStr() { return std::string(m_name); }

		size_t GetSize() { return m_size; }
		
	};

	class Method
	{
	private:
		const char* m_name;
		TypeData* m_owner;

	public:
		Method() : m_name("") {}
		Method(const char* name) : m_name(name) {}
		~Method() {}

		void SetOwner(TypeData* owner) { m_owner = owner; }
		TypeData* GetOwner() { return m_owner; }

		const char* GetName() const { return m_name; }
		std::string GetNameStr() const { return std::string(m_name); }

		virtual int GetArity() const = 0;
		virtual TypeRecord GetReturnType() const = 0;
		virtual TypeRecord GetParamType(unsigned int i) const = 0;

		//Call()
		//CanCall()

	};

	class TypeData
	{
	private:
		const char* m_name;
		size_t m_size;

	protected:
		static std::unordered_map<std::string, TypeData>* sTypeDictionary;
		std::vector<Member*> m_members;
		std::vector<Method*> m_methods;

	public:
		static std::unordered_map<std::string, TypeData>* GetTypeDataStore()
		{
			if(sTypeDictionary == nullptr)
			{
				sTypeDictionary = new std::unordered_map<std::string, TypeData>();
				sTypeDictionary->reserve(TYPEDATA_CONTAINER_SIZE);
			}
			return sTypeDictionary;
		}

		TypeData() : m_name(""), m_size(0) {}
		
		TypeData(const char* name, size_t size) : 
			m_name(name), 
			m_size(size) 
		{}
		
		TypeData(TypeData&& rhs) : 
			m_name(rhs.m_name), 
			m_size(rhs.m_size), 
			m_members(std::move(rhs.m_members)), 
			m_methods(std::move(rhs.m_methods)) 
		{}
		
		~TypeData() {}

		const char* GetName() const { return m_name; }
		std::string GetNameStr() const { return std::string(m_name); }

		size_t GetSize() const { return m_size; }
	};

	class TypeData_Creator
	{
	private:
		TypeData* ref;
		TypeData_Creator() {}

	public:
		TypeData_Creator(const TypeData& rhs)
		{
			auto iter_bool = TypeData::GetTypeDataStore()->emplace(rhs.GetNameStr(), rhs);
			ref = &iter_bool.first->second;		//NOTE: if the map rehashes/resizes, this reference is invalidated. Make sure it's big enough.
		}

		TypeData_Creator(TypeData&& rhs)
		{
			auto iter_bool = TypeData::GetTypeDataStore()->emplace(rhs.GetNameStr(), rhs);
			ref = &iter_bool.first->second;		//NOTE: if the map rehashes/resizes, this reference is invalidated. Make sure it's big enough.
		}

		TypeData* Get() { return ref; }
	};

	// Holder for primitives and types that we cannot edit, and thus cannot create the typedata within.

	namespace internal
	{
		template <typename T>
		struct TypeDataHolder
		{
			static TypeDataHolder s_TypeData;
		};
	}
}

#define meta_declare(T)																						\
	public:																									\
		struct PrivateTypeDataCreator { static const meta::TypeData_Creator s_TypeData; };					\
		virtual const meta::TypeData* GetType() const { return PrivateTypeDataCreator::s_TypeData.Get(); }

#define meta_declare_primitive(T)	\
	template<> const meta::TypeData_Creator meta::internal::TypeDataHolder<T>::s_TypeData = meta::internal::TypeDataBuilder<T, !std::is_fundamental<T>::value>(#T, sizeof(T))

#define meta_define(T) \
	const meta::TypeData_Creator T::PrivateTypeDataCreator::s_TypeData = meta::internal::TypeDataBuilder<T, !std::is_fundamental<T>::value>(#T, sizeof(T))
