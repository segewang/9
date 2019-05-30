#include <iostream>
#include "TextQuery.h"
#include <chrono>
#include <memory>

using namespace std;
using namespace chrono;
std::mutex mylock;
vector<string> make_wordlist()
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

void search_without_sort(string want, vector<string>::iterator begin, vector<string>::iterator end, vector<string> &out)
{
	for (vector<string>::iterator it = begin; it != end; it++)
	{
		if (!want.compare(0, want.length(), *it, 0, want.length()))
		{
			mylock.lock();
			out.push_back(*it);
			mylock.unlock();
		}
	}
}


int main()
{
	/*cout << "start to creat wordlist " << endl;
	vector<string> wordlist = make_wordlist();
	random_shuffle(wordlist.begin(), wordlist.end());*/
	TextQuery Tq("out.txt");
	cout << "wordlist complete " << endl;
	string want = "AAAA";

	Tq.SequenQuery(want);
	Tq.ParallelQuery(want);

	vector<string> wordlist = Tq.getwordlist();
	vector<string> words_nosort = wordlist;
	vector<string> result;
	auto c_start = system_clock::now();
	string s1 = want;
	for (unsigned int i = 0; i < words_nosort.size(); ++i)
	{
		if (!s1.compare(0, s1.length(), words_nosort[i], 0, s1.length()))
			result.push_back(words_nosort[i]);
	}
	auto c_end = system_clock::now();
	std::cout << "XXXXX time: " << duration_cast<milliseconds>(c_end - c_start).count() << "ms\n";



	vector<string> words_nosort_p = wordlist;
	vector<string> out_nosort_p;
	unsigned int threads_num = 4;
	vector<thread> threads_nosort(threads_num);
	vector<vector<string>::iterator> indexs_nosort;
	c_start = system_clock::now();
	auto length_nosort = words_nosort_p.end() - words_nosort_p.begin();
	for (unsigned int i = 0; i < threads_num; ++i)
	{
		indexs_nosort.push_back(words_nosort_p.begin() + (length_nosort / threads_num * i));
	}
	indexs_nosort.push_back(words_nosort_p.end());
	for (unsigned int i = 0; i < threads_num; ++i)
	{
		threads_nosort[i] = thread(search_without_sort, want, indexs_nosort[i], indexs_nosort[i + 1], std::ref(out_nosort_p));
	}
	for (unsigned int i = 0; i < threads_num; ++i)
	{
		threads_nosort[i].join();
	}
	c_end = system_clock::now();
	std::cout << "XXXXX parallel time: " << duration_cast<milliseconds>(c_end - c_start).count() << "ms\n";
}

