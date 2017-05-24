#pragma once
#include <functional>
#include <memory>

namespace detail
{
	template<class ReturnType, class ClassType, bool AddConst, class... Args>
	struct WeakInvoker
	{
		using ConstMethodType = ReturnType(ClassType::*)(Args... args) const;
		using NonConstMethodType = ReturnType(ClassType::*)(Args... args);
		using MethodType = std::conditional_t<AddConst, ConstMethodType, NonConstMethodType>;
		using WeakPtrType = std::weak_ptr<ClassType>;

		WeakInvoker(MethodType pMethod, WeakPtrType&& pObject)
			: m_pMethod(pMethod)
			, m_pObject(pObject)
		{
		}

		ReturnType operator()(Args... args) const
		{
			if (auto pThis = m_pObject.lock())
			{
				return (pThis.get()->*m_pMethod)(std::forward<Args>(args)...);
			}
			return ReturnType();
		}

		MethodType m_pMethod;
		WeakPtrType m_pObject;
	};
}

/// Weak this binding of non-const methods on weak_ptr
template<typename ReturnType, typename ClassType, typename... Params, typename... Args>
decltype(auto) BindWeakPtr(ReturnType(ClassType::*memberFn)(Params... args), std::weak_ptr<ClassType> pThis, Args... args)
{
	using Invoker = detail::WeakInvoker<ReturnType, ClassType, false, Params...>;

	Invoker invoker(memberFn, std::move(pThis));
	return std::bind(invoker, args...);
}

/// Weak this binding of const methods on weak_ptr
template<typename ReturnType, typename ClassType, typename... Params, typename... Args>
decltype(auto) BindWeakPtr(ReturnType(ClassType::*memberFn)(Params... args) const, std::weak_ptr<ClassType> pThis, Args... args)
{
	using Invoker = detail::WeakInvoker<ReturnType, ClassType, true, Params...>;

	Invoker invoker(memberFn, std::move(pThis));
	return std::bind(invoker, args...);
}

/// Weak this binding of non-const methods via shared_ptr
template<typename ReturnType, typename ClassType, typename... Params, typename... Args>
decltype(auto) BindWeakPtr(ReturnType(ClassType::*memberFn)(Params... args), std::shared_ptr<ClassType> const& pThis, Args... args)
{
	return BindWeakPtr(memberFn, std::weak_ptr<ClassType>(pThis), args...);
		/*
	using Invoker = detail::WeakInvoker<ReturnType, ClassType, false, Params...>;

	Invoker invoker(memberFn, std::weak_ptr<ClassType>(pThis));
	return std::bind(invoker, args...); */
}

/// Weak this binding of const methods via shared_ptr
template<typename ReturnType, typename ClassType, typename... Params, typename... Args>
decltype(auto) BindWeakPtr(ReturnType(ClassType::*memberFn)(Params... args) const, std::shared_ptr<ClassType> const& pThis, Args... args)
{
	return BindWeakPtr(memberFn, std::weak_ptr<ClassType>(pThis), args...);

	/*
	using Invoker = detail::WeakInvoker<ReturnType, ClassType, true, Params...>;

	Invoker invoker(memberFn, std::weak_ptr<ClassType>(pThis));
	return std::bind(invoker, args...); */
}