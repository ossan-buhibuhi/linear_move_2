//
//  linear_move_2.h
//
//  Copyright (c) 2016 Matsusaki Satoru. All rights reserved.
//
//  Released under the MIT license
//  http://opensource.org/licenses/mit-license.php
//

#ifndef linear_move_2_h
#define linear_move_2_h

#include <initializer_list>

#include <iostream>
#include <list>
#include <vector>
#include <memory>
#include <cassert>
#include <random>       // std::default_random_engine

#include "cached_ptr.h"

namespace lm2 {

template <typename T, size_t C>
using cach = cached_ptr<T,C>;

template <class T>
class ref_ptr {
	const T * p;
public:
	ref_ptr ()
	: p (NULL)
		{}
	ref_ptr (const T & t)
	: p (&t)
		{}
	const T & operator * () const
		{ return *p; }
	const T * operator -> () const
		{ return p; }
};

template <typename T, size_t C>
std::ostream& operator<<(std::ostream & os, ref_ptr<cach<T,C>> & r)
{
	os << *r;
	
	return os;
}

template <typename T, size_t C>
std::ostream& operator<<(std::ostream & os, const cached_ptr <ref_ptr<T>,C> & vec)
{
	os << "(";
	size_t len = vec.size();
	if (len) {
		os << *vec[0];
		for(int i=1; i<len; i++) {
			os << ' ';
			os << *vec[i];
		}
	}
	os << ")";
	
	return os;
}

template <typename T, size_t C>
std::ostream& operator<<(std::ostream & os, const cached_ptr <T,C> & vec)
{
	os << "(";
	size_t len = vec.size();
	if (len) {
		os << vec[0];
		for(int i=1; i<len; i++) {
			os << ' ';
			os << vec[i];
		}
	}
	os << ")";
	
	return os;
}

template <class T, size_t C>
void display_of (const cached_ptr<T,C> & vec, const std::string & tag)
{
	std::cout << tag << ": " << vec << std::endl;
}

template <class T, size_t C>
cached_ptr<T,C> display (const std::string & tag, cached_ptr<T,C> && vec)
{
	std::cout << tag << ": " << vec << std::endl;
	return std::move (vec);
}

template <size_t C, class T, class F>
cached_ptr<T,C> progress (int len, T && ini, F && f) {
	cached_ptr <T,C> vec (len);
	if (!len)
		return std::move (vec);
	vec[0] = std::move(ini);
	for (int i=1; i<len; i++)
		vec[i] = f(vec[i - 1]);
	
	return std::move (vec);
}

template <class T, size_t C, class F>
T reduce (F && f, cached_ptr<T,C> && vec) {
	assert (vec.size());
	size_t len = vec.size();
	T acc = std::move (vec[0]);
	for (int i=1; i<len; i++)
		acc = f (std::move (acc), std::move (vec[i]));
	return std::move (acc);
}

template <class A, class T, size_t C, class F>
A fold (A && acc, F && f, cached_ptr<T,C> && vec) {
	size_t len = vec.size();
	for (int i=0; i<len; i++)
		acc = f (std::move (acc), std::move (vec[i]));
	return std::move (acc);
}

template <class A, class F>
A loop (A && arg, F && f) {
	while (std::get<0> (arg))
		arg = f (std::move (arg));
	return std::move (arg);
}

template <class T, size_t C>
cached_ptr<T,C> reverse (cached_ptr<T,C> && vec) {
	unsigned len = vec.size();
	unsigned cnt = len / 2;
	for (unsigned i=0; i<cnt; i++)
		std::swap(vec [i], vec [len - 1 - i]);
	return std::move (vec);
}

template <class T, size_t C, class F>
cached_ptr<T,C> sort (F && f, cached_ptr<T,C> && vec) {
	struct {
		F & f;
		cached_ptr<T,C> & vec;
		void recur (unsigned pos, size_t len) {
			thread_local char buf [(C - 1) * sizeof (T)];
			thread_local T * div = (T *) buf;
			
			if (len < 2)
				return;
			
			T t = std::move (vec [pos]);
			
			unsigned l_cnt = 0;
			unsigned r_cnt = 0;

			for (unsigned i = 1; i < len; i++)
				if (f (t, vec [pos + i]))
					div [l_cnt++] = std::move (vec [pos + i]);
				else
					div [len - ++r_cnt] = std::move (vec [pos + i]);
			
			for (unsigned i = 0; i < l_cnt; i++)
				vec [pos + i] = std::move (div [i]);
			
			vec [pos + l_cnt] = std::move (t);
			
			unsigned r_pos = pos + l_cnt + 1;
			for (unsigned i = 0; i < r_cnt; i++)
				vec [r_pos + i] = std::move (div [len - (i + 1)]);
			
			recur (pos, l_cnt);
			recur (r_pos, r_cnt);
		}
	} self = {f, vec};
	self.recur (0, vec.size());
	return std::move (vec);
}

template <class T, size_t C, class F>
size_t find_of (cached_ptr<T,C> & vec, F && f) {
	size_t len = vec.size ();
	size_t ret = len;
	for (unsigned i=0; i<len; i++)
		if (f (vec [i])) {
			ret = i;
			break;
		}
	return ret;
}

template <class T, size_t C>
cached_ptr<T,C> shuffle (cached_ptr<T,C> && vec) {
	std::random_device rd;
	std::mt19937 mt (rd());
	
	size_t size = vec.size();
	size_t len = size - 2;
	for (unsigned i = 1; i <= len; i++)
		std::swap (vec [i - 1], vec [i + mt() % (size - i)]);
	std::swap (vec [size - 1], vec [size - 2]);
	return std::move (vec);
}

template <class T, size_t C>
cached_ptr<T,C> append (T && t, cached_ptr<T,C> && vec) {
	size_t size = vec.size();
	vec.resize (size + 1);
	vec [size] = std::move (t);
	return std::move (vec);
}

template <class T, size_t C1, size_t C2>
bool compare (const cached_ptr<T,C1> & vec1, const cached_ptr<T,C2> & vec2) {
	size_t len = vec1.size();
	if (len != vec2.size())
		return false;
	for (size_t i=0; i<len; i++) {
		T & t1 = vec1 [i];
		T & t2 = vec2 [i];
		if (!(t1 == t2))
//		if (!(vec1[i] == vec2[i]))
			return false;
	}
	return true;
}

template <class T, size_t C, size_t CC1, size_t CC2>
bool compare (const cached_ptr<cached_ptr<T,C>, CC1> & vec1, const cached_ptr<cached_ptr<T,C>, CC2> & vec2) {
	size_t len = vec1.size();
	if (len != vec2.size())
		return false;
	for (size_t i=0; i<len; i++)
		if (!compare (vec1[i], vec2[i]))
			return false;
	return true;
}

template <class T, size_t C>
cached_ptr<T,C> combine (cached_ptr<T,C> && vec, cached_ptr<T,C> && vec2) {
	size_t pre_len = vec.size();
	size_t new_len = pre_len + vec2.size();
	vec.resize (new_len);
	for (size_t i=pre_len; i<new_len; i++)
		vec[i] = std::move (vec2 [i - pre_len]);
	return std::move (vec);
}

template <class T, size_t C>
cached_ptr<T,C> take (size_t len, cached_ptr<T,C> && vec) {
	assert (len < vec.size());
	vec.resize (len);
	return std::move (vec);
}

template <class T, size_t C>
cached_ptr<T,C> drop (size_t len, cached_ptr<T,C> && vec) {
	assert (len < vec.size());
	size_t cnt = vec.size() - len;
	for (int i=0; i<cnt; i++)
		vec [i] = std::move (vec[i + len]);
	vec.resize (vec.size() - len);

	return std::move (vec);
}

template <class T, size_t C, class F>
cached_ptr<T,C> drop_while (F && f, cached_ptr<T,C> && vec) {
	size_t len = vec.size();
	size_t i = 0;
	for (; i<len; i++)
		if (!f (vec [i]))
			break;
	if (!i)
		return std::move (vec);
	if (i == len)
		return cached_ptr<T,C>();
	return drop (i, std::move (vec));
}

template <class T, size_t C, class F>
cached_ptr<T,C> take_while (F && f, cached_ptr<T,C> && vec) {
	size_t len = vec.size();
	size_t i = 0;
	for (; i<len; i++)
		if (!f (vec [i]))
			break;
	if (!i)
		return cached_ptr<T,C> ();
	if (i == len)
		return std::move (vec);
	return take (i, std::move (vec));
}

template <class T, size_t C, size_t C2>
cached_ptr<T,C> join (cached_ptr<cached_ptr<T,C>,C2> && vec_vec) {
	size_t vec_vec_len = vec_vec.size();
	size_t len = 0;
	for (int i=0; i<vec_vec_len; i++) {
		size_t vec_len = vec_vec[i].size ();
		len += vec_len;
	}
	cached_ptr<T,C> ret (len);
	size_t pos = 0;
	for (size_t i=0; i<vec_vec_len; i++) {
		cached_ptr <T,C> & vec = vec_vec [i];
		size_t vec_len = vec.size ();
		for (size_t j=0; j<vec_len; j++) {
			ret [pos + j] = std::move (vec [j]);
		}
		pos += vec_len;
	}
	return std::move (ret);
}

template <class T, size_t C, class F>
auto map (F && f, cached_ptr<T,C> && vec)
-> cached_ptr<typename std::result_of<F(T)>::type, C>
{
	size_t len = vec.size();
	cached_ptr<typename std::result_of<F(T)>::type, C> ret (len);
	for (size_t i=0; i < len; i++)
		ret[i] = f (std::move (vec[i]));

	return std::move (ret);
}

template <class F>
auto let (F && f)
-> typename std::result_of<F()>::type
{
	return f();
}
/*
template <class T, size_t C, class F>
auto product_of (cached_ptr<T, C> & vec, F && f)
-> typename std::result_of<F(T&)>::type
{
	size_t len = vec.size();
	typename std::result_of<F(T&)>::type ret;
	for (size_t i=0; i < len; i++) {
		auto r = f (vec[i]);
//		std::copy (r.begin(), r.end(), std::back_inserter (ret));
		size_t r_len = r.size();
		for (size_t j=0; j < r_len; j++)
			ret.push_back (std::move (r[j]));
	}
	return std::move (ret);
}
*/
template <class T, size_t C, class F>
auto map_of (cached_ptr<T,C> & vec, F && f)
-> cached_ptr<typename std::result_of<F(T)>::type, C>
{
	size_t len = vec.size();
	cached_ptr<typename std::result_of<F(T)>::type, C> ret (len);
	for (size_t i=0; i < len; i++)
		ret[i] = f (vec[i]);

	return std::move (ret);
}

template <class T, size_t C, class U, class F>
cached_ptr<T,C> foreach (U && acc, F && f, cached_ptr<T,C> && vec)
{
	size_t len = vec.size();
	for (size_t i=0; i < len; i++) {
		const T & t = vec [i];
		acc = f (std::move (acc), t);
	}

	return std::move (vec);
}

template <class T, size_t C, class U, class F>
U fold_of (cached_ptr<T,C> & vec, U && acc, F && f)
{
	size_t len = vec.size();
	for (size_t i=0; i < len; i++) {
		const T & t = vec [i];
		acc = f (std::move (acc), t);
	}

	return std::move (acc);
}

template <class T, size_t C, class F>
cached_ptr<T,C> filter (F && f, cached_ptr<T,C> && vec) {
	size_t len = vec.size();
	size_t cnt = 0;
	for (size_t i=0; i<len; i++) {
		T & e = vec [i];
		if (f (e))
			vec [cnt++] = std::move (e);
	}
	vec.resize (cnt);

	return std::move (vec);
}

template <size_t RC, class T, size_t C, class F>
cached_ptr<cached_ptr<T,C>, RC> assort (/*size_t cnt,*/ F && f, cached_ptr <T,C> && vec) {
	size_t len = vec.size();
	cached_ptr<cached_ptr<T,C>, RC> ret; //cnt);
	size_t vec_lens [RC]; //cnt];
	for (size_t j=0; j<RC/*cnt*/; j++) {
		cached_ptr <T,C> vec (len);
		ret [j] = std::move (vec);
		vec_lens [j] = 0;
	}
	for (size_t i=0; i<len; i++) {
		T & e = vec [i];
		size_t pos = f (e);
		assert (pos < RC/*cnt*/);
		ret[pos][vec_lens [pos]++] = std::move (e);
	}
	for (size_t j=0; j<RC/*cnt*/; j++)
		ret[j].resize (vec_lens [j]);

	return std::move (ret);
}

template <size_t L, class T, size_t C>
cached_ptr<cached_ptr<T,C>, C / L> group (cached_ptr<T,C> && vec) {
	size_t len = vec.size();
	size_t rest_cnt = len % L;
	size_t group_cnt = len / L;
	cached_ptr<cached_ptr<T,C>, C / L> ret (group_cnt + (rest_cnt ? 1 : 0));
	size_t src_pos = 0;
	for (size_t i=0; i < group_cnt; i++) {
		ret [i] = cached_ptr<T,C> (L);
		for (int j=0; j < L; j++)
			ret[i][j] = std::move (vec[src_pos++]);
	}
	if (rest_cnt) {
		ret[group_cnt] = cached_ptr<T,C> (rest_cnt);
		for (int j=0; j < rest_cnt; j++)
			ret[group_cnt][j] = std::move (vec [src_pos++]);
	}
	return std::move (ret);
}

template <size_t C2, class T, size_t C>
cached_ptr<cached_ptr<T,C>, C2> group (unsigned cnt, cached_ptr<T,C> && vec) {
	size_t len = vec.size();
	size_t rest_cnt = len % cnt;
	size_t group_cnt = len / cnt;
	cached_ptr<cached_ptr<T,C>, C2> ret (group_cnt + (rest_cnt ? 1 : 0));
	size_t src_pos = 0;
	for (size_t i=0; i < group_cnt; i++) {
		ret [i] = cached_ptr<T,C> (cnt);
		for (int j=0; j < cnt; j++)
			ret[i][j] = std::move (vec[src_pos++]);
	}
	if (rest_cnt) {
		ret[group_cnt] = cached_ptr<T,C> (rest_cnt);
		for (int j=0; j < rest_cnt; j++)
			ret[group_cnt][j] = std::move (vec [src_pos++]);
	}
	return std::move (ret);
}

template <class T, size_t C>
cached_ptr<T *, C> refdup (cached_ptr<T,C> & vec) {
	size_t len = vec.size();
	cached_ptr<T *, C> ret (len);
	for (size_t i = 0; i < len; i++)
		ret [i] = & vec [i];
	return std::move (ret);
}

} // namespace

#endif
