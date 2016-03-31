//
//  linear_move_2_test.cpp
//

#include "linear_move_2_test.h"

#include <iostream>
#include "cached_ptr.h"

int Fuga::copy_cnt = 0;
int Fuga::life_cnt = 0;
int Hoge::life_cnt = 0;
int Hoge::copy_cnt = 0;

bool progress__test () {
	puts ("progress__test");

	auto hoges =
		progress <100> (3, Hoge (2),
			[] (const Hoge & hoge) {
				int num = hoge.get_num();
				return Hoge (num + 2);
			}
		);
	
	for (int i = 1; i <= 3; i++)
		if (hoges [i - 1].get_num () != i * 2)
			return false;
	
    return true;
}

bool make_hoges__test ()
{
	puts ("make_hoges__test");
	auto hoges (make_hoges <100> (10, 1, 2));
	for (int i=0; i<10; i++)
		if (hoges [i].get_num() != 1 + i * 2)
			return false;
	return true;
}

bool make_hoges__test2 ()
{
	puts ("make_hoges__test2");
	auto hoges (make_hoges<100> (10, 2));
	for (int i=0; i<10; i++) {
		if (hoges [i].get_num() != i + 2)
			return false;
	}
	return true;
}

bool make_hoges__test3 ()
{
	puts ("make_hoges__test3");
	auto hoges (make_hoges<100> (10, 2));
	for (int i=0; i<10; i++) {
		if (hoges [i].get_num() != i + 2)
			return false;
	}
	return true;
}

bool compare__ttest ()
{
	puts ("compare__ttest");
	auto hoges1 = make_hoges<100> (10);
	auto hoges2 = make_hoges<100> (10);
	return compare (hoges1, hoges2);
}

bool compare__ftest0 ()
{
	puts ("compare__ftest0");
	auto hoges1 = make_hoges<100> (10);
	auto hoges2 = make_hoges<100> (9);
	return !compare (hoges1, hoges2);
}

bool compare__ftest1 ()
{
	puts ("compare__ftest1");
	auto hoges1 = make_hoges<100> (100, 1, 1);
	auto hoges2 = make_hoges<100> (100, 1, 2);
	return !compare (hoges1, hoges2);
}

bool append__test ()
{
	puts ("append__test");
	auto good = make_hoges<100> (11);
	auto hoges =
		append (Hoge (11),
		make_hoges<100> (10)
		);
	return compare (hoges, good);
}

bool compare__ttest1()
{
	puts ("compare__ttest1 # compare vector-vector");
	cached_ptr<cached_ptr<Hoge, 100>, 100> hoges1 (3);
	hoges1 [0] = make_hoges<100> (3, 1);
	hoges1 [1] = make_hoges<100> (3, 4);
	hoges1 [2] = make_hoges<100> (3, 7);

	cached_ptr<cached_ptr<Hoge, 100>, 100> hoges2 (3);
	hoges2 [0] = make_hoges<100> (3, 1);
	hoges2 [1] = make_hoges<100> (3, 4);
	hoges2 [2] = make_hoges<100> (3, 7);

	return compare (hoges1, hoges2);
}

bool compare__ftest2()
{
	puts ("compare__ftest2 # compare vector-vector");
	cached_ptr<cached_ptr<Hoge, 100>, 100> hoges1 (2);
	hoges1 [0] = make_hoges<100> (3, 1);
	hoges1 [1] = make_hoges<100> (3, 4);

	cached_ptr<cached_ptr<Hoge, 100>, 100> hoges2 (3);
	hoges2 [0] = make_hoges<100> (3, 1);
	hoges2 [1] = make_hoges<100> (3, 4);
	hoges2 [2] = make_hoges<100> (3, 7);

	return ! compare (hoges1, hoges2);
}

bool group__test ()
{
	puts ("group__test");
	cached_ptr<cached_ptr<Hoge, 100>, 100> good (3);
	good [0] = make_hoges<100> (3, 1);
	good [1] = make_hoges<100> (3, 4);
	good [2] = make_hoges<100> (3, 7);
	auto hoges =
		group<3> (
		make_hoges<100> (9)
		);
	return compare (hoges, good);
}

