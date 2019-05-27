// ConsoleApplication2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

// ConsoleApplication2.cpp : 定义控制台应用程序的入口点。
//

#include <iostream>
#include <functional>
#include <algorithm>
#include <utility>
#include <iterator>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <iostream>
#include <time.h>
#include <omp.h>
#include <thread>
#include <random>
#include <future>
#include <execution>

using namespace std;

void stdsort(vector<string>::iterator start, vector<string>::iterator end)
{
	sort(start, end);
}

template<typename Iter>
void mergesort_mt1(Iter begin, Iter end,
	unsigned int N = std::thread::hardware_concurrency())
{
	auto len = std::distance(begin, end);
	if (len < 2)
		return;

	Iter mid = std::next(begin, len / 2);
	if (N > 1)
	{
		auto fn = std::async(mergesort_mt1<Iter>, begin, mid, N - 2);
		mergesort_mt1(mid, end, N - 2);
		fn.wait();
	}
	else
	{
		mergesort_mt1(begin, mid, 0);
		mergesort_mt1(mid, end, 0);
	}

	std::inplace_merge(begin, mid, end);
}

template<typename Iter>
void mergesort_mt3(Iter begin, Iter end,
	unsigned int N = std::thread::hardware_concurrency())
{
	auto len = std::distance(begin, end);
	if (len <= 1024 || N < 2)
	{
		std::sort(begin, end);
		return;
	}

	Iter mid = std::next(begin, len / 2);
	auto fn = std::async(mergesort_mt3<Iter>, begin, mid, N - 2);
	mergesort_mt3(mid, end, N - 2);
	fn.wait();
	std::inplace_merge(begin, mid, end);
}

int main()
{
	clock_t t_start, t_end;
	vector<string> words;
	for (char alp1 = 'A'; alp1 <= 'Z'; alp1++)
	{
		for (char alp2 = 'A'; alp2 <= 'Z'; alp2++)
		{
			for (char alp3 = 'A'; alp3 <= 'Z'; alp3++)
			{
				for (char alp4 = 'A'; alp4 <= 'Z'; alp4++)
				{
					string word;
					word += alp1;
					word += alp2;
					word += alp3;
					word += alp4;
					words.push_back(word);
				}
			}
		}
	}
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(words.begin(), words.end(), g);
	cout << "shuffle complete" << endl;
	vector<string> words_copy1 = words;
	cout << "words_copy1 complete" << endl;
	vector<string> words_copy2 = words;
	t_start = clock();
	sort(std::execution::par,words_copy1.begin(), words_copy1.end());
	t_end = clock();
	cout << "time parallel: " << (t_end - t_start) * 1.0 / CLOCKS_PER_SEC << endl;
	t_start = clock();
	sort(words_copy2.begin(), words_copy2.end());
	t_end = clock();
	cout << "time single: " << (t_end - t_start) * 1.0 / CLOCKS_PER_SEC << endl;

	cout << "Parallel std sort start" << endl;
	vector<string> words_copy5 = words;
	t_start = clock();

	//const int threads_num = std::thread::hardware_concurrency();
	const int threads_num = 4;
	vector<thread> threads(threads_num);
	vector<vector<string>::iterator> indexs;
	auto length = words_copy5.end() - words_copy5.begin();
	for (auto i = 0; i < threads_num; ++i)
	{
		indexs.push_back(words_copy5.begin() + (length / threads_num * i));
	}
	indexs.push_back(words_copy5.end());
	for (auto i = 0; i < threads_num; ++i)
	{
		threads[i] = thread(stdsort, indexs[i], indexs[i + 1]);
	}
	for (auto i = 0; i < threads_num; ++i)
	{
		threads[i].join();
	}
	for (unsigned int i = 1; i < indexs.size() - 1; ++i)
	{
		std::inplace_merge(indexs[0], indexs[i], indexs[i + 1]);
	}
	t_end = clock();
	cout << "Parallel std sort time: " << (t_end - t_start) * 1.0 / CLOCKS_PER_SEC << endl;

	////////////////////////////////
	cout << "mergesort_mt1 std sort start" << endl;
	vector<string> words_copy6 = words;
	t_start = clock();
	//const int threads_num = std::thread::hardware_concurrency();
	mergesort_mt1(words_copy6.begin(), words_copy6.end());
	t_end = clock();
	cout << "mergesort_mt1 std sort time: " << (t_end - t_start) * 1.0 / CLOCKS_PER_SEC << endl;

	////////////////////////////////
	cout << "mergesort_mt3 std sort start" << endl;
	vector<string> words_copy8 = words;
	t_start = clock();
	//const int threads_num = std::thread::hardware_concurrency();
	mergesort_mt3(words_copy8.begin(), words_copy8.end());
	t_end = clock();
	cout << "mergesort_mt3 std sort time: " << (t_end - t_start) * 1.0 / CLOCKS_PER_SEC << endl;
}
