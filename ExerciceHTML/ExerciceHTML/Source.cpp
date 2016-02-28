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
void Write(vector<string> words, string filename);
void Replace(vector<string> &words, bool color);
void addWords(vector<string> &allWords, vector<string> words);
void createStatsFile(vector<string> words);
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		if (!item.empty()) {
			elems.push_back(item);
		}
	}
	return elems;
}

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

	for (unsigned int i = 0; i < Files.size(); ++i)
	{
		vector<string> words = ReadFile(Files.at(i));
		Replace(words, color);
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
		else if (param.find(".cpp") != std::string::npos)
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

void Write(vector<string> words, string filename)
{
	ofstream file(filename + ".html");
	for (unsigned int i = 0; i < words.size(); ++i)
		file << words.at(i) << "<br>" << flush;
}

void Replace(vector<string> &words, bool color)
{
	vector<string> keywords = vector<string>{	"alignas", "alignof", "and", "and_eq", "asm", "auto",
		"bitand", "bitor", "bool", "break", "case", "catch",
		"char", "char16_t", "char32_t", "class", "compl",
		"concept", "const", "constexpr", "const_cast",
		"continue", "decltype", "default", "delete", "do",
		"double", "dynamic_cast", "else", "enum", "explicit",   
		"export", "extern", "false", "float", "for", "friend",
		"goto", "if", "inline", "int", "long", "mutable",
		"namespace", "new", "noexcept", "not", "not_eq",
		"nullptr", "operator", "or", "or_eq", "private",
		"protected", "public", "register", "reinterpret_cast",
		"requires", "return", "short", "signed", "sizeof",
		"static", "static_assert", "static_cast", "struct",
		"switch", "template", "this", "thread_local", "throw",
		"true", "try", "typedef", "typeid", "typename", "union",
		"unsigned", "using", "virtual", "void", "volatile",
		"wchar_t", "while", "xor", "xor_eq"};
	
	
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

void addWords(vector<string> &allWords, vector<string> words)
{
	allWords.insert(end(allWords), begin(words), end(words));
}

void createStatsFile(vector<string> words)
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