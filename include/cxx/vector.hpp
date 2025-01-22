#pragma once

#include <algorithm>
#include <stdexcept>
#include <limits>
#include <cassert>
#include "meta.hpp"
#include <iostream>

namespace cxx
{

namespace details
{

/*!
\brief 向量表示类型模板
*/
template<typename _type, class _tAlloc>
class vector_rep
{
public:
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using allocator_type = _tAlloc;
	using iterator = _type*;
	using const_iterator = const _type*;

private:
	using value_type = _type;
	using elem_allocator = rebind_alloc_t<_tAlloc, _type>;
	using elem_ator_traits = allocator_traits<elem_allocator>;
	using allocator_reference
		= std::conditional_t<sizeof(elem_allocator) <= sizeof(void*),
			elem_allocator, elem_allocator&>;

public:
	using pointer = typename elem_ator_traits::pointer;
	using const_pointer = typename elem_ator_traits::const_pointer;

private:
	struct vector_header
	{
		pointer data;
		size_type size;
		size_type capacity;

		vector_header() noexcept : data(), size(0), capacity(0)
		{}
		vector_header(vector_header&& x) noexcept
			: data(x.data), size(x.size), capacity(x.capacity)
		{
			x.data = pointer();
			x.size = x.capacity = 0;
		}
		void
		copy_data(const vector_header& x) noexcept
		{
			data = x.data;
			size = x.size;
			capacity = x.capacity;
		}
		void
		swap_data(const vector_header& x) noexcept
		{
			vector_header tmp;
			tmp.copy_data(*this);
			copy_data(x);
			x.copy_data(tmp);
		}
	};

protected:
	struct components : elem_allocator
	{
		using base = elem_allocator;
		vector_header header{};

