#include <cstddef> // size_t
#include <type_traits> // true_type, false_type, is_const, is_same, is_base_ofs

namespace expr
{
	namespace detail
	{

		////////////////////////////////////////////////////////////////////////////////
		// opposite
		////////////////////////////////////////////////////////////////////////////////

		template <typename T, typename Enable = void>
		struct opposite;

		template <typename T>
		struct opposite<T, typename
			std::enable_if< std::is_base_of<std::true_type, T>::value>::type>
			: public std::false_type{};

		template <typename T>
		struct opposite<T, typename
			std::enable_if<std::is_base_of<std::false_type, T>::value>::type>
			: public std::true_type{};



		////////////////////////////////////////////////////////////////////////////////
		// type_list
		////////////////////////////////////////////////////////////////////////////////

		// A helper trait that takes a raw variadic list of types and gets the Nth type in the list.

		template <size_t Idx, typename... Args>
		struct type_list;

		template <size_t Idx, typename Current, typename... Args>
		struct type_list<Idx, Current, Args...>
		{
			typedef typename type_list<Idx - 1, Args...>::type type;
		};

		template <size_t Idx>
		struct type_list<Idx>
		{
			typedef void type;
		};

		template <typename Curr, typename... Args>
		struct type_list<0, Curr, Args...>
		{
			typedef Curr type;
		};


		////////////////////////////////////////////////////////////////////////////////
		// return_type
		////////////////////////////////////////////////////////////////////////////////

		template <typename T>
		struct return_type;

		template <>
		struct return_type<void>
		{
			typedef void type;
		};

		template <typename T>
		struct return_type : return_type<void>
		{
		};

		template <typename Ret, typename... Args>
		struct return_type<Ret(*)(Args...)>
		{
			typedef Ret type;
		};

		template <typename Ret, typename... Args>
		struct return_type<Ret(*)(Args..., ...)>
		{
			typedef Ret type;
		};

		template <typename Ret, typename Caller, typename... Args>
		struct return_type<Ret(Caller::*)(Args...)>
		{
			typedef Ret type;
		};

		template <typename Ret, typename Caller, typename... Args>
		struct return_type<Ret(Caller::*)(Args..., ...)>
		{
			typedef Ret type;
		};

		template <typename Ret, typename Caller, typename... Args>
		struct return_type<Ret(Caller::*)(Args...) const>
		{
			typedef Ret type;
		};

		template <typename Ret, typename Caller, typename... Args>
		struct return_type<Ret(Caller::*)(Args..., ...) const>
		{
			typedef Ret type;
		};

		////////////////////////////////////////////////////////////////////////////////
		// caller_type
		////////////////////////////////////////////////////////////////////////////////

		template <typename T>
		struct caller_type
		{
			typedef void type;
		};

		template <typename Ret, typename Caller>
		struct caller_type<Ret(Caller::*)>
		{
			typedef Caller type;
		};

		template <typename Ret, typename Caller, typename... Args>
		struct caller_type<Ret(Caller::*)(Args...)>
		{
			typedef Caller type;
		};

		template <typename Ret, typename Caller, typename... Args>
		struct caller_type<Ret(Caller::*)(Args..., ...)>
		{
			typedef Caller type;
		};

		template <typename Ret, typename Caller, typename... Args>
		struct caller_type<Ret(Caller::*)(Args...) const>
		{
			typedef Caller type;
		};

		template <typename Ret, typename Caller, typename... Args>
		struct caller_type<Ret(Caller::*)(Args..., ...) const>
		{
			typedef Caller type;
		};

		////////////////////////////////////////////////////////////////////////////////
		// is_functionally_const
		////////////////////////////////////////////////////////////////////////////////

		/*!
		 * @class is_functionally_const
		 * @file expressions
		 * A helper trait that checks const of a raw typename.
		 * This trait works differently from std::is_const, in that it checks whether a
		 * function is marked as const or not. If we are not dealing with a function or
		 * member pointer, value is set to `false`.
		 */
		template <typename T>
		struct is_functionally_const
		: public std::false_type
		{
		};

