#ifndef TEXTQUERY
#define TEXTQUERY

#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <functional>
#include <algorithm>
#include <iostream>
#include <string>
#include <future>
#include <fstream>

class QueryResult;
class TextQuery
{
public:
	using WordlistType = std::vector<std::string>;
	using ThreadsType = std::vector<std::thread>;
	using IteratorType = std::vector<std::string>::iterator;
	using ConstIteratorType = std::vector<std::string>::const_iterator;
	using BinsType = std::vector<IteratorType>;
	using ConstBinsType = std::vector<ConstIteratorType>;
	using ClockType = std::chrono::system_clock;
	TextQuery(const WordlistType& wordlist);
	TextQuery(const std::string path);
	void ParallelQuery(const std::string&, unsigned int threads_num = std::thread::hardware_concurrency()-1);//
	void SequenQuery(const std::string&);
	void print();
	const WordlistType& getwordlist() { return *_wordlist; }

private:
	void query(const std::string &want, ConstIteratorType begin, ConstIteratorType end);
	void seqquery(const std::string &want, ConstIteratorType begin, ConstIteratorType end);
	void stdcopy(std::vector<std::string>::const_iterator begin, std::vector<std::string>::const_iterator end, IteratorType dest);
	void ParalleCopy(const WordlistType& wordlist, unsigned int threads_num = std::thread::hardware_concurrency());
	std::shared_ptr<WordlistType> _wordlist;
	std::shared_ptr<WordlistType> _result;
	std::mutex mylock;
};

void TextQuery::stdcopy(std::vector<std::string>::const_iterator begin, std::vector<std::string>::const_iterator end, IteratorType dest)
{
	std::copy(begin, end, dest);
}
void TextQuery::ParalleCopy(const WordlistType& wordlist, unsigned int threads_num)
{
	*_wordlist = WordlistType(wordlist.size());
	ThreadsType threads(threads_num);
	std::vector<std::vector<std::string>::const_iterator> indexs;
	auto c_start = ClockType::now();
	unsigned int length = wordlist.end() - wordlist.begin();
	for (unsigned int i = 0; i < threads_num; ++i)
	{
		indexs.push_back(wordlist.cbegin() + (length / threads_num * i));
	}
	indexs.push_back(_wordlist->end());

	BinsType indexs_in;
	for (unsigned int i = 0; i < threads_num; ++i)
	{
		indexs_in.push_back(_wordlist->begin() + (length / threads_num * i));
	}

	for (unsigned int i = 0; i < threads_num; ++i)
	{
		threads[i] = std::thread(std::bind(&TextQuery::stdcopy, this, indexs[i], indexs[i + 1], indexs_in[i]));
	}
	for (unsigned int i = 0; i < threads_num; ++i)
	{
		threads[i].join();
	}
	auto c_end = ClockType::now();
	std::cout << "Copy complete " << std::endl;
	std::cout << "Time use: " << std::chrono::duration_cast<std::chrono::milliseconds>(c_end - c_start).count() << "ms\n";
}


TextQuery::TextQuery(const std::string path) :_wordlist(new WordlistType), _result(new WordlistType)
{
	std::ifstream file;
	file.open("out.txt");
	std:: string w;
	WordlistType ws;
	auto c_start = ClockType::now();
	if (!file.is_open())
	{
		std::cout << "Error opening file"; exit(1);
	}
	while (std::getline(file, w))
	{
		_wordlist->push_back(w);
	}
	auto c_end = ClockType::now();
	std::cout << "Copy time use: " << std::chrono::duration_cast<std::chrono::milliseconds>(c_end - c_start).count() << "ms\n";
}

TextQuery::TextQuery(const WordlistType& wordlist) :_wordlist(new WordlistType), _result(new WordlistType)
{
	auto c_start = ClockType::now();
	*_wordlist = WordlistType(wordlist.cbegin(), wordlist.cend());
	auto c_end = ClockType::now();
	std::cout << "Copy time use: " << std::chrono::duration_cast<std::chrono::milliseconds>(c_end - c_start).count() << "ms\n";

	/*c_start = ClockType::now();
	ParalleCopy(wordlist);
	c_end = ClockType::now();
	std::cout << "Copy time use: " << std::chrono::duration_cast<std::chrono::milliseconds>(c_end - c_start).count() << "ms\n";*/
}

void TextQuery::query(const std::string &want, ConstIteratorType begin, ConstIteratorType end)
{
	for (ConstIteratorType it = begin; it != end; it++)
	{
		if (!want.compare(0, want.length(), *it, 0, want.length()))
		{
			//mylock.lock();
			_result->push_back(*it);
			//mylock.unlock();
		}
	}
}

void TextQuery::seqquery(const std::string &want, ConstIteratorType begin, ConstIteratorType end)
{
	for (ConstIteratorType it = begin; it != end; it++)
	{
		if (!want.compare(0, want.length(), *it, 0, want.length()))
		{
			_result->push_back(*it);
		}
	}
}

void TextQuery::ParallelQuery(const std::string& want, unsigned int threads_num)
{
	_result->clear();
	_result->shrink_to_fit();
	ThreadsType threads(threads_num);
	ConstBinsType indexs;
	auto c_start = ClockType::now();
	unsigned int length = _wordlist->end() - _wordlist->begin();
	for (unsigned int i = 0; i < threads_num; ++i)
	{
		indexs.push_back(ConstIteratorType(_wordlist->cbegin() + (length / threads_num * i)));
	}
	indexs.push_back(_wordlist->cend());


	


	for (unsigned int i = 0; i < threads_num; ++i)
	{
		//threads[i] = std::thread(std::bind(&TextQuery::query, this, want, indexs[i], indexs[i + 1]));
		auto fn = std::async(std::bind(&TextQuery::query, this, want, indexs[i], indexs[i + 1]));
	}
	for (unsigned int i = 0; i < threads_num; ++i)
	{
		threads[i].join();
	}
	auto c_end = ClockType::now();
	std::cout << "Parallel Time use: " << std::chrono::duration_cast<std::chrono::milliseconds>(c_end - c_start).count() << "ms\n";
	//print();
}

void TextQuery::SequenQuery(const std::string& want)
{
	_result->clear();
	_result->shrink_to_fit();
	auto c_start = ClockType::now();
	unsigned int length = _wordlist->cend() - _wordlist->cbegin();
	seqquery(want, _wordlist->cbegin(), _wordlist->cend());
	auto c_end = ClockType::now();
	std::cout << "Seq Time use: " << std::chrono::duration_cast<std::chrono::milliseconds>(c_end - c_start).count() << "ms\n";
}

void TextQuery::print()
{
	for (unsigned int i = 0; i < _result->size(); ++i)
		std::cout << (*_result)[i] << std::endl;
}










#endif