bool group__test2 ()
{
	puts ("group__test # remainder");
	cached_ptr<cached_ptr<Hoge, 100>, 100> good (3);
	good [0] = make_hoges<100> (3, 1);
	good [1] = make_hoges<100> (3, 4);
	good [2] = make_hoges<100> (2, 7);
	auto hoges =
		group<3> (
		make_hoges<100> (8)
		);
	return compare (hoges, good);
}

bool join__test ()
{
	puts ("join__test");
	auto good = make_hoges<100> (10);
	
	auto hoges =
		join (
		group<3> (
		make_hoges<100> (10)
		));
	return compare (hoges, good);
}

bool assort__test ()
{
	puts ("assort__test");
	auto good =
		group<4> (
		cached_ptr<Hoge, 100> {1, 3, 5, 7, 2, 4, 6, 8}
		);
	auto hoges =
		assort<2> (//2,
			[] (const Hoge & hoge) {
				return hoge.get_num() % 2 ? 0 : 1;
			},
		make_hoges<100> (8)
		);
	return compare (hoges, good);
}

bool combine__test ()
{
	puts ("combine__test");
	auto good = make_hoges<100> (10);
	auto hoges =
		combine (
			make_hoges<100> (4, 1),
			make_hoges<100> (6, 5)
		);
	return compare (hoges, good);
}

bool filter__test ()
{
	puts ("filter__test");
	auto good = make_hoges<100> (5, 2, 2);
	auto hoges =
		filter (
			[] (const Hoge & hoge) {
				return !(hoge.get_num() % 2);
			},
		make_hoges<100> (10)
		);
	return compare (hoges, good);
}

bool fold__test ()
{
	puts ("fold__test");
	auto sum =
		fold (0,
			[] (int && acc, Hoge && hoge) {
				acc += hoge.get_num();
				return acc;
			},
		make_hoges<100> (5)
		);
	return sum == 15;
}

bool take__test ()
{
	puts ("left__test");
	auto good = make_hoges<100> (5);
	auto hoges =
		take (5,
		make_hoges<100> (10)
		);
	return compare(hoges, good);
}

bool map__test ()
{
	puts ("map__test");
	
	struct number {
		int num;
		bool operator == (const number & n) const
			{ return num == n.num; }
	};
	int inits[] = {2, 4, 6, 8, 10};
	cached_ptr<number, 100> good (5,
		[&] (int i) {
			return number {inits [i]};
		});
	cached_ptr<number, 100> temp (5);
	for (int i=0; i < 5; i++)
		temp [i] = number {inits [i]};
	if (!compare (temp, good))
		puts ("...something wrong!");
	auto nums =
		map (
			[](Hoge && hoge) {
				int num = hoge.get_num ();
				return number {num * 2};
			},
		make_hoges<100> (5)
		);
	return compare (nums, good);
}

bool map__test2 ()
{
	puts ("map__test");
	
	auto good = make_hoges<100> (5, 2, 2);
	auto hoges =
		map (
			[](Hoge && hoge) {
				int num = hoge.get_num ();
				hoge = Hoge (num * 2);
				return std::move (hoge);
			},
		make_hoges<100> (5)
		);
	
	return compare (hoges, good);
}

bool reduce__test ()
{
	puts ("reduce__test");
	auto good = Hoge (15);
	auto hoge =
		reduce (
			[] (Hoge && acc, Hoge && hoge) {
				int num_1 = acc.get_num ();
				int num_2 = hoge.get_num ();
				acc.set_num (num_1 + num_2);
				return std::move (acc);
			},
		make_hoges<100> (5)
		);
	
	return good == hoge;
}

bool refdup__test ()
{
	puts ("refdup__test");
	auto hoges = make_hoges<100> (10);
	auto refs =
		take (5,
		refdup (hoges)
		);
	
	for (int i=0; i<5; i++)
		if (hoges[i].get_num() != refs[i]->get_num())
			return false;
	
	if (hoges[9].get_num() != 10)
		return false;
	
	return true;
}

bool refdup__test2 ()
{
	puts ("refdup__test2");
	int inits [] = {0, 1, 2, 3, 4, 5};
	cached_ptr<int, 100> ints (6,
		[&] (int i) {
			return inits [i];
		});
	auto refs =
		take (3,
		refdup (ints)
		);
	
	for (int i=0; i<3; i++)
		if (ints[i] != *refs[i])
			return false;
	
	if (ints [5] != 5)
		return false;
	
	return true;
}

bool refdup__test3 ()
{
	puts ("refdup__test3");
	auto hoges = make_hoges<100> (10);
	auto folded =
		fold (Hoge (0),
			[] (Hoge && acc, Hoge * hoge) {
				acc.set_num (acc.get_num() + hoge->get_num());
				return std::move (acc);
			},
		take (5,
		refdup (hoges)
		));
	
	if (folded.get_num() != 15)
		return false;
	
	if (!(hoges [0].get_num() == 1 && hoges[9].get_num() == 10))
		return false;
	
	return true;
}

bool reverse__test () {
	puts ("reverse__test");
	auto good = make_hoges<100> (10, 10, -1);
	auto hoges =
		reverse (
		make_hoges<100> (10)
		);
	return compare(hoges, good);
}

bool drop__test ()
{
	puts ("drop__test");
	auto good = make_hoges<100> (6, 5);
	auto hoges =
		drop (4,
		make_hoges<100> (10)
		);
	return compare (good, hoges);
}

bool shuffle__test ()
{
	puts ("shuffle__test");
	auto hoges = make_hoges<100> (10);
	auto shuffled =
		shuffle (
		make_hoges<100> (10));
	return ! compare (hoges, shuffled);
}

void overwrite__test ()
{
	cached_ptr <Hoge, 100> hoge (1);
	*hoge = Hoge(1);
	cached_ptr<const Hoge, 100> hoge2 (1);
//	*hoge2 = Hoge(1);
//	hoge2->set_num(1);
}

bool foreach__test ()
{
	puts ("foreach__test");
	auto good = make_hoges<100> (5);
	auto hoges =
		foreach (1,
			[] (int && acc, const Hoge & hoge) {
				std::cout << acc++ << ':' << hoge << std::endl;
				return std::move (acc);
			},
		make_hoges<100> (5)
		);
		
	return compare (hoges, good);
}

bool display__test ()
{
	puts ("display__test");
	auto good = make_hoges<100> (5);
	auto hoges =
		display ("hoges",
		make_hoges<100> (5)
		);
	display_of (hoges, "hoges");
	return compare (hoges, good);
}

bool fold_of__test ()
{
	puts ("fold_of__test");
	auto good =	15;
	auto hoges = make_hoges<100> (5);
	auto folded = fold_of (hoges, 0,
		[] (int && acc, const Hoge & hoge) {
			acc += hoge.get_num();
			return std::move (acc);
		});
	return folded == good && compare (hoges, make_hoges<100> (5));
}

bool sort__test ()
{
	puts ("sort__test");
	auto good = make_hoges<100> (10);
	auto hoges =
		display ("sorted",
		sort (
			[] (const Hoge & x, const Hoge & y) {
				return y.get_num() < x.get_num();
			},
		shuffle (
		make_hoges<100> (10)
		)));
	return compare(good, hoges);
}

bool map_of__test ()
{
	puts ("map_of__test");
	auto good = progress<100> (5, 2,
		[] (int & n) {
			return n + 2;
		});
	auto hoges = make_hoges<100> (5);
	auto mapped = map_of (hoges,
		[](const Hoge & hoge) {
			int num = hoge.get_num ();
			return num * 2;
		});
	
	return compare (mapped, good);
}
/*
bool product_of__test ()
{
	puts ("product_of__test");
	int inits[] = {2, 2, 2, 4, 4, 2, 4, 4};
	auto good = make_cached_ptr<Hoge, 100> (sizeof(inits), inits);
	auto vec1 = make_hoges<100> (4);
	auto vec2 = make_hoges<100> (4);
	auto hoges =
		product_of (vec1, [&] (const Hoge & hoge1) {
			return product_of (vec2, [&] (const Hoge & hoge2) {
				int num1 = hoge1.get_num();
				int num2 = hoge2.get_num();
				int inits[] = {num1, num2};
				return num1 % 2 || num2 % 2 ?
					cached_ptr<Hoge, 100> (0):
					make_cached_ptr<Hoge, 100> (sizeof(inits), inits);
			});
		});
	return compare (good, hoges);
}
*/
template <class T, size_t C, class F>
cached_ptr<T, C> quick_sort (F && f, cached_ptr<T, C> && vec) {
	struct {
		F & f;
		cached_ptr<T, C> loop (cached_ptr<T, C> & vec) {
			return !vec.size() ?
				std::move (vec):
				let ([&] () {
					T tail = std::move (vec [vec.size() - 1]);
					auto ass =
						assort<2> (//2,
							[&] (T & elem) {
								return this->f (elem, tail);
							},
							take (vec.size() - 1, std::move (vec)));
					return combine (
						append (std::move (tail), loop (ass [1])),
						loop (ass [0]));
				});
		}
	} self = {f};
	return self.loop (vec);
}

cached_ptr<Fuga, 100000> eratosthenes_loop (int x)
{
	using fuga_vector = cached_ptr<Fuga, 100000>;
	int end = (int)sqrt(x) + 1;
	auto ret = loop (
		std::make_tuple (true, fuga_vector{},
			progress<100000> (x - 1, Fuga (x),
				[] (Fuga & pre) {
					return Fuga (pre.get_num() - 1);
				}),
			fuga_vector (0)),
		[&] (std::tuple<bool, fuga_vector, fuga_vector, fuga_vector> && arg) {
			auto & s_vec = std::get<2>(arg);
			auto & p_vec = std::get<3>(arg);
			auto & head = s_vec [s_vec.size() - 1];
			return head.get_num() >= end ?
				std::make_tuple (false,
					reverse (combine (std::move (s_vec), reverse (std::move (p_vec)))),
					fuga_vector{}, fuga_vector{}):
				let ([&] () {
					int head_num = head.get_num();
					auto new_pvec = append (std::move (head), std::move (p_vec));
					auto new_svec =
						filter (
							[&] (Fuga & fuga) {
								return fuga.get_num() % head_num;
							},
						take (s_vec.size() -1, std::move (s_vec))
						);
					return std::make_tuple (true, std::move (std::get<1>(arg)),
						std::move (new_svec), std::move (new_pvec));
				});
			});
	return std::move (std::get<1> (ret));
}

cached_ptr<Hoge, 100000> eratosthenes (int x)
{
	using hoge_vector = cached_ptr<Hoge, 100000>;
	auto s_vec = progress<100000> (x - 1, Hoge (x),
		[] (Hoge & pre) {
			return Hoge (pre.get_num() - 1);
		});
	auto p_vec = hoge_vector (0);
	struct {
		int end;
		hoge_vector loop (hoge_vector & s_vec, hoge_vector & p_vec) {
			auto & head = s_vec [s_vec.size() - 1];
			return head.get_num() >= end ?
				reverse (combine (std::move (s_vec), reverse (std::move (p_vec)))):
				let ([&] () {
					int head_num = head.get_num();
					auto new_pvec = append (std::move (head), std::move (p_vec));
					auto new_svec =
						filter (
							[&] (Hoge & hoge) {
								return hoge.get_num() % head_num;
							},
						take (s_vec.size() -1, std::move (s_vec))
						);
					return loop (new_svec, new_pvec);
				});
		}
	} self = {(int)sqrt(x) + 1};
	return self.loop (s_vec, p_vec);
}

bool loop__test () {
	puts ("loop__test");
	auto ret = loop (std::make_tuple (true, 1),
		[] (std::tuple<bool, int> && arg) {
			int num = std::get<1> (arg) + 1;
			return std::make_tuple (num < 5, num);
		});
	return std::get<1> (ret) == 5;
}

bool let__test () {
	puts ("let__test");
	return let ([]() {
		return true;
	});
}

bool find_of__test ()
{
	puts ("find_of__test");
	auto hoges = make_hoges<100> (10);
	auto pos = find_of (hoges,
		[] (const Hoge & hoge) {
			return hoge.get_num() == 5;
		});
	return pos == 4;
}

bool drop_while__test ()
{
	puts ("drop_while__test");
	auto good = make_hoges<100> (5, 6);
	auto hoges =
		drop_while(
			[] (const Hoge & hoge) {
				return hoge.get_num() <= 5;
			},
		make_hoges<100> (10)
		);
	return compare (good, hoges);
}

bool take_while__test ()
{
	puts ("take_while__test");
	auto good = make_hoges<100> (5);
	auto hoges =
		take_while (
			[] (const Hoge & hoge) {
				return hoge.get_num () <= 5;
			},
		make_hoges<100> (10)
		);
	return compare (good, hoges);
}

bool quick_sort__test ()
{
	puts ("quick_sort__test");
	auto good = make_hoges<100> (10);
	auto hoges =
		quick_sort (
			[] (const Hoge & x, const Hoge & y) {
				return y.get_num() > x.get_num();
			},
		shuffle (
		make_hoges<100> (10)
		));
	return compare(good, hoges);
}

bool make_vector__test ()
{
	puts ("make_vector__test");
	cached_ptr<Fuga, 100> fugas = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	for (int i=0; i<10; i++)
		if (fugas[i].get_num() != i + 1)
			return false;
	return true;
}

bool compare__ttest2 ()
{
	puts ("compare__ttest2");
	cached_ptr<Fuga, 100> fugas1 = {1, 2, 3, 4, 5};
	cached_ptr<Fuga, 100> fugas2 = {1, 2, 3, 4, 5};
	return compare (fugas1, fugas2);
}

bool compare__ftest3 ()
{
	puts ("compare__ftest3");
	cached_ptr<Fuga, 100> fugas1 = {1, 2, 3, 4, 5};
	cached_ptr<Fuga, 100> fugas2 = {1, 2, 4, 8};
	return !compare (fugas1, fugas2);
}

bool eratosthenes__test () {
	puts ("eratosthenes__test");
	auto good = cached_ptr<Hoge, 100000> {2, 3, 5, 7, 11, 13};
	auto hoges = eratosthenes (15);
	return compare (good, hoges);
}

bool eratosthenes_loop__test () {
	puts ("eratosthenes_loop__test");
	auto good = cached_ptr<Fuga, 100000>{2, 3, 5, 7, 11, 13};
	auto hoges = eratosthenes_loop (15);
	return compare (good, hoges);
}

void qiita_test () {
	auto hoge =
		fold (Hoge (1),
			[] (Hoge && x, Fuga && y) {
				int xn = x.get_num();
				int yn = y.get_num();
				x.set_num (xn + yn);
				return std::move (x);
			},
		filter (
			[] (const Fuga & fuga) {
				int num = fuga.get_num();
				return
					!(num % 2) ?
						false:
					!(num % 3) ?
						false:
					true;
			},
		map (
			[] (Hoge && hoge) {
				return Fuga (hoge.get_num() * 5);
			},
		progress<1000> (1000, Hoge(1),
			[] (const Hoge & hoge) {
				return Hoge (hoge.get_num() + 1);
			}
		))));
	std::cout << hoge.get_num() << std::endl;

	auto hoges (
		progress<1000> (10, Hoge (1),
			[] (const Hoge & hoge) {
				int num = hoge.get_fuga()->get_num();
				return Hoge (num * 2);
			}));
	std::cout << hoges << std::endl;
	auto refs (
		take (2,
		group<3> (
		take (8,
		reverse (
		refdup (hoges)
		)))));

	std::cout << refs << std::endl;
	std::cout << (*refs[0][0]).get_num() << std::endl;
	std::cout << hoges << std::endl;
}

bool test_all () {
	return
		progress__test () &&
		make_hoges__test () &&
		make_hoges__test2 () &&
		make_hoges__test3 () &&
		compare__ttest () &&
		compare__ftest0 () &&
		compare__ftest1 () &&
		append__test () &&
		compare__ttest1 () &&
		compare__ftest2 () &&
		group__test () &&
		group__test2 () &&
		join__test () &&
		assort__test () &&
		combine__test () &&
		filter__test () &&
		fold__test () &&
		take__test () &&
		map__test () &&
		map__test2 () &&
		reduce__test () &&
		refdup__test2 () &&
		refdup__test3 () &&
		reverse__test () &&
		drop__test () &&
		shuffle__test () &&
		foreach__test () &&
		display__test () &&
		fold_of__test () &&
		sort__test () &&
		map_of__test () &&
		loop__test () &&
		let__test () &&
		drop_while__test () &&
		find_of__test () &&
		take_while__test () &&
		quick_sort__test () &&
		make_vector__test () &&
		compare__ttest2 () &&
		compare__ftest3 () &&
		eratosthenes__test () &&
		eratosthenes_loop__test ();
}

