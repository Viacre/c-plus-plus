#pragma once

#include <iterator>
#include <algorithm>
#include "cassert"

namespace cxx
{

using std::size_t;
using std::ptrdiff_t;

template<typename _type, size_t _vN>
struct array
{
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using value_type = _type;
	using pointer = _type*;
	using const_pointer = const _type*;
	using reference = _type&;
	using const_reference = const _type&;
	using iterator = _type*;
	using const_iterator = const _type*;
	using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	value_type data_[_vN];

	constexpr reference
	operator[](size_type pos)
	{
		return assert(pos < _vN), data_[pos];
	}
	constexpr const_reference
	operator[](size_type pos) const
	{
		return assert(pos < _vN), data_[pos];
	}
	constexpr reference
	at(size_type pos)
	{
		return pos < _vN ? data_[pos]
						 : (throw std::out_of_range("array::at"), data_[0]);
	}
	constexpr const_reference
	at(size_type pos) const
	{
		return pos < _vN ? data_[pos]
						 : (throw std::out_of_range("array::at"), data_[0]);
	}
	constexpr reference
	front()
	{
		return assert(0 < _vN), data_[0];
	}
	constexpr const_reference
	front() const
	{
		return assert(0 < _vN), data_[0];
	}
	constexpr reference
	back()
	{
		return assert(0 < _vN), data_[_vN - 1];
	}
	constexpr const_reference
	back() const
	{
		return assert(0 < _vN), data_[_vN - 1];
	}
	constexpr pointer
	data() noexcept
	{
		return data_;
	}
	constexpr const_pointer
	data() const noexcept
	{
		return data_;
	}
	constexpr size_type
	size() const noexcept
	{
		return _vN;
	}
	constexpr size_type
	max_size() const noexcept
	{
		return _vN;
	}
	void
	fill(const value_type& val)
	{
		std::fill_n(data_, _vN, val);
	}
};

template<typename _type, size_t _vN, typename _tSrc>
constexpr cxx::array<_type, _vN>
to_array(const _tSrc& src)
{
	return cxx::array<_type, _vN>(src);
}
template<typename _type, size_t _vN>
constexpr cxx::array<_type, _vN>
to_array(const cxx::array<_type, _vN>& src)
{
	return src;
}
template<typename _type, size_t _vN, typename _tSrcElement>
inline cxx::array<_type, _vN>
to_array(const _tSrcElement (&src)[_vN])
{
	cxx::array<_type, _vN> arr;

	std::copy_n(std::addressof(src[0]), _vN, std::addressof(arr[0]));
	return arr;
}
template<typename _type, size_t _vN, typename _tSrcElement>
inline cxx::array<_type, _vN>
to_array(_tSrcElement (&&src)[_vN])
{
	cxx::array<_type, _vN> arr;

	std::copy_n(std::make_move_iterator(std::addressof(src[0])), _vN,
		std::addressof(arr[0]));
	return arr;
}
template<typename _type, size_t _vN, typename _tSrcElement>
inline cxx::array<_type, _vN>
to_array(std::initializer_list<_tSrcElement> il)
{
	cxx::array<_type, il.size()> arr;

	std::copy_n(std::make_move_iterator(il.begin()), std::min(_vN, il.size()),
		std::addressof(arr[0]));
	return arr;
}

}