//
//  cached_ptr.h
//
//  Copyright (c) 2016年 Matsusaki Satoru. All rights reserved.
//
//  Released under the MIT license
//  http://opensource.org/licenses/mit-license.php
//

#ifndef cached_ptr_h
#define cached_ptr_h

#include <cassert>
#include <thread>
#include <initializer_list>

namespace lm2 {

template <size_t C>
class memory_chain;

template <size_t C>
struct memory_node {
	char memory [C];
	union {
		memory_node <C> * next;
		memory_chain <C> * chain;
	};
	template <class T>
	T & at (size_t pos)
		{ return * (T *) (memory + sizeof (T) * pos); }
};

template <size_t C>
class memory_chain {
	memory_node <C> * chain;
	memory_node <C> * reserved;
	std::thread::id thread_id;
	std::mutex mutex;
public:
	memory_chain ()
	: chain (nullptr), reserved (nullptr),
	thread_id (std::this_thread::get_id())
		{}
	~memory_chain ()
	{
		unsigned cnt = 0;
		while (chain) {
			auto node = chain;
			chain = chain->next;
			delete node;
			cnt++;
		}
		while (reserved) {
			auto node = reserved;
			reserved = reserved->next;
			delete node;
			cnt++;
		}
#ifdef DEBUG
		std::cout << "lm2::make_memory_cache<" << C << ">(" << cnt << ");" << std::endl;
#endif
	}
	void reserve (memory_node<C> * node) {
		std::lock_guard<std::mutex> lock (mutex);
		node->next = reserved;
		reserved = node;
	}
	void push (memory_node<C> * node)
	{
		if (thread_id != std::this_thread::get_id()) {
			reserve (node);
			return;
		}
		node->next = chain;
		chain = node;
	}
	void unreserve ()
	{
		std::lock_guard<std::mutex> lock (mutex);
		while (reserved) {
			auto node = reserved;
			reserved = node->next;
			node->next = chain;
			chain = node;
		}
	}
	memory_node<C> * pop ()
	{
		if (reserved)
			unreserve();
		auto ret = chain;
		if (ret)
			chain = ret->next;
		else
			ret = new memory_node<C>;
		ret->chain = this;
		return ret;
	}
	void make_cache (unsigned len)
	{
		for (unsigned i=0; i<len; i++) {
			auto node = new memory_node<C>;
			node->next = chain;
			chain = node;
		}
	}
};

template <size_t C>
memory_chain<C> & get_memory_chain ()
{
	thread_local memory_chain<C> chain;
	return chain;
}

template <size_t S>
void make_memory_cache (unsigned len)
{
	memory_chain<S> & chain = get_memory_chain<S>();
	chain.make_cache (len);
}

template <class T, size_t C=1>
class cached_ptr {
	memory_node<sizeof (T) * C> * node;
	size_t len;
	
public:
	//間接参照演算子と前置インクリメント演算子と不等価演算子を定義する
	class iterator
	{
	public:
		iterator (const cached_ptr<T,C> & self, unsigned pos)
		: self (self), pos (pos)
		{
		}

		T & operator * ()
		{
			return self [pos];
		}

		void operator ++ ()
		{
			pos++;
		}

		bool operator != (iterator & iter)
		{
			return pos != iter.pos;
		}

	private:
		const cached_ptr & self;
		unsigned pos;
	};
    iterator begin() const
    {
        return iterator (*this, 0);
    }
    iterator end() const
    {
        return iterator (*this, len);
    }
public:
	template <class F>
	cached_ptr (size_t size, T && ini, F f)
	: node (get_memory_chain<sizeof(T)*C>().pop()), len (size)
	{
		assert (size <= C);
		node->template at <T> (0) = ini;
		for (int i=1; i<size; i++)
			new (node->memory + sizeof (T) * (i)) T (f (node [i - 1]));
	}
	template <class F>
	cached_ptr (size_t size, F f)
	: node (get_memory_chain<sizeof(T)*C>().pop()), len (size)
	{
		assert (size <= C);
		for (int i=0; i<size; i++)
			new (node->memory + sizeof (T) * i) T (f (i));
	}
	cached_ptr ()
	: node (get_memory_chain<sizeof(T)*C>().pop()), len (0)
	{
	}
	template <class I>
	cached_ptr (I && ini)
	: node (get_memory_chain<sizeof(T)*C>().pop()), len (1)
	{
		new (node->memory) T (ini);
	}
	template <class I>
	cached_ptr (std::initializer_list<I> inits)
	: node (get_memory_chain<sizeof(T)*C>().pop()), len (inits.size())
	{
		assert (C >= len);
		size_t pos = 0;
		for (const I & ini : inits)
			new (node->memory + sizeof (T) * (pos++)) T (ini);
	}
	~cached_ptr ()
	{
		if (node) {
			for (size_t i=0; i<len; i++)
				node->template at <T> (i).~T();
			node->chain->push (node);
		}
	}
    cached_ptr (cached_ptr && self) noexcept
	: node (std::move (self.node)), len (std::move (self.len))
	{
		self.node = nullptr;
	};
    cached_ptr & operator = (cached_ptr && self)
	{
		if (node) {
			for (size_t i=0; i<len; i++)
				node->template at <T> (i).~T();
			node->chain->push (node);
		}
		node = std::move(self.node);
		len = std::move(self.len);
		self.node = nullptr;
		return *this;
	}
	size_t size () const
		{ return len; }
	void resize (size_t size)
	{
		assert (size <= C);
		if (size == len)
			return;
		else if (size > len)
			for (size_t i=len; i<size; i++)
				new (node->memory + sizeof (T) * i) T();
		else
			for (size_t i=len; i>size; i--)
				node->template at <T> (i - 1).~T();
		len = size;
	}
	void push_back (T && t)
	{
		assert (len + 1 <= C);
		new (node->memory + sizeof (T) * len) T (std::move (t));
		len++;
	}
	T & operator [] (unsigned pos) const
		{ assert (pos < len); return node->template at <T> (pos); }
	T & operator * () const
		{ return node->template at <T> (0); }
	T * operator -> () const
		{ return & node->template at <T> (0); }
	T * operator & () const
		{ return & node->template at <T> (0); }
};

} // namespace

#endif
