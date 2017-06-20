#pragma once
#include <boost/variant.hpp>

namespace variant_utils
{

// If `pv` is not null and internal value has type T, returns T*
// Otherwise returns nullptr.
template <class T, class variant_type>
inline std::add_pointer_t<T> get_if(variant_type *pv)
{
	std::add_pointer_t<T> ptr = nullptr;
	if (pv)
	{
		ptr = boost::get<T>(pv);
	}
	return ptr;
}

// If `pv` is not null and internal value has type T, returns const T*
// Otherwise returns nullptr.
template <class T, class variant_type>
inline std::add_pointer_t<const T> get_if(const variant_type *pv)
{
	std::add_pointer_t<const T> ptr = nullptr;
	if (pv)
	{
		ptr = boost::get<const T>(pv);
	}
	return ptr;
}

}
