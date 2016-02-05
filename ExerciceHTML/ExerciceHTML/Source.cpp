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

vector<string> ReadFile(string filename);
void setArguments(int argc, char* argv[], vector<string> &SourceFiles, vector<string> &Options);
void Write(vector<string> words, string filename);
void Replace(vector<string> &words, bool color);
void addWords(vector<string> &allWords, vector<string> words);
void SetStats(vector<string> words);

int main(int argc, char* argv[])
{
	vector<string> Files;
	vector<string> Options;
	vector<string> allWords;
	setArguments(argc, argv, Files, Options);

	bool color = false;
	bool stats = false;
	for (int i = 0; i < Options.size(); ++i)
	{
		if (Options.at(i) == "couleur")
			color = true;
		else if (Options.at(i) == "stats")
			stats = true;
	}

	if (Files.size() <= 0)
		Files.push_back("Source.cpp");

	for (int i = 0; i < Files.size(); ++i)
	{
		vector<string> words = ReadFile(Files.at(i));
		Replace(words, color);
		Write(words, Files.at(i));
		addWords(allWords, words);
	}
	
	if (stats)
		SetStats(allWords);
}

void setArguments(int argc,char* argv[], vector<string> &SourceFiles, vector<string> &Options)
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

vector<string> ReadFile(string filename)
{
	vector<string> sentences;
	ifstream file(filename);
	if (file.is_open())
	{
		for (string line; getline(file, line);)
			sentences.push_back(line);
	}
	file.close();
	return sentences;
}

void Write(vector<string> words, string filename)
{
	ofstream file(filename + ".html");
	for (int i = 0; i < words.size(); ++i)
		file << words.at(i) << "<br>" << flush;
	file.close();
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
	
	regex expression("");
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
				expression = "\\b" + *keywordPos + "\\b";
				*debut = regex_replace(*debut, expression, " <span style='color:blue'> " + *keywordPos + "</span>");
			}
		}
	}

}

void addWords(vector<string> &allWords, vector<string> words)
{
	for (int i = 0; i < words.size(); ++i)
		allWords.push_back(words.at(i));
}

void SetStats(vector<string> words)
{
	regex expression("[a-zA-Z_]([a-zA-Z0-9_])*");
	map<string, int> mots;
	string file = "";
	for (auto s = words.begin(); s != words.end(); ++s)
		file += *s;

	istringstream iss(file);
	vector<string> tokens;
	copy(istream_iterator<string>(iss),
		istream_iterator<string>(),
		back_inserter(tokens));

	for (int i = 0; i < tokens.size(); ++i)
	{
		if (regex_match(tokens.at(i), expression))
			mots[tokens.at(i)]++;
	}

	ofstream stats("Stats.txt");
	for (auto s = mots.begin(); s != mots.end(); s++)
		stats << s->first << " : " << s->second << endl;
	stats.close();
}