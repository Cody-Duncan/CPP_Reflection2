#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include "Any.h"

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

	/****************************************************************/
	//                      Meta Lookup / Get                       //
	/****************************************************************/

	template <typename Type>
	struct Remove_Ptr_Ref
	{
		typedef typename std::remove_const<typename std::remove_reference<Type>::type>::type type;
	};


	//has meta

	template <typename T>
	class has_get_meta 
	{
		private:
			template<typename U> static auto test(void*) -> decltype(std::declval<U>().GetType(), std::true_type());
			template<typename>   static auto test(...)   -> decltype(std::false_type());
 
		public:
			static bool const value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
	};

	template <typename T>
	class has_meta 
	{
		private:
			template<typename U> static auto test(void*) -> decltype(std::declval<typename U::TypeDataStaticHolder>().s_TypeData, std::true_type());
			template<typename>   static auto test(...)   -> decltype(std::false_type());
 
		public:
			static bool const value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
	};

	//meta lookup
	
	template<typename T, bool HasMeta> struct meta_lookup
	{
		static const TypeData* Get()
		{
			return T::TypeDataStaticHolder::s_TypeData.Get();
		}
	};

	template<typename T> struct meta_lookup<T, false>
	{
		static const TypeData* Get()
		{
			return internal::TypeDataHolder<typename Remove_Ptr_Ref<T>::type>::s_TypeData.Get();
		}
	};

	template<> struct meta_lookup<void, false>
	{
		static const TypeData* Get()
		{
			return &(internal::TypeDataHolder<void>::s_TypeData);
		}
	};

	template<> struct meta_lookup<nullptr_t, false>
	{
		static const TypeData* Get()
		{
			return nullptr; //not sure what to return here
		}
	};

	//Get By Type
	template <typename T>
	const TypeData* Get()
	{
		return meta_lookup<T, has_meta<T>::value>::Get();
	}

	//Get From Pointer

	template <typename T>
	typename std::enable_if<has_get_meta<T>::value, const TypeData*>::type Get(const T* type)
	{
		return type->GetType();
	}

	template <typename T>
	typename std::enable_if<!has_get_meta<T>::value, const TypeData*>::type Get(const T* type)
	{
		return Get<T>();
	}

	//Get From Reference

	template <typename T>
	typename std::enable_if<!std::is_pointer<T>::value && has_get_meta<T>::value, const TypeData*>::type Get(const T& type)
	{
		return type.GetType();
	}

	template <typename T>
	typename std::enable_if<!std::is_pointer<T>::value && !has_get_meta<T>::value, const TypeData*>::type Get(const T& type)
	{
		return Get<T>();
	}

	/*********************************************************/
	//                      TypeRecord                       //
	/*********************************************************/

	class TypeRecord
	{
	public:
		enum Qualifier
		{
			Q_Void,
			Q_Value,
			Q_Reference,
			Q_ConstReference,
			Q_Pointer,
			Q_ConstPointer
		};

		const TypeData* m_type;
		Qualifier m_qualifier;

		TypeRecord(const TypeData* type, Qualifier qualifier) : m_type(type), m_qualifier(qualifier) {}
		TypeRecord() : m_type(nullptr), m_qualifier(Q_Void){}
	};

	template <typename T> struct make_type_record
	{
		static const TypeRecord type()
		{
			return TypeRecord(Get<T>(), TypeRecord::Q_Value);
		}
	};

	template <typename T> struct make_type_record<T*>
	{
		static const TypeRecord type()
		{
			return TypeRecord(Get<T>(), TypeRecord::Q_Pointer);
		}
	};

	template <typename T> struct make_type_record<const T*>
	{
		static const TypeRecord type()
		{
			return TypeRecord(Get<T>(), TypeRecord::Q_ConstPointer);
		}
	};

	template <typename T> struct make_type_record<T&>
	{
		static const TypeRecord type()
		{
			return TypeRecord(Get<T>(), TypeRecord::Q_Reference);
		}
	};

	template <typename T> struct make_type_record<const T&>
	{
		static const TypeRecord type()
		{
			return TypeRecord(Get<T>(), TypeRecord::Q_ConstReference);
		}
	};

		template <> struct make_type_record<void>
	{
		static const TypeRecord type()
		{
			return TypeRecord(Get<void>(), TypeRecord::Q_Void);
		}
	};

	// Make a TypeRecord from a specific type in variadic template, specified by index

	template<typename... Args>
	const TypeRecord make_type_record_byVariadicIndex(int i)
	{
		return CallFunctor_type_ByVariadicIndex<const TypeRecord, make_type_record, Args...>(i);
	}

	//overload for functions with 0 arguments.

	template<>
	inline const TypeRecord make_type_record_byVariadicIndex<>(int i)
	{
		return TypeRecord(Get<void>(), TypeRecord::Qualifier::Q_Void);
	}

	/*****************************************************/
	//                      Member                       //
	/*****************************************************/
	class Member
	{
	private:
		const char*     m_name;
		const TypeData* m_owner;
		const TypeData* m_type;

	public:
		Member() : m_name(""), m_type(nullptr) {}
		Member(const char* name, const TypeData* type) : m_name(name), m_type(type) {}
		
		Member(const Member& mem) :
			m_name(mem.m_name), 
			m_owner(mem.m_owner), 
			m_type(mem.m_type) 
		{}

		Member(Member&& mem) : 
			m_name(mem.m_name), 
			m_owner(mem.m_owner), 
			m_type(mem.m_type) 
		{
			mem.m_name = "";
			mem.m_owner = nullptr;
			mem.m_type = nullptr;
		}
		~Member() {}

		void SetOwner(TypeData* owner) { m_owner = owner; }
		const TypeData* GetOwner() { return m_owner; }

		const TypeData* GetType() const { return m_type; }

		const char* GetTypeName() const;
		std::string GetTypeNameStr() const;

		const char* GetName() const { return m_name; }
		std::string GetNameStr() { return std::string(m_name); }

		size_t GetSize();
	};




	/*****************************************************/
	//                      Method                       //
	/*****************************************************/

	class Method
	{
	private:
		const char* m_name;
		TypeData* m_owner;

	public:
		Method() : m_name("") {}
		Method(const char* name) : m_name(name) {}
		Method(const Method& mem) : m_name(mem.m_name), m_owner(mem.m_owner)
		{}

		Method(Method&& mem) :
			m_name(mem.m_name),
			m_owner(mem.m_owner)
		{
			mem.m_name = "";
			mem.m_owner = nullptr;
		}

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



	/*****************************************************/
	//                     TypeData                      //
	/*****************************************************/

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

		static int AddTypeData(const TypeData& rhs)
		{
			s_TypeDataStorage.emplace_back(rhs);

			unsigned int lastIndex = s_TypeDataStorage.size() - 1;
			sTypeDictionary.insert( std::make_pair(rhs.m_name, lastIndex) );

			return lastIndex;
		}

		static int AddTypeData(TypeData&& rhs)
		{
			s_TypeDataStorage.emplace_back(std::move(rhs));

			unsigned int lastIndex = s_TypeDataStorage.size() - 1;
			sTypeDictionary.insert( std::make_pair(s_TypeDataStorage[lastIndex].m_name, lastIndex) );

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
			m_members(rhs.m_members), 
			m_methods(rhs.m_methods) 
		{
			std::string name = rhs.GetName();
			std::for_each(m_members.begin(), m_members.end(), [&](Member* &mem)  { mem->SetOwner(this); });
			std::for_each(m_methods.begin(), m_methods.end(), [&](Method* &mthd) { mthd->SetOwner(this); });
		}
		
		~TypeData() {}


		//functions

		const char* GetName() const { return m_name; }
		std::string GetNameStr() const { return std::string(m_name); }

		size_t GetSize() const { return m_size; }

		std::vector<Member*>&       GetMembers()       { return m_members; }
		const std::vector<Member*>& GetMembers() const { return m_members; }

		std::vector<Method*>&       GetMethods()       { return m_methods; }
		const std::vector<Method*>& GetMethods() const { return m_methods; }
	};






	/*****************************************************/
	//                 TypeData_Creator                  //
	/*****************************************************/
	class TypeData_Creator
	{
	private:
		unsigned int refIndex;
		TypeData_Creator() {}

	public:
		TypeData_Creator(const TypeData& rhs)
		{
			//TODO: this AddTypeData doesn't cover members and methods
			unsigned int index = TypeData::AddTypeData(rhs);
			refIndex = index;
		}

		TypeData_Creator(TypeData&& rhs)
		{
			unsigned int index = TypeData::AddTypeData(std::move(rhs));
			refIndex = index;
		}

		const TypeData* Get() const
		{
			return &(*TypeData::GetTypeDataStorage())[refIndex];
		}
	};

	/**************************************************/
	//                 TypeDataHolder                 //
	/**************************************************/

	namespace internal
	{
		template <typename T>
		struct TypeDataHolder
		{
			static const TypeData_Creator s_TypeData;
		};
	}

	namespace internal
	{
		/**************************************************/
		//                 ConcreteMember                 //
		/**************************************************/

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


		/***************************************************************/
		//                 VerMethod (Concrete Method)                 //
		/***************************************************************/
		
		template<typename ReturnT, typename Object, bool isConst, typename... Args>
		class MethodPtr
		{
		};

		template<typename ReturnT, typename Object, typename... Args>
		class MethodPtr<ReturnT, Object, false, Args...>
		{
		public:
			typedef ReturnT(Object::*MethodPointerT)(Args...);
		};

		template<typename ReturnT, typename Object, typename... Args>
		class MethodPtr<ReturnT, Object, true, Args...>
		{
		public:
			typedef ReturnT(Object::*MethodPointerT)(Args...) const;
		};
		
		//This expands the array of Any objects, casts them to thier appropriate types, and sends them as arguements to the member function.
		//Note indicesT is only there because it separates the Is and Args parameter packs. This cannot work with a struct without a wrapper like tuple to hold the packs.
		template<typename ObjectT, typename ReturnT, unsigned int... Is, template <unsigned int...> class indicesT, typename... Args>
		ReturnT Call_Internal(ReturnT(ObjectT::*method)(Args...), ObjectT* obj, Any* argv, indicesT<Is...> indices)
		{
			return (obj->*method)(*argv[Is].getPointer<Args>()...);
		}

		//This creates the indices trick, to create a pack of indices for referencing the elements in the argv array.
		template <typename ObjectT, typename ReturnT, typename... Args>
		ReturnT Call(ReturnT(ObjectT::*method)(Args...), ObjectT* obj, Any* argv)
		{
			assert((sizeof...(Args) >  0 && argv != nullptr) ||		        // if has args, must not have null argv.
					(sizeof...(Args) == 0 && argv == nullptr)    );			// if no args, must have null argv.
			return Call_Internal(method, obj, argv, toIndices(build_indices<sizeof...(Args)>{}));
		}

		// CONST METHODS //

		template<typename ObjectT, typename ReturnT, unsigned int... Is, template <unsigned int...> class indicesT, typename... Args>
		ReturnT Call_Internal(ReturnT(ObjectT::*method)(Args...) const, ObjectT* obj, Any* argv, indicesT<Is...> indices)
		{
			return (obj->*method)(*argv[Is].getPointer<Args>()...);
		}

		//This creates the indices trick, to create a pack of indices for referencing the elements in the argv array.
		template <typename ObjectT, typename ReturnT, typename... Args>
		ReturnT Call(ReturnT(ObjectT::*method)(Args...) const, ObjectT* obj, Any* argv)
		{
			assert((sizeof...(Args) >  0 && argv != nullptr) ||		        // if has args, must not have null argv.
					(sizeof...(Args) == 0 && argv == nullptr)    );			// if no args, must have null argv.
			return Call_Internal(method, obj, argv, toIndices(build_indices<sizeof...(Args)>{}));
		}
		

		// VarMethod - Return Type
		template<typename ReturnT, typename Object, bool isConst, typename... Args>
		class VarMethod : public Method
		{
			typedef typename MethodPtr<ReturnT, Object, isConst, Args...>::MethodPointerT MethodPointerT;
			MethodPointerT m_methodPtr;
		public:
			VarMethod(const char* name, MethodPointerT method) :
				Method(name),
				m_methodPtr(method)
			{}

			virtual int GetArity() const { return sizeof...(Args); }
			virtual TypeRecord GetReturnType() const { return make_type_record<ReturnT>::type(); } 
			virtual TypeRecord GetParamType(unsigned int i) const 
			{ 
				return make_type_record_byVariadicIndex<Args...>(i);
			} 

			virtual Any DoCall(Any& obj, Any* argv) const 
			{
				return make_any<ReturnT>::make(meta::internal::Call(m_methodPtr, obj.getPointer<Object>(), argv));
			}
		};

		
		// VarMethod - void Return
		template<typename Object, bool isConst, typename... Args>
		class VarMethod<void, Object, isConst, Args...> : public Method
		{
			typedef typename MethodPtr<void, Object, isConst, Args...>::MethodPointerT MethodPointerT;
			MethodPointerT m_methodPtr;
		public:
			VarMethod(const char* name, MethodPointerT method) :
				Method(name),
				m_methodPtr(method)
			{}

			virtual int GetArity() const { return sizeof...(Args); }
			virtual TypeRecord GetReturnType() const { return make_type_record<void>::type(); } 
			virtual TypeRecord GetParamType(unsigned int i) const 
			{ 
				return make_type_record_byVariadicIndex<Args...>(i);
			} 

			//void return
			virtual Any DoCall(Any& obj, Any* argv) const 
			{
				meta::internal::Call(m_methodPtr, obj.getPointer<Object>(), argv);
				return Any();
			}
		};

		// Saves a function pointer inside a VarMethod
		template<typename Object, typename ReturnType, typename... Args>
		Method* createMethod(const char* name, ReturnType(Object::*method)(Args...))
		{
			return new meta::internal::VarMethod<ReturnType, Object, false, Args...>(name, method);
		}

		template<typename Object, typename ReturnType, typename... Args>
		Method* createMethod(const char* name, ReturnType(Object::*method)(Args...) const)
		{
			return new meta::internal::VarMethod<ReturnType, Object, true, Args...>(name, method);
		}



		
		/**************************************************************/
		//                      TypeDataBuilder                       //
		/**************************************************************/

		template <typename Object, bool IsClass>
		struct TypeDataBuilder : public TypeData
		{
			TypeDataBuilder(const char* name, size_t size) : TypeData(name, size) {}

			template<typename T> 
			typename std::enable_if<!std::is_member_function_pointer<T>::value, TypeDataBuilder&>::type member(const char* name, T Object::*memberVar )
			{
				const TypeData* t_data = meta::Get<T>();
				m_members.push_back(new ConcreteMember<Object, typename std::remove_reference<T>::type>(name, t_data, memberVar));
				
				return *this;
			}

			template<typename ReturnType, typename... Args>
			TypeDataBuilder& method(const char* name, ReturnType(Object::*method)(Args...) )
			{
				m_methods.push_back(createMethod(name, method));
				return *this;
			}

			template<typename ReturnType, typename... Args>
			TypeDataBuilder& method(const char* name, ReturnType(Object::*method)(Args...) const )
			{
				m_methods.push_back(createMethod(name, method));
				return *this;
			}

			TypeDataBuilder&& finish()
			{
				return std::move(*this);
			}
		};

		//specialized for pointer types (cannot have members or methods)
		template <typename Object> 
		struct TypeDataBuilder<Object*, true> : public TypeData
		{
			TypeDataBuilder(const char* name, size_t size) : TypeData(name, size) 
			{}
		};

		//specialized for primitive types (cannot have members or methods)
		template <typename Object> 
		struct TypeDataBuilder<Object, false> : public TypeData
		{
			TypeDataBuilder(const char* name, size_t size) : TypeData(name, size) 
			{}
		};
	}
}

/**************************************************************************/
//                      Reflection Data Building API                      //
/**************************************************************************/

/// Declares meta information internally to a type.
#define meta_declare(T)																						\
	public:																									\
		struct TypeDataStaticHolder { static const meta::TypeData_Creator s_TypeData; };					\
		virtual const meta::TypeData* GetType() const { return TypeDataStaticHolder::s_TypeData.Get(); }


/// Declares and Defines meta information externally to a type.
#define meta_declare_primitive(T)	\
	template<> const meta::TypeData_Creator meta::internal::TypeDataHolder<T>::s_TypeData = meta::internal::TypeDataBuilder<T, !std::is_fundamental<T>::value>(#T, sizeof(T))


/// Defines meta information externally. Pair with meta_declare.
#define meta_define(T) \
	const meta::TypeData_Creator T::TypeDataStaticHolder::s_TypeData = meta::internal::TypeDataBuilder<T, !std::is_fundamental<T>::value>(#T, sizeof(T))
