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
using namespace std;
using namespace std::chrono;

vector<string> ReadFile(const string& filename);
void setArguments(const int& argc, char* argv[], vector<string> &SourceFiles, vector<string> &Options);
void Write(const vector<string> &words, const string &filename);
void Replace(vector<string> &words, const vector<string> &keywords);
void addWords(vector<string> &allWords, const  vector<string> & words);
void Transformation(const vector<string> &keywords, vector<string> &allWords, const vector<string> &Files);

int main(int argc, char* argv[])
{
	vector<string> Files;
	vector<string> Options;
	vector<string> allWords;
	setArguments(argc, argv, Files, Options);

	// s'il n'y a pas de fichier en paramètre, prendre ce fichier-ci
	if (!Files.empty())
		Files.push_back("Source.cpp");

	vector<string> keywords = ReadFile("keywords.txt");
	const int NTACHESMAX = thread::hardware_concurrency();

	for (int i = 1; i <= NTACHESMAX; ++i)
	{
		const int NTACHES = Files.size();
		vector <function<void()>> fcts;
		fcts.reserve(NTACHES);
		{
			for (int i = 0; i < NTACHES; ++i)
			{
				fcts.emplace_back([keywords, Files, &allWords]() {
					for (unsigned int i = 0; i < Files.size(); ++i)
					{
						vector<string> words = ReadFile(Files[i]);
						Replace(words, keywords);
						Write(words, Files.at(i));
						addWords(allWords, words);
						ofstream file(Files[i] + (char)i + ".html");
						for (auto &s : allWords)
							file << s;
						file.close();
					}
				});
			}
		}
		cout << string(70, '-') << '\n' << "Test (sequentiel, async, pool...), "
			<< NTACHES << (NTACHES == 1 ? " tache" : " taches") << endl;
		{
			auto avant = system_clock::now();
			for (auto & f : fcts)
				f();
			auto apres = system_clock::now();
			cout << "Execution sequentielle: "
				<< duration_cast<milliseconds>(apres - avant).count() << " ms." << endl;
		}
	}
}

void setArguments(const int& argc, char* argv[], vector<string> &SourceFiles, vector<string> &Options)
{
	for (int i = 0; i < argc; ++i)
	{
		string param = argv[i];
		if (param[0] == '-' || param[0] == '/')
			Options.push_back(param.substr(1));
		else
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

void Write(const vector<string> &words,const string &filename)
{
	ofstream file(filename + ".html");
	file << "<!DOCTYPE html>" << "<head></head><body>";
	for (unsigned int i = 0; i < words.size(); ++i)
		file << words.at(i) << "<br>" << flush;
	file << "</body></html>";
}

void Replace(vector<string> &words, const vector<string> &keywords)
{
	for(auto debut = begin(words); debut != end(words); ++debut)
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

void addWords(vector<string> &allWords,const  vector<string> & words)
{
	allWords.insert(end(allWords), begin(words), end(words));
}

void Transformation(const vector<string> &keywords, vector<string> &allWords, const vector<string> &Files)
{
	for (unsigned int i = 0; i < Files.size(); ++i)
	{
		vector<string> words = ReadFile(Files.at(i));
		Replace(words, keywords);
		Write(words, Files.at(i));
		addWords(allWords, words);
	}
}
