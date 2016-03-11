#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <thread>
#include <future>
#include "ThreadPool.h"

using namespace std;
using namespace std::chrono;

vector<string> ReadFile(const string& filename);
void setArguments(const int& argc, char* argv[], vector<string> &SourceFiles);
void Write(const vector<string> &words, const string &filename);
void Replace(vector<string> &words, const vector<string> &keywords);
void Transformation(const vector<string> &keywords, string FileName);

int main(int argc, char* argv[])
{
	vector<string> Files;
	vector<string> allWords;
	ofstream timeFile("Time.txt", std::ios_base::app);
	setArguments(argc, argv, Files);

	// s'il n'y a pas de fichier en paramètre, prendre ce fichier-ci
	if (!Files.empty())
		Files.push_back("Source.cpp");

	vector <function<void()>> fcts;
	fcts.reserve(Files.size());
	vector<string> keywords = ReadFile("keywords.txt");
	ThreadPool pool;
	string out;

	auto avant = system_clock::now();
	for (string &s : Files)
	{
		fcts.push_back([keywords, Files, &allWords]() {
			for (unsigned int i = 0; i < Files.size(); ++i)
			{
				vector<string> words = ReadFile(Files[i]);
				Replace(words, keywords);
				Write(words, Files.at(i));
				ofstream file(Files[i] + (char)i + ".html");
				for (auto &s : allWords)
					file << s;
				file.close();
			}
		});
	}

	for (auto &f : fcts)
	{
		pool.add_task(f);
	}
	pool.meurs();
	pool.wait_end();
	auto apres = system_clock::now();
	out = "ThreadPool: " + to_string(duration_cast<milliseconds>(apres - avant).count()) + " ms.";
	cout << out << endl;
	timeFile << out << endl;

	avant = system_clock::now();
	for (auto &f : fcts)
	{
		f();
	}
	apres = system_clock::now();
	out = "Séquentielle: " + to_string(duration_cast<milliseconds>(apres - avant).count()) + " ms.";
	cout << out << endl;
	timeFile << out << endl;

	avant = system_clock::now();
	vector<future<void>> v;

	for (auto & f : fcts)
		v.emplace_back(async(f));
	for (auto & f : v)
		f.wait();

	apres = system_clock::now();
	out = "Async: " + to_string(duration_cast<milliseconds>(apres - avant).count()) + " ms.";
	cout << out << endl;
	timeFile << out << endl;
}

void setArguments(const int& argc, char* argv[], vector<string> &SourceFiles)
{
	for (int i = 0; i < argc; ++i)
	{
		string param = argv[i];
		SourceFiles.push_back(param);
	}
}

vector<string> ReadFile(const string& filename)
{
	vector<string> sentences;
	ifstream file(filename);
	if (file)
	{
		for (string line; getline(file, line);)
			sentences.push_back(line);
	}
	return sentences;
}

void Write(const vector<string> &words, const string &filename)
{
	ofstream file(filename + ".html");
	file << "<!DOCTYPE html>" << "<head></head><body>";
	for (unsigned int i = 0; i < words.size(); ++i)
		file << words.at(i) << "<br>" << flush;
	file << "</body></html>";
}

void Replace(vector<string> &words, const vector<string> &keywords)
{
	for (auto debut = begin(words); debut != end(words); ++debut)
	{

		*debut = regex_replace(*debut, regex{ "&" }, "&amp;");
		*debut = regex_replace(*debut, regex{ "<" }, "&lt;");
		*debut = regex_replace(*debut, regex{ ">" }, "&gt;");
		*debut = regex_replace(*debut, regex{ "\\t" }, "&nbsp&nbsp&nbsp&nbsp;");
		for (auto keywordPos = begin(keywords); keywordPos != end(keywords); ++keywordPos)
		{
			regex expression("\\b" + *keywordPos + "\\b");
			*debut = regex_replace(*debut, expression, " <span style='color:blue'> " + *keywordPos + "</span>");
		}
	}

}

void Transformation(const vector<string> &keywords, string FileName)
{
	vector<string> words = ReadFile(FileName);
	Replace(words, keywords);
	Write(words, FileName);
	ofstream file(FileName + ".html");
	for (auto &s : words)
		file << s;
	file.close();
}
