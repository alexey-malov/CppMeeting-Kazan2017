#pragma once
#include <boost/variant.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/copy.hpp>

namespace variant_utils
{

// Defines struct with constexpr static field `type`
//  which keeps index of type or special value `npos`.
template <class variant_type, class variant_case>
using variant_index_t = typename boost::mpl::find<
	typename boost::mpl::copy<
	typename variant_type::types,
	boost::mpl::back_inserter<boost::mpl::vector<>>
	>::type,
	variant_case
>::type::pos;

// Defines template variable
//  which keeps index of type or special value `npos`.
template <class variant_type, class variant_case>
static constexpr int variant_index_v = variant_index_t<variant_type, variant_case>::value;

// If `pv` is not null and internal value has type T, returns T*
// Otherwise returns nullptr.
template <class T, class variant_type>
inline std::add_pointer_t<T> get_if(variant_type *pv)
{
	std::add_pointer_t<T> ptr = nullptr;
	if (pv)
	{
		ptr = boost::get<T>(&pv);
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
		ptr = boost::get<const T>(&pv);
	}
	return ptr;
}

}