		components() noexcept(
			std::is_nothrow_default_constructible<elem_allocator>())
			: base()
		{}
		explicit components(elem_allocator a) noexcept : base(std::move(a))
		{}
		components(const components& x)
			: base(elem_ator_traits::select_on_container_copy_construction(
				  x.get()))
		{}
		components(components&&) = default;
		components(components&& x, elem_allocator a) noexcept
			: base(std::move(a)), header(std::move(x.header))
		{}
		elem_allocator&
		get() noexcept
		{
			return static_cast<elem_allocator&>(*this);
		}
		const elem_allocator&
		get() const noexcept
		{
			return static_cast<const elem_allocator&>(*this);
		}
	};
	components objects;

public:
	vector_rep() = default;
	vector_rep(const allocator_type& a) noexcept : objects(elem_allocator(a))
	{}
	vector_rep(size_type n)
	{
		create_storage(n);
	}
	vector_rep(size_type n, const allocator_type& a)
		: objects(elem_allocator(a))
	{
		create_storage(n);
	}
	vector_rep(vector_rep&&) = default;
	vector_rep(vector_rep&& x, const allocator_type& a) noexcept(
		elem_ator_traits::is_always_equal::value)
		: vector_rep(std::move(x), elem_allocator(a),
			  typename elem_ator_traits::is_always_equal())
	{}

private:
	vector_rep(vector_rep&& x, elem_allocator a, true_) noexcept
		: objects(std::move(x.objects), std::move(a))
	{}
	vector_rep(vector_rep&& x, elem_allocator a, false_) : objects(std::move(a))
	{
		if(get_elem_allocator() == x.get_elem_allocator())
			objects.header.swap_data(x.objects.header);
		else
			insert_range(begin(), std::make_move_iterator(x.begin()),
				std::make_move_iterator(x.end()));
	}

private:
	void
	create_storage(size_type n)
	{
		objects.header.data = allocate_storage(n);
		objects.header.size = 0;
		objects.header.capacity = n;
	}
	pointer
	allocate_storage(size_type n)
	{
		allocator_reference a(get_elem_allocator());
		return n != 0 ? elem_ator_traits::allocate(a, n) : pointer();
	}
	void
	deallocate_storage(pointer p, size_type n)
	{
		allocator_reference a(get_elem_allocator());
		elem_ator_traits::deallocate(a, p, n);
	}
	template<typename... _tParams>
	void
	construct_storage(pointer p, _tParams&&... args)
	{
		allocator_reference a(get_elem_allocator());
		elem_ator_traits::construct(a, p, std::forward<_tParams>(args)...);
	}
	void
	destroy_storage(pointer p) noexcept
	{
		allocator_reference a(get_elem_allocator());
		elem_ator_traits::destroy(a, p);
	}

public:
	~vector_rep()
	{
		deallocate_storage(objects.header.data, capacity());
	}
	vector_rep&
	operator=(const vector_rep& x)
	{
		if(std::addressof(x) != this)
		{
			assign_range(x.begin(), x.end());
		}
		return *this;
	}
	vector_rep&
	operator=(vector_rep&& x)
	{
		assign_range(std::make_move_iterator(x.begin()),
			std::make_move_iterator(x.end()));
		return *this;
	}
	template<typename _tIn>
	void
	init_range(_tIn first, _tIn last)
	{
		for(; first != last; ++first)
			insert_it(end(), *first);
	}
	template<typename _tIn>
	void
	assign_range(_tIn first, _tIn last)
	{
		reserve(std::distance(first, last));
		auto i(begin());
		const auto j(end());

		for(; i != j && first != last; ++i, static_cast<void>(++first))
			*i = *first;
		if(first == last)
			erase_range(i, j);
		else
			insert_range(j, first, last);
	}
	allocator_type
	get_allocator() const noexcept
	{
		return allocator_type(get_elem_allocator());
	}
	elem_allocator&
	get_elem_allocator() noexcept
	{
		return objects.get();
	}
	const elem_allocator&
	get_elem_allocator() const noexcept
	{
		return objects.get();
	}
	pointer
	data() noexcept
	{
		return objects.header.data;
	}
	const_pointer
	data() const noexcept
	{
		return objects.header.data;
	}
	iterator
	begin() noexcept
	{
		return objects.header.data;
	}
	const_iterator
	begin() const noexcept
	{
		return objects.header.data;
	}
	iterator
	end() noexcept
	{
		return objects.header.data + objects.header.size;
	}
	const_iterator
	end() const noexcept
	{
		return objects.header.data + objects.header.size;
	}
	bool
	empty() const noexcept
	{
		assert((begin() == end()) == (size() == 0));
		return size() == 0;
	}
	size_type
	size() const noexcept
	{
		assert(std::distance(begin(), end())
			== difference_type(objects.header.size));
		return objects.header.size;
	}
	size_type
	max_size() const noexcept
	{
		return elem_ator_traits::max_size(get_elem_allocator());
	}
	size_type
	capacity() const noexcept
	{
		return objects.header.capacity;
	}
	/*!
	\brief 修改 sz 为需要插入的新元素数量
	\return 需要擦除的元素的左区间迭代器
	*/
	const_iterator
	resize_pos(size_type& sz) const
	{
		const_iterator i;
		const size_type len(size());

		if(sz < len)
		{
			if(sz <= len / 2)
			{
				i = begin();
				std::advance(i, sz);
			}
			else
			{
				const ptrdiff_t n_erase(len - sz);

				i = end();
				std::advance(i, -n_erase);
			}
			sz = 0;
			return i;
		}
		else
			i = end();
		sz -= len;
		return i;
	}
	//! \brief 在容器末尾插入 n 个默认值
	void
	append_default(size_type n)
	{
		if(size() + n > capacity())
			reserve(1.5 * capacity() + n);

		for(auto current(end()); n != 0; ++current, --n)
			insert_it(current, value_type());
	}
	void
	reserve(size_type n)
	{
		if(n <= capacity())
			return;
		if(n > max_size())
			throw std::length_error("vector::reserve: n > max_size()");

		pointer tmp = allocate_storage(n);

		for(size_type i(0); i < size(); ++i)
			construct_storage(&tmp[i], objects.header.data[i]);

		for(size_type i(0); i < size(); ++i)
			destroy_storage(&objects.header.data[i]);

		deallocate_storage(objects.header.data, capacity());
		objects.header.data = tmp;
		objects.header.capacity = n;
	}

public:
	template<typename... _tParams>
	void
	insert_it(iterator position, _tParams&&... args)
	{
		construct_storage(position, std::forward<_tParams>(args)...);
		++objects.header.size;
	}
	template<typename _tIn>
	iterator
	insert_range(iterator position, _tIn first, _tIn last)
	{
		if(first != last)
		{
			const size_type n(std::distance(first, last));
			const size_type dist(std::distance(position, end()));
			if(size() + n > capacity())
				reserve(1.5 * capacity() + n);

			const auto backit(end() - 1);
			auto current(backit + n);

			for(; current != backit + n - dist; --current)
				insert_it(current, *(current - n));

			for(; current != backit; --current, static_cast<void>(--last))
				insert_it(current, *(last - 1));

			for(; first != last; --current, static_cast<void>(--last))
				*current = *(last - 1);
			return current;
		}
		return position;
	}
	void
	erase_it(iterator position) noexcept
	{
		destroy_storage(position);
		--objects.header.size;
	}
	void
	erase_range(iterator first, iterator last) noexcept
	{
		for(; last != end(); ++first, ++last)
			*first = std::move(*last);

		while(first != last)
		{
			erase_it(last);
			--last;
		}
	}
	template<typename... _tParams>
	iterator
	emplace(const_iterator position, _tParams&&... args)
	{
		const size_type dist(
			std::distance(begin(), const_cast<iterator>(position)));

		if(size() == capacity())
			reserve(1.5 * capacity() + 1);

		insert_it(end(), *(end() - 1));
		auto current(end() - 1);
		for(; current != begin() + dist; --current)
			*current = std::move(*(current - 1));
		*current = {std::forward<_tParams>(args)...};
		return current;
	}
	void
	clear() noexcept
	{
		iterator i(end() - 1);
		while(!empty())
		{
			erase_it(i);
			--i;
		}
	}
};

}

/*!
\brief 向量容器
*/
template<typename _type, class _tAlloc = std::allocator<_type>>
class vector
{
public:
	using value_type = _type;
	static_assert(is_unqualified_object<value_type>(),
		"The value type for allocator shall be an unqualified object type.");
	static_assert(is_allocator_for<_tAlloc, value_type>(),
		"Value type mismatched to the allocator found.");
	using allocator_type = _tAlloc;

private:
	using ator_traits = allocator_traits<allocator_type>;
	using rep_type = details::vector_rep<_type, _tAlloc>;

public:
	using pointer = typename ator_traits::pointer;
	using const_pointer = typename ator_traits::const_pointer;
	using reference = _type&;
	using const_reference = const _type&;
	using size_type = typename rep_type::size_type;
	using difference_type = typename rep_type::difference_type;
	using iterator = typename rep_type::iterator;
	using const_iterator = typename rep_type::const_iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
	rep_type rep; // Representation type

public:
	vector() = default;
	explicit vector(const allocator_type& a) noexcept : rep(a)
	{}
	explicit vector(size_type n, const allocator_type& a = allocator_type())
		: rep(n, a)
	{
		for(; n != 0; --n)
			emplace_back();
	}
	vector(size_type n, const value_type& val,
		const allocator_type& a = allocator_type())
		: rep(n, a)
	{
		init_fill(n, val);
	}
	template<typename _tIn, typename = enable_for_input_iterator_t<_tIn>>
	vector(_tIn first, _tIn last, const allocator_type& a = allocator_type())
		: rep(std::distance(first, last), a)
	{
		init_range(first, last);
	}
	vector(std::initializer_list<value_type> il,
		const allocator_type& a = allocator_type())
		: rep(il.size(), a)
	{
		init_range(il.begin(), il.end());
	}
	vector(const vector& x) : rep(x.size())
	{
		init_range(x.begin(), x.end());
	}
	vector(const vector& x, const allocator_type& a) : rep(x.size(), a)
	{
		init_range(x.begin(), x.end());
	}
	vector(vector&&) = default;
	vector(vector&& x, const allocator_type& a);

public:
	~vector() = default;

