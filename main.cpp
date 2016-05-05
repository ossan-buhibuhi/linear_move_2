//
//  main.cpp
//
//  Copyright (c) 2016 Matsusaki Satoru. All rights reserved.
//
//  Released under the MIT license
//  http://opensource.org/licenses/mit-license.php
//

#include <future>

#include "linear_move_2_test.h"
#include "linear_move_2.h"
#include "ThreadPool.h"

using namespace lm2;

bool do_test (ThreadPool & pool) {
	pool.enqueue ([](){
		lm2::make_memory_cache<800>(9);
		lm2::make_memory_cache<1600>(2);
		lm2::make_memory_cache<528>(1);
		lm2::make_memory_cache<400>(3);
		lm2::make_memory_cache<32>(5);
		lm2::make_memory_cache<800000>(3);
		lm2::make_memory_cache<400000>(6);
		lm2::make_memory_cache<4>(1);
		lm2::make_memory_cache<800>(1);
		lm2::make_memory_cache<400>(1);
	});
	std::future<cached_ptr<Fuga>> future;
	future = pool.enqueue ([](){
		cached_ptr<Fuga> ret = {test_all()};
		return ret;
	});
	cached_ptr<Fuga> res = future.get ();
	puts (res->get_num() ? "good." : "bad.");
	
	return true;
}

void foo ()
{
	cached_ptr<Fuga> fuga = 1;
	printf ("%d\n", fuga->get_num());
	
	cached_ptr<cached_ptr<Fuga>, 100> fugas = {1, 2, 3};
	display_of (fugas, "fugas");
	auto res = filter(
		[] (cached_ptr<Fuga> & fuga) {
			return fuga[0].get_num() % 2;
		},
		std::move (fugas));
	display_of (res, "filtered");
}

int main (int argc, const char * argv[])
{
	ThreadPool pool (1);
	do_test (pool);
	
	foo ();
	
	printf ("Fuga::copy_cnt = %d\n", Fuga::copy_cnt);
	printf ("Fuga::life_cnt = %d\n", Fuga::life_cnt);
	printf ("Hoge::copy_cnt = %d\n", Hoge::copy_cnt);
	printf ("Hoge::life_cnt = %d\n", Hoge::life_cnt);
	
	return 0;
}
