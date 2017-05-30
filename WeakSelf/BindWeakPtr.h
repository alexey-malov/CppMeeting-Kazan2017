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

/// Generates a forwarding call wrapper for non-const member function. Member function will be called if pThis is not expired.
template<typename ReturnType, typename ClassType, typename... Params, typename... Args>
decltype(auto) BindWeakPtr(ReturnType(ClassType::*memberFn)(Params... args), std::weak_ptr<ClassType> pThis, Args... args)
{
	using Invoker = detail::WeakInvoker<ReturnType, ClassType, false, Params...>;

	Invoker invoker(memberFn, std::move(pThis));
	return std::bind(invoker, args...);
}

/// Generates a forwarding call wrapper for const member function. Member function will be called if pThis is not expired.
template<typename ReturnType, typename ClassType, typename... Params, typename... Args>
decltype(auto) BindWeakPtr(ReturnType(ClassType::*memberFn)(Params... args) const, std::weak_ptr<ClassType> pThis, Args... args)
{
	using Invoker = detail::WeakInvoker<ReturnType, ClassType, true, Params...>;

	Invoker invoker(memberFn, std::move(pThis));
	return std::bind(invoker, args...);
}

template<typename ReturnType, typename ClassType, typename... Params, typename... Args>
decltype(auto) BindWeakPtr(ReturnType(ClassType::*memberFn)(Params... args), std::shared_ptr<ClassType> const& pThis, Args... args)
{
	return BindWeakPtr(memberFn, std::weak_ptr<ClassType>(pThis), args...);
}

template<typename ReturnType, typename ClassType, typename... Params, typename... Args>
decltype(auto) BindWeakPtr(ReturnType(ClassType::*memberFn)(Params... args) const, std::shared_ptr<ClassType> const& pThis, Args... args)
{
	return BindWeakPtr(memberFn, std::weak_ptr<ClassType>(pThis), args...);
}
