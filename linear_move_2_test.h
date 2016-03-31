//
//  linear_move_2_test.h
//  fix_test
//
//  Created by 松崎暁 on 2015/12/29.
//  Copyright (c) 2015年 松崎暁. All rights reserved.
//

#ifndef __fix_test__linear_move_2_test__
#define __fix_test__linear_move_2_test__

#include "linear_move_2.h"

using namespace lm2;

class Fuga {
	int num;
public:
	static int copy_cnt;
	static int life_cnt;
	Fuga ()
	: num (0)
		{ life_cnt++; }
	Fuga (int n)
	: num (n)
		{ life_cnt++; }
	Fuga (const Fuga & fuga)
	: num (fuga.num)
		{ life_cnt++; copy_cnt++; }
	~Fuga ()
		{ life_cnt--; }
    Fuga(Fuga && self) noexcept
	: num (std::move (self.num))
		{ life_cnt++; };
    Fuga & operator = (Fuga && self)
		{ num = std::move(self.num); return *this; }
	bool operator == (const Fuga & fuga) const
		{ return num == fuga.num; }
	bool operator != (const Fuga & fuga) const
		{ return num != fuga.num; }
	int get_num () const
		{ return num; }
	void set_num (int n)
		{ num = n; }
};

class Hoge {
	std::unique_ptr <Fuga> fuga;
public:
	Hoge ()
		{ life_cnt++; }
	Hoge (const Hoge & hoge)
	: fuga (new Fuga (*hoge.fuga))
		{ life_cnt++; copy_cnt++; }
	Hoge (int n)
	: fuga (new Fuga (n))
		{ life_cnt++; }
	~Hoge ()
		{ life_cnt--; }
    Hoge(Hoge && self) noexcept
	: fuga (std::move (self.fuga))
		{ life_cnt++;};
    Hoge & operator = (Hoge && self)
		{ fuga = std::move(self.fuga); return *this; }
	bool operator == (const Hoge & hoge) const
		{ return *fuga == *hoge.fuga; }
	bool operator != (const Hoge & hoge) const
		{ return *fuga != *hoge.fuga; }
	const std::unique_ptr <Fuga> & get_fuga () const
		{ return fuga; }
	int get_num () const
		{ return fuga->get_num(); }
	void set_num (int n)
		{ return fuga->set_num(n); }
	static int life_cnt;
	static int copy_cnt;
};
/*
inline std::ostream& operator<<(std::ostream& os, const uniq <Hoge> & hoge)
{
	os << hoge->get_fuga()->get_num();
	
	return os;
}
*/

inline std::ostream& operator<<(std::ostream& os, const Hoge & hoge)
{
	os << hoge.get_num();
	
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Fuga & fuga)
{
	os << fuga.get_num();
	
	return os;
}

template <size_t C>
inline cached_ptr <Hoge, C> make_hoges (int len, int init=1, int step=1)
{
	return progress <C> (len, Hoge (init),
		[step] (const Hoge & hoge) {
			return Hoge (hoge.get_num() + step);
		});
}

bool test_all ();

#endif /* defined(__fix_test__linear_move_2_test__) */
