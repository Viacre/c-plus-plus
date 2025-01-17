#pragma once

#include <type_traits>
#include <memory>

namespace cxx
{

using std::is_same;
using std::is_convertible;
using std::enable_if_t;
using std::allocator_traits;

template<bool _vInt>
using bool_ = std::integral_constant<bool, _vInt>;

using true_ = bool_<true>;
using false_ = bool_<false>;

template<typename _type>
using indirect_t = decltype(*std::declval<_type>());

template<typename _type>
using increment_t = decltype(++std::declval<_type>());

namespace details
{

template<typename _tParam>
using iterator_reqirement_t = is_same<increment_t<_tParam&>, _tParam&>;

template<typename _tParam>
using input_iterator_reqirement_t
	= is_convertible<typename std::iterator_traits<_tParam>::iterator_category,
		std::input_iterator_tag>;

} // namespace details;

template<typename _tParam, typename _type = void,
	typename = indirect_t<_tParam&>, typename = increment_t<_tParam&>>
using enable_for_input_iterator_t
	= enable_if_t<details::input_iterator_reqirement_t<_tParam>::value, _type>;

template<typename _type>
using is_unqualified_object
	= is_same<typename std::remove_cv<_type>::type, _type>;

template<class _tAlloc, typename _type>
using rebind_alloc_t =
	typename allocator_traits<_tAlloc>::template rebind_alloc<_type>;

template<typename _type, typename _tValue>
using is_allocator_for = is_same<typename _type::value_type, _tValue>;

}