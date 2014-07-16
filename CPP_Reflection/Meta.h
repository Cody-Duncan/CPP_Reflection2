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
	public:
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
		const TypeData* m_owner;
		const TypeData* m_type;

	public:
		Member() : m_name(""), m_type(nullptr) {}
		Member(const char* name, const TypeData* type) : m_name(name), m_type(type) {}
		~Member() {}

		void SetOwner(TypeData* owner) { m_owner = owner; }
		const TypeData* GetOwner() { return m_owner; }

		const TypeData* GetType() { return m_type; }

		const char* GetTypeName() const;
		std::string GetTypeNameStr() const;

		const char* GetName() { return m_name; }
		std::string GetNameStr() { return std::string(m_name); }

		size_t GetSize();
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
		static std::vector<TypeData> s_TypeDataStorage;
		static std::unordered_map<std::string, unsigned int> sTypeDictionary;
		std::vector<Member*> m_members;
		std::vector<Method*> m_methods;

	public:
		friend class TypeData_Creator;
		
		//STATIC

		static const std::vector<TypeData>* GetTypeDataStorage()
		{
			return &s_TypeDataStorage;
		}

		static const std::unordered_map<std::string, unsigned int>* GetTypeDataDictionary()
		{
			return &sTypeDictionary;
		}

		static int AddTypeData(const char* name, size_t size)
		{
			s_TypeDataStorage.emplace_back(name, size);

			unsigned int lastIndex = s_TypeDataStorage.size() - 1;
			sTypeDictionary.insert( std::make_pair(name, lastIndex) );

			return lastIndex;
		}

		//Constructors
		
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


		//functions

		const char* GetName() const { return m_name; }
		std::string GetNameStr() const { return std::string(m_name); }

		size_t GetSize() const { return m_size; }
	};

	class TypeData_Creator
	{
	private:
		unsigned int refIndex;
		TypeData_Creator() {}

	public:
		TypeData_Creator(const TypeData& rhs)
		{
			unsigned int index = TypeData::AddTypeData(rhs.GetName(), rhs.GetSize());
			refIndex = index;
		}

		TypeData_Creator(TypeData&& rhs)
		{
			unsigned int index = TypeData::AddTypeData(rhs.GetName(), rhs.GetSize());
			refIndex = index;
		}

		const TypeData* Get() const
		{
			return &(*TypeData::GetTypeDataStorage())[refIndex];
		}
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

	namespace internal
	{
		// ********** Concrete Member **********

		template<typename Object, typename T>
		class ConcreteMember : public Member
		{
		protected:
			T Object::*m_memberPtr;

		public:
			ConcreteMember(const char* name, const TypeData* T_data, T Object::*memberVar) :
				Member(name, T_data),
				m_memberPtr(memberVar)
			{}
		};


		// ********** VarMethod (Concrete Method) **********

		template<typename ReturnType, typename Object, typename... Args>
		class VarMethod : public Method
		{
			ReturnType(Object::*m_methodPtr)(Args...);
		public:
			VarMethod(const char* name, ReturnType(Object::*method)(Args...)) :
				Method(name),
				m_methodPtr(method)
			{}

			virtual int GetArity() const { return sizeof...(Args); }
			virtual TypeRecord GetReturnType() const { return TypeRecord();  } //TODO
			virtual TypeRecord GetParamType(unsigned int i) const { return TypeRecord(); } //TODO
		};

		template<typename Object, typename ReturnType, typename... Args>
		Method* createMethod(const char* name, ReturnType(Object::*method)(Args...))
		{
			return new meta::internal::VarMethod<ReturnType, Object, Args...>(name, method);
		}


		// ********** TypeDataBuilder **********

		template <typename Object, bool IsClass>
		class TypeDataBuilder : public TypeData
		{
		public:
			TypeDataBuilder(const char* name, size_t size) : TypeData(name, size) {}

			template<typename T> 
			typename std::enable_if<!std::is_member_function_pointer<T>::value, TypeDataBuilder&>::type member(const char* name, T Object::*memberVar )
			{
				TypeData* t_data = nullptr; //TODO Get<T>();
				m_members.push_back(new ConcreteMember<Object, typename std::remove_reference<T>::type>(name, t_data, memberVar));
				return *this;
			}

			template<typename ReturnType, typename... Args>
			TypeDataBuilder& method(const char* name, ReturnType(Object::*method)(Args...) )
			{
				m_methods.push_back(createMethod(name, method));
				return *this;
			}
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