	vector&
	operator=(const vector&)
		= default;

	vector&
	operator=(vector&&)
		= default;

	vector&
	operator=(std::initializer_list<value_type> il)
	{
		assign(il);
		return *this;
	}
	friend bool
	operator==(const vector& x, const vector& y)
	{
		return x.size() == y.size()
			&& std::equal(x.cbegin(), x.cend(), y.cbegin());
	}
	friend bool
	operator<(const vector& x, const vector& y)
	{
		return std::lexicographical_compare(
			x.cbegin(), x.cend(), y.cbegin(), y.cend());
	}

private:
	void
	init_fill(size_type n, const value_type& val)
	{
		for(; n != 0; --n)
			push_back(val);
	}
	template<typename _tIn>
	void
	init_range(_tIn first, _tIn last)
	{
		rep.init_range(first, last);
	}
	void
	assign_fill(size_type n, const value_type& val)
	{
		reserve(n);
		iterator i = begin();
		for(; i != end() && n > 0; ++i, --n)
			*i = val;
		if(n > 0)
			insert(end(), n, val);
		else
			erase(i, end());
	}

public:
	void
	assign(size_type n, const value_type& val)
	{
		assign_fill(n, val);
	}
	template<typename _tIn, typename = enable_for_input_iterator_t<_tIn>>
	void
	assign(_tIn first, _tIn last)
	{
		rep.assign_range(first, last);
	}
	void
	assign(std::initializer_list<value_type> il)
	{
		rep.assign_range(il.begin(), il.end());
	}
	allocator_type
	get_allocator() const noexcept
	{
		return rep.get_allocator();
	}
	iterator
	begin() noexcept
	{
		return rep.begin();
	}
	const_iterator
	begin() const noexcept
	{
		return rep.begin();
	}
	iterator
	end() noexcept
	{
		return rep.end();
	}
	const_iterator
	end() const noexcept
	{
		return rep.end();
	}
	const_iterator
	cbegin() const noexcept
	{
		return begin();
	}
	const_iterator
	cend() const noexcept
	{
		return end();
	}
	const_reverse_iterator
	rbegin() const noexcept
	{
		return const_reverse_iterator(end());
	}
	const_reverse_iterator
	rend() const noexcept
	{
		return const_reverse_iterator(begin());
	}
	const_reverse_iterator
	crbegin() const noexcept
	{
		return const_reverse_iterator(cend());
	}
	const_reverse_iterator
	crend() const noexcept
	{
		return const_reverse_iterator(cbegin());
	}
	bool
	empty() const noexcept
	{
		return rep.empty();
	}
	size_type
	size() const noexcept
	{
		return rep.size();
	}
	size_type
	capacity() const noexcept
	{
		return rep.capacity();
	}
	size_type
	max_size() const noexcept
	{
		return rep.max_size();
	}
	void
	reserve(size_type n)
	{
		rep.reserve(n);
	}
	reference
	operator[](size_type pos)
	{
		return assert(pos < size()), *(begin() + pos);
	}
	const_reference
	operator[](size_type pos) const
	{
		return assert(pos < size()), *(begin() + pos);
	}
	reference
	at(size_type pos)
	{
		return pos < size()
			? *(begin() + pos)
			: (throw std::out_of_range("vector::at: pos >= size()"), *begin());
	}
	const_reference
	at(size_type pos) const
	{
		return pos < size()
			? *(begin() + pos)
			: (throw std::out_of_range("vector::at: pos >= size()"), *begin());
	}
	reference
	front()
	{
		return assert(!empty()), *begin();
	}
	const_reference
	front() const
	{
		return assert(!empty()), *begin();
	}
	reference
	back()
	{
		assert(!empty());
		auto i(end());
		--i;
		return *i;
	}
	const_reference
	back() const
	{
		assert(!empty());
		auto i(cend());
		--i;
		return *i;
	}
	pointer
	data() noexcept
	{
		return rep.data();
	}
	const_pointer
	data() const noexcept
	{
		return rep.data();
	}
	void
	clear() noexcept
	{
		rep.clear();
	}
	iterator
	insert(const_iterator position, const value_type& val)
	{
		return rep.emplace(position, val);
	}
	iterator
	insert(const_iterator position, value_type&& val)
	{
		return rep.emplace(position, std::move(val));
	}
	iterator
	insert(const_iterator position, std::initializer_list<value_type> il)
	{
		return insert(position, il.begin(), il.end());
	}
	iterator
	insert(const_iterator position, size_type n, const value_type& val)
	{
		const size_type dist(std::distance(position, cend()));
		if(n != 0)
		{
			if(size() + n > capacity())
				reserve(1.5 * capacity() + n);

			const auto backit(end() - 1);
			auto current(backit + n);

			for(; current != backit + n - dist; --current)
				rep.insert_it(current, *(current - n));

			for(; current != backit; --current, --n)
				rep.insert_it(current, val);

			for(; n != 0; --current, --n)
				*current = val;
			return begin() + dist;
		}
		return begin() + dist;
	}
	template<typename _tIn, typename = enable_for_input_iterator_t<_tIn>>
	iterator
	insert(const_iterator position, _tIn first, _tIn last)
	{
		return rep.insert_range(const_cast<iterator>(position), first, last);
	}
	template<typename... _tParams>
	iterator
	emplace(const_iterator position, _tParams&&... args)
	{
		return rep.emplace(position, std::forward<_tParams>(args)...);
	}
	iterator
	erase(const_iterator position)
	{
		auto current(const_cast<iterator>(position));
		const auto result(current);
		for(; current != end() - 1; ++current)
			*current = std::move(*(current + 1));
		rep.erase_it(end() - 1);
		return result;
	}
	iterator
	erase(const_iterator first, const_iterator last)
	{
		auto result(const_cast<iterator>(last));
		rep.erase_range(const_cast<iterator>(first), result);
		return result;
	}
	void
	push_back(const value_type& val)
	{
		emplace_back(val);
	}
	void
	push_back(value_type&& val)
	{
		emplace_back(std::move(val));
	}
	template<typename... _tParams>
	reference
	emplace_back(_tParams&&... args)
	{
		if(size() == capacity())
			reserve(1.5 * capacity() + 1);
		rep.insert_it(end(), std::forward<_tParams>(args)...);
		return back();
	}
	void
	pop_back()
	{
		assert(!empty());
		rep.erase_it(end() - 1);
	}
	void
	resize(size_type sz)
	{
		const auto i(rep.resize_pos(sz));

		if(sz != 0)
			rep.append_default(sz);
		else
			erase(i, end());
	}
	void
	resize(size_type sz, const value_type& val)
	{
		const auto i(rep.resize_pos(sz));

		if(sz != 0)
			insert(end(), sz, val);
		else
			erase(i, end());
	}
};

}