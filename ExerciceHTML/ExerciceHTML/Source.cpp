#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <algorithm>
#include <sstream>
#include <chrono>
using namespace std;
using namespace std::chrono;

vector<string> ReadFile(const string& filename);
void setArguments(const int& argc, char* argv[], vector<string> &SourceFiles, vector<string> &Options);
void Write(const vector<string> &words, const string &filename);
void Replace(vector<string> &words, const vector<string> &keywords, bool &color);
void addWords(vector<string> &allWords, const  vector<string> & words);
void createStatsFile(const vector<string> &words);

int main(int argc, char* argv[])
{
	vector<string> Files;
	vector<string> Options;
	vector<string> allWords;
	setArguments(argc, argv, Files, Options);

	bool color = false;
	bool stats = false;
	for (unsigned int i = 0; i < Options.size(); ++i)
	{
		if (Options[i] == "couleur")
			color = true;
		else if (Options[i] == "stats")
			stats = true;
	}

	// s'il n'y a pas de fichier en paramètre, prendre ce fichier-ci
	if (!Files.empty())
		Files.push_back("Source.cpp");

	vector<string> keywords = ReadFile("keywords.txt");

	for (unsigned int i = 0; i < Files.size(); ++i)
	{
		vector<string> words = ReadFile(Files.at(i));
		Replace(words,keywords, color);
		Write(words, Files.at(i));
		addWords(allWords, words);
	}
	
	if (stats)
		createStatsFile(allWords);
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

void Replace(vector<string> &words, const vector<string> &keywords, bool &color)
{
	for(auto debut = begin(words); debut != end(words); ++debut)
	{
		
		*debut = regex_replace(*debut, regex{ "&" }, "&amp;");
		*debut = regex_replace(*debut, regex{ "<" }, "&lt;");
		*debut = regex_replace(*debut, regex{ ">" }, "&gt;");
		*debut = regex_replace(*debut, regex{ "\\t" }, "&nbsp&nbsp&nbsp&nbsp;");
		if (color)
		{
			for (auto keywordPos = begin(keywords); keywordPos != end(keywords); ++keywordPos)
			{
				regex expression("\\b" + *keywordPos + "\\b");
				*debut = regex_replace(*debut, expression, " <span style='color:blue'> " + *keywordPos + "</span>");
			}
		}
	}

}

void addWords(vector<string> &allWords,const  vector<string> & words)
{
	allWords.insert(end(allWords), begin(words), end(words));
}

void createStatsFile(const vector<string> &words)
{
	regex expression("\\d+\\.?\\d*|\\w+");
	map<string, int> stats;
	smatch match;
	for (unsigned int i = 0; i < words.size(); ++i)
	{
		string sentence = words[i];
		while (regex_search(sentence, match, expression))
		{
			for (auto& mot : match)
				stats[mot.str()]++;
			sentence = match.suffix().str();
		}
	}

	ofstream fichier("Stats.txt");
	for (auto s = stats.begin(); s != stats.end(); s++)
		fichier << s->first << " : " << s->second << endl;
}
