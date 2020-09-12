#pragma once

#include <iostream>

#include "../include/test_runner.h"
#include "../include/profiler.h"

template <typename TCache>
void lineTest()
{
	std::cout << std::endl;
	{
		LOG_DURATION("warst test");
		size_t hit_count = 0u;
		size_t try_count = 0u;

		TCache t(10);

		for (int i = 0; i < 100; i++) {
			t.add(i, [i]() { return new typename TCache::ValType(i); });
		}

		for (int k = 0; k < 100; k++) {
			try_count++;
			if (t.check(k)) { hit_count++; }
			t.get(k);
		}
		std::cout << "try count: " << try_count << std::endl;
		std::cout << "hit count: " << hit_count << std::endl;
	}
	std::cout << std::endl;
	{
		LOG_DURATION("ideal test");
		size_t hit_count = 0u;
		size_t try_count = 0u;

		TCache t(101);

		for (int i = 0; i < 100; i++) {
			t.add(i, [i]() { return new typename TCache::ValType(i); });
		}

		for (int k = 0; k < 5; k++) {
			for (int l = 0; l < 100; l++) {
				try_count++;
				if (t.check(l)) { hit_count++; }
				t.get(l);
			}
		}
		std::cout << "try count: " << try_count << std::endl;
		std::cout << "hit count: " << hit_count << std::endl;
	}
}

template <typename TCache>
void RandTest()
{
	std::cout << std::endl;
	{
		LOG_DURATION("rand test");
		size_t hit_count = 0u;
		size_t try_count = 0u;

		TCache t(100);

		for (int i = 0; i < 100; i++) {
			t.add(i, [i]() { return new typename TCache::ValType(i); });
		}

		for (int l = 0; l < 1000; l++) {
			try_count++;
			int h = std::rand() % 100;

			if (t.check(h)) { hit_count++; }

			t.get(h);
		}

		std::cout << "try count: " << try_count << std::endl;
		std::cout << "hit count: " << hit_count << std::endl;
	}
}

template <typename TCache>
void Test4()
{
	std::cout << std::endl;
	{
		LOG_DURATION("rand test");
		size_t hit_count = 0u;
		size_t try_count = 0u;

		TCache t(100);

		for (int i = 0; i < 100; i++) {
			t.add(i, [i]() { return new typename TCache::ValType(i); });
		}

		for (int k = 0; k < 10; k++) {
			for (int l = 0; l < 100; l++) {
				try_count++;
				int h = std::rand() % (l + 1);
				if (t.check(h)) { hit_count++; }
				t.get(h);
			}
		}
		std::cout << "try count: " << try_count << std::endl;
		std::cout << "hit count: " << hit_count << std::endl;
	}
}

template <typename TCache>
void BlockTest()
{
	std::cout << std::endl;
	{
		//firstly 0 - 10
		//secondly 10 - 20
		//etc
		LOG_DURATION("by block test");
		size_t hit_count = 0u;
		size_t try_count = 0u;

		TCache t(20);

		for (int i = 0; i < 100; i++) {
			t.add(i, [i]() { return new typename TCache::ValType(i); });
		}

		for (int i = 0; i < 10; i++) {
			for (int k = 0; k < 20; k++) {
				try_count++;
				int h = 10 * i + k % 10;

				if (t.check(h)) { hit_count++; }

				t.get(h);
			}
		}
		std::cout << "try count: " << try_count << std::endl;
		std::cout << "hit count: " << hit_count << std::endl;
	}
}

template <typename TCache>
void BlockTest2()
{
	std::cout << std::endl;
	{
		//|1|2|3|4|
		//    |3|4|5|6| 
		//        |5|6|7|8|
		//etc
		LOG_DURATION("by block test");
		size_t hit_count = 0u;
		size_t try_count = 0u;

		TCache t(20);

		for (int i = 0; i < 100; i++) {
			t.add(i, [i]() { return new typename TCache::ValType(i); });
		}

		for (int i = 0; i < 18; i++) {
			for (int k = 0; k < 10; k++) {
				try_count++;
				int h = 5 * i + k % 10;
				if (t.check(h)) { hit_count++; }
				t.get(h);
			}
		}
		std::cout << "try count: " << try_count << std::endl;
		std::cout << "hit count: " << hit_count << std::endl;
	}
}

template <typename T>
void RunTest()
{
	dbg::TestRunner tr;

	std::srand(std::time(nullptr));

	RUN_TEST(tr, lineTest<T>);
	RUN_TEST(tr, RandTest<T>);
	RUN_TEST(tr, Test4<T>);
	RUN_TEST(tr, BlockTest<T>);
	RUN_TEST(tr, BlockTest2<T>);
}