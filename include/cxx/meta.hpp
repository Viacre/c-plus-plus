#pragma once

#include <type_traits>
#include <memory>

namespace cxx
{

using std::is_same;
using std::is_convertible;
using std::is_object;
using std::enable_if_t;
using std::allocator_traits;
using std::remove_cv_t;

template<bool _vInt>
using bool_ = std::integral_constant<bool, _vInt>;

using true_ = bool_<true>;
using false_ = bool_<false>;

template<class...>
struct and_;

template<>
struct and_<> : true_
{};

template<class _b1>
struct and_<_b1> : _b1
{};

template<class _b1, class _b2, class... _bn>
struct and_<_b1, _b2, _bn...>
	: std::conditional<_b1::value, and_<_b2, _bn...>, _b1>::type
{};

template<class...>
struct or_;

template<>
struct or_<> : false_
{};

template<class _b1>
struct or_<_b1> : _b1
{};

template<class _b1, class _b2, class... _bn>
struct or_<_b1, _b2, _bn...>
	: std::conditional<_b1::value, _b1, or_<_b2, _bn...>>::type
{};

template<typename _type>
using indirect_t = decltype(*std::declval<_type>());

template<typename _type>
using increment_t = decltype(++std::declval<_type>());

namespace details
{

template<typename _tParam>
using iterator_reqirement_t = is_same<increment_t<_tParam&>, _tParam&>;

template<typename _tParam>
using input_iterator_reqirement_t = and_<iterator_reqirement_t<_tParam>,
	is_convertible<typename std::iterator_traits<_tParam>::iterator_category,
		std::input_iterator_tag>>;

} // namespace details;

template<typename _tParam, typename _type = void,
	typename = indirect_t<_tParam&>, typename = increment_t<_tParam&>>
using enable_for_input_iterator_t
	= enable_if_t<details::input_iterator_reqirement_t<_tParam>::value, _type>;

template<typename _type, typename _tTo = _type>
struct is_unqualified : is_same<_type, remove_cv_t<_tTo>>
{};

template<typename _type>
struct is_unqualified_object
	: and_<is_object<_type>, is_unqualified<_type>>
{};

template<class _tAlloc, typename _type>
using rebind_alloc_t =
	typename allocator_traits<_tAlloc>::template rebind_alloc<_type>;

template<typename _type, typename _tValue>
using is_allocator_for = is_same<typename _type::value_type, _tValue>;

}