		template <typename Ret, typename Caller, typename... Args>
		struct is_functionally_const<Ret(Caller::*)(Args...) const>
		: public std::true_type
		{
		};

		template <typename Ret, typename Caller, typename... Args>
		struct is_functionally_const<Ret(Caller::*)(Args..., ...) const>
		: public std::true_type
		{
		};

		////////////////////////////////////////////////////////////////////////////////
		// is_variadic
		////////////////////////////////////////////////////////////////////////////////

		/*!
		 * @class is_variadic
		 * @file expressions
		 * A helper trait that checks variadics of a raw typename.
		 * This trait only works with functional types, as only functional types can be
		 * variadic. If a function has a variadic argument (...), value should be set
		 * to std::true_type. Otherwise, std::false_type.
		 */
		template <typename T>
		struct is_variadic
		: public std::false_type
		{
		};

		template <typename Ret, typename... Args>
		struct is_variadic<Ret(*)(Args..., ...)>
		: public std::true_type
		{
		};

		template <typename Ret, typename Caller, typename... Args>
		struct is_variadic<Ret(Caller::*)(Args..., ...)>
		: public std::true_type
		{
		};

		template <typename Ret, typename Caller, typename... Args>
		struct is_variadic<Ret(Caller::*)(Args..., ...) const>
		: public std::true_type
		{
		};

		////////////////////////////////////////////////////////////////////////////////
		// argument_type
		////////////////////////////////////////////////////////////////////////////////

		template <typename T, size_t Idx>
		struct argument_type
		{
			typedef void type;
		};

		template <size_t Idx, typename Ret, typename... Args>
		struct argument_type<Ret(*)(Args...), Idx>
		{
			typedef typename type_list<Idx, Args...>::type type;
		};

		template <size_t Idx, typename Ret, typename... Args>
		struct argument_type<Ret(*)(Args..., ...), Idx>
		{
			typedef typename type_list<Idx, Args...>::type type;
		};

		template <size_t Idx, typename Ret, typename Caller, typename... Args>
		struct argument_type<Ret(Caller::*)(Args...), Idx>
		{
			typedef typename type_list<Idx, Args...>::type type;
		};

		template <size_t Idx, typename Ret, typename Caller, typename... Args>
		struct argument_type<Ret(Caller::*)(Args..., ...), Idx>
		{
			typedef typename type_list<Idx, Args...>::type type;
		};

		template <size_t Idx, typename Ret, typename Caller, typename... Args>
		struct argument_type<Ret(Caller::*)(Args...) const, Idx>
		{
			typedef typename type_list<Idx, Args...>::type type;
		};

		template <size_t Idx, typename Ret, typename Caller, typename... Args>
		struct argument_type<Ret(Caller::*)(Args..., ...) const, Idx>
		{
			typedef typename type_list<Idx, Args...>::type type;
		};

		////////////////////////////////////////////////////////////////////////////////
		// argument_count
		////////////////////////////////////////////////////////////////////////////////

		template <typename T>
		struct argument_count
			: std::integral_constant<size_t, 0>
		{
		};

		template <typename Ret, typename... Args>
		struct argument_count<Ret(*)(Args...)>
			: std::integral_constant<size_t, sizeof...(Args)>
		{
		};

		template <typename Ret, typename... Args>
		struct argument_count<Ret(*)(Args..., ...)>
			: std::integral_constant<size_t, sizeof...(Args)>
		{
		};

		template <typename Ret, typename Caller, typename... Args>
		struct argument_count<Ret(Caller::*)(Args...)>
			: std::integral_constant<size_t, sizeof...(Args)>
		{
		};

		template <typename Ret, typename Caller, typename... Args>
		struct argument_count<Ret(Caller::*)(Args..., ...)>
			: std::integral_constant<size_t, sizeof...(Args)>
		{
		};

		template <typename Ret, typename Caller, typename... Args>
		struct argument_count<Ret(Caller::*)(Args...) const>
			: std::integral_constant<size_t, sizeof...(Args)>
		{
		};

		template <typename Ret, typename Caller, typename... Args>
		struct argument_count<Ret(Caller::*)(Args..., ...) const>
			: std::integral_constant<size_t, sizeof...(Args)>
		{
		};

	} // end namespace detail




	////////////////////////////////////////////////////////////////////////////////
	// expression
	////////////////////////////////////////////////////////////////////////////////

	// Global Value Expression
	template <typename Ret>
	struct expression
	{
		typedef Ret type;
	};

	// Static Function Substitution
	template <typename Ret, typename... Args>
	struct expression<Ret(*)(Args...)>
	{
		typedef Ret(*type)(Args...);
	};

	// Static Variadic Function Substitution
	template <typename Ret, typename... Args>
	struct expression<Ret(*)(Args..., ...)>
	{
		typedef Ret(*type)(Args..., ...);
	};

	// Non-Const Method Substitution
	template <typename Ret, typename Caller, typename... Args>
	struct expression<Ret(Caller::*)(Args...)>
	{
		typedef Ret(Caller::*type)(Args...);
	};

	// Const Method Substitution
	template <typename Ret, typename Caller, typename... Args>
	struct expression<Ret(Caller::*)(Args...) const>
	{
		typedef Ret(Caller::*type)(Args...) const;
	};

	// Non-Const Variadic Method Substitution
	template <typename Ret, typename Caller, typename... Args>
	struct expression<Ret(Caller::*)(Args..., ...)>
	{
		typedef Ret(Caller::*type)(Args..., ...);
	};

	// Const Method Variadic Substitution
	template <typename Ret, typename Caller, typename... Args>
	struct expression<Ret(Caller::*)(Args..., ...) const>
	{
		typedef Ret(Caller::*type)(Args..., ...) const;
	};

	// Global Pointer Substitution
	template <typename Ret>
	struct expression<Ret*>
	{
		typedef Ret* type;
	};

	// Member Pointer Substitution
	template <typename Ret, typename Caller>
	struct expression<Ret(Caller::*)>
	{
		typedef Ret(Caller::*type);
	};



	////////////////////////////////////////////////////////////////////////////////
	// evaluations
	////////////////////////////////////////////////////////////////////////////////


	/*!
	 * @class argument_count
	 * @file expressions
	 * Returns the argument_count struct of an expression.
	 * argument_count<expression>::value -> Number of arguments.
	 * argument_count<expression>::type -> Type of value.
	 */
	template <typename Info>
	using argument_count = typename detail::argument_count<typename Info::type>;

	/*!
	 * @class return_type
	 * @file expressions
	 * Returns the detail::return_type<expression::type>::type of an expression.
	 * This may be used directly as a type, as it is the raw typename of the return
	 * type.
	 */
	template <typename Info>
	using return_type = typename detail::return_type<typename Info::type>::type;

	/*!
	 * @class caller_type
	 * @file expressions
	 * Returns the detail::caller_type<expression::type>::type of an expression.
	 * This may be used directly as a type, as it is the raw typename of the caller
	 * type.
	 */
	template <typename Info>
	using caller_type = typename detail::caller_type<typename Info::type>::type;

	/*!
	 * @class argument_type
	 * @file expressions
	 * Returns the detail::argument_type<expression::type, Idx>::type of an expression.
	 * This may be used directly as a type, as it is the raw typename of the argument
	 * type.
	 */
	template <typename Info, size_t Idx>
	using argument_type = typename detail::argument_type<typename Info::type, Idx>::type;

	/*!
	 * @class return_expression
	 * @file expressions
	 * Same as expression<return_type<expression>>
	 */
	template <typename Info>
	using return_expression = expression<return_type<Info>>;

	/*!
	 * @class caller_expression
	 * @file expressions
	 * Same as expression<caller_type<expression>>
	 */
	template <typename Info>
	using caller_expression = expression<caller_type<Info>>;

	/*!
	 * @class argument_expression
	 * @file expressions
	 * Same as expression<argument_type<expression, Idx>>
	 */
	template <typename Info, size_t Idx>
	using argument_expression = expression<argument_type<Info, Idx>>;

	/*!
	 * @class is_static
	 * @file expressions
	 * An expression is considered static if it does not need a caller_type in order
	 * to be called. (Member functions, and member pointers are NOT static)
	 */
	template <typename Info>
	struct is_static
	: public std::is_same<caller_type<Info>, void>
	{
	};

	
	/*!
	 * @class is_member
	 * @file expressions
	 * Returns the opposite of what is_static<expression> would return.
	 */
	template <typename Info>
	struct is_member
	: public detail::opposite<is_static<Info>>
	{
	};

	/*!
	 * @class is_variable
	 * @file expressions
	 * An expression is considered variable if it cannot be acted upon as a
	 * function. (Global variables, member pointers, and global pointers are variable.)
	 */
	template <typename Info>
	struct is_variable
	: public std::is_base_of<detail::return_type<void>, detail::return_type<typename Info::type>>
	{
	};

	/*!
	 * @class is_functional
	 * @file expressions
	 * Returns the opposite of is_variable
	 */
	template <typename Info>
	struct is_functional
	: public detail::opposite<is_variable<Info>>
	{
	};

	/*!
	 * @class is_trivial
	 * @file expressions
	 * An expression is considered trivial if it has one or fewer arguments. Non-
	 * function types are considered to have no arguments, thus they are also
	 * trivial.
	 */
	template <typename Info>
	struct is_trivial
	: public std::is_same<argument_type<Info, 1>, void>
	{
	};

	/*!
	 * @class is_nontrivial
	 * @file expressions
	 * Returns the opposite of is_trivial.
	 */
	template <typename Info>
	struct is_nontrivial
	: public detail::opposite<is_trivial<Info>>
	{
	};

	/*!
	 * @class no_return
	 * @file expressions
	 * An expression that does not return a type will have a evaluation of no_return.
	 * If the expression does not represent a function, it is also no_return.
	 */
	template <typename Info>
	struct no_return
	: public std::is_same<return_type<Info>, void>
	{
	};

	/*!
	 * @class has_return
	 * @file expressions
	 * Returns the opposite of no_return.
	 */
	template <typename Info>
	struct has_return
	: public detail::opposite<no_return<Info>>
	{
	};

	/*!
	 * @class no_arguments
	 * @file expressions
	 * An expression who's first argument is `void` is said to have no arguments.
	 * Note that expressions representing non-functional types are automatically
	 * considered to have no_arguments.
	 */
	template <typename Info>
	struct no_arguments
	: public std::is_same<argument_type<Info, 0>, void>
	{
	};

	/*!
	 * @class has_arguments
	 * @file expressions
	 * Returns the opposite of no_arguments.
	 */
	template <typename Info>
	struct has_arguments
	: public detail::opposite<no_arguments<Info>>
	{
	};

	/*!
	 * @class no_arguments
	 * @file expressions
	 * An expression who's first argument is `void` is said to have no arguments.
	 * Note that expressions representing non-functional types are automatically
	 * considered to have no_arguments.
	 */
	template <typename Info>
	struct is_variadic
	: public detail::is_variadic<typename Info::type>
	{
	};

	/*!
	 * @class is_nonvariadic
	 * @file expressions
	 * Returns the opposite of is_variadic.
	 */
	template <typename Info>
	struct is_nonvariadic
	: public detail::opposite<is_variadic<Info>>
	{
	};

}


