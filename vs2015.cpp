// ConsoleApplication2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
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
#include <future>


using namespace std;

vector<string> make_words()
{
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
	return words;
}

bool check_start(const string &s, const string &want)
{
	return (s >= want);
}


void BubbleSort(vector<string> &a)
{
	for (unsigned int i = 0; i<a.size(); i++)
	{
		bool flag = true;
		for (unsigned int j = 0; j<a.size() - i - 1; j++)
		{
			if (a[j]>a[j + 1])
			{
				swap(a[j + 1], a[j]);
				flag = false;
			}
		}
		if (true == flag)return;
	}
}

int Partition(vector<string> &list, int low, int high)
{
	auto pivotKey = list[low];
	while (low<high)
	{
		while (low<high&&list[high] >= pivotKey)
		{
			high--;
		}
		swap(list[low], list[high]);
		while (low<high&&list[low] <= pivotKey)
		{
			low++;
		}
		swap(list[low], list[high]);
	}
	return low;
}

int Partition_P(vector<string> &list, int low, int high, string pivotKey)
{
	while (low<high)
	{
		while (low<high&&list[high] >= pivotKey)
		{
			high--;
		}
		swap(list[low], list[high]);
		while (low<high&&list[low] <= pivotKey)
		{
			low++;
		}
		swap(list[low], list[high]);
	}
	return low;
}

void Qsort(vector<string> &list, int low, int high)
{
	int pivot;
	if (low<high)
	{
		pivot = Partition(list, low, high);
		Qsort(list, low, pivot - 1);
		Qsort(list, pivot + 1, high);
	}

}

void Quick_Sort(vector<string> &list)
{
	Qsort(list, 0, list.size() - 1);
}

void stdsort(vector<string>::iterator start, vector<string>::iterator end)
{
	sort(start, end);
}

vector<string> merge(const vector<string> &vec, vector<string>::const_iterator start_add, vector<string>::const_iterator end_add)
{
	auto start = vec.begin();
	auto end = vec.end();
	if (start == end)
		return vector<string>(start_add, end_add);
	else
	{
		vector<string> out;
		while (start != end && start_add != end_add)
		{
			if (*start <= *start_add)
				out.push_back(*start++);
			else
				out.push_back(*start_add++);
		}
		while (start != end)
			out.push_back(*start++);
		while (start_add != end_add)
			out.push_back(*start_add++);
		return out;
	}
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
void mergesort_mt2(Iter begin, Iter end,
	unsigned int N = std::thread::hardware_concurrency())
{
	auto len = std::distance(begin, end);
	if (len <= 1024)
	{
		std::sort(begin, end);
		return;
	}

	Iter mid = std::next(begin, len / 2);
	if (N > 1)
	{
		auto fn = std::async(mergesort_mt2<Iter>, begin, mid, N - 2);
		mergesort_mt2(mid, end, N - 2);
		fn.wait();
	}
	else
	{
		mergesort_mt2(begin, mid, 0);
		mergesort_mt2(mid, end, 0);
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
	vector<string> words = make_words();
	cout << "words complete " << endl;
	random_shuffle(words.begin(), words.end());
	vector<string> words_copy1=words;
	cout << "words_copy1 complete " << endl;
	t_start = clock();
	sort(words_copy1.begin(), words_copy1.end());
	t_end = clock();
	cout << "STL Sort time: " << (t_end - t_start)*1.0 / CLOCKS_PER_SEC << endl;

	/*string want = "AA";
	t_start = clock();
	auto first = find_if(words_copy1.begin(), words_copy1.end(), bind(check_start, placeholders::_1, want));
	cout << *first << endl;
	want.back() += 1;
	auto end = find_if(words_copy1.begin(), words_copy1.end(), bind(check_start, placeholders::_1, want));
	t_end = clock();
	cout << *(end-1) << endl;
	cout << "Search time: " << (t_end - t_start)*1.0 / CLOCKS_PER_SEC << endl;
	for (auto it = first; it < end; it++)
	{
		cout << *it << endl;
	}*/
	/*vector<string> words_copy2 = words;
	cout << "Quick_Sort start" << endl;
	t_start = clock();
	Quick_Sort(words_copy2);
	t_end = clock();
	cout << "Quick_Sort time: " << (t_end - t_start)*1.0 / CLOCKS_PER_SEC << endl;*/
	
	vector<string> words_copy3 = words;
	cout << "Parallel quick sort start" << endl;
	t_start = clock();
	
	auto x = Partition_P(words_copy3, 0, words_copy3.size()-1, "NNNN");
	vector<string> v_s1(words_copy3.begin(), words_copy3.begin()+x);
	vector<string> v_s2(words_copy3.begin() + x, words_copy3.end());
	x = Partition_P(v_s1, 0, v_s1.size() - 1,"GGGG");
	vector<string> v_t1(v_s1.begin(), v_s1.begin() + x);
	vector<string> v_t2(v_s1.begin() + x, v_s1.end());
	x = Partition_P(v_s2, 0, v_s2.size() - 1, "TTTT");
	vector<string> v_t3(v_s2.begin(), v_s2.begin() + x);
	vector<string> v_t4(v_s2.begin() + x, v_s2.end());
	clock_t t_copy;
	t_copy = clock();
	thread t1(Quick_Sort, ref(v_t1));
	thread t2(Quick_Sort, ref(v_t2));
	thread t3(Quick_Sort, ref(v_t3));
	thread t4(Quick_Sort, ref(v_t4));
	t1.join();
	t2.join();
	t3.join();
	t4.join();

	t_end = clock();
	cout << "Copy time: " << (t_copy - t_start)*1.0 / CLOCKS_PER_SEC << endl;
	cout << "Parallel quick sort time: " << (t_end - t_start)*1.0 / CLOCKS_PER_SEC << endl;

	////////////////////////////////
	cout << "Parallel std sort start" << endl;
	vector<string> words_copy5 = words;
	t_start = clock();
	//const int threads_num = std::thread::hardware_concurrency();
	const int threads_num =4;
	vector<thread> threads(threads_num );
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
		std::inplace_merge(indexs[0], indexs[i], indexs[i+1]);
	}
	t_end = clock();
	cout << "Parallel std sort time: " << (t_end - t_start)*1.0 / CLOCKS_PER_SEC << endl;


	////////////////////////////////
	cout << "mergesort_mt1 std sort start" << endl;
	vector<string> words_copy6 = words;
	t_start = clock();
	//const int threads_num = std::thread::hardware_concurrency();
	mergesort_mt1(words_copy6.begin(), words_copy6.end());
	t_end = clock();
	cout << "mergesort_mt1 std sort time: " << (t_end - t_start)*1.0 / CLOCKS_PER_SEC << endl;

	////////////////////////////////
	cout << "mergesort_mt2 std sort start" << endl;
	vector<string> words_copy7 = words;
	t_start = clock();
	//const int threads_num = std::thread::hardware_concurrency();
	mergesort_mt2(words_copy7.begin(), words_copy7.end());
	t_end = clock();
	cout << "mergesort_mt2 std sort time: " << (t_end - t_start)*1.0 / CLOCKS_PER_SEC << endl;

	////////////////////////////////
	cout << "mergesort_mt3 std sort start" << endl;
	vector<string> words_copy8 = words;
	t_start = clock();
	//const int threads_num = std::thread::hardware_concurrency();
	mergesort_mt3(words_copy8.begin(), words_copy8.end());
	t_end = clock();
	cout << "mergesort_mt3 std sort time: " << (t_end - t_start)*1.0 / CLOCKS_PER_SEC << endl;
}
