#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <algorithm>
#include <sstream>
using namespace std;

void AddSpanToKeywords(vector<string>& words)
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
	
	for (int i = 0; i < words.size(); ++i)
	{
		bool trouve = false;
		for (int j = 0; j < keywords.size(); j++)
		{
			int pos = words[i].find(keywords.at(j));
			if (pos != -1 && !trouve)
			{
				trouve = true;
				words[i].replace(pos, keywords[j].length(), "<span style='color:blue'>" + words[i].substr(pos,keywords[j].length()) + "</span>");
			}
		}
	}
}

void RemplacerTout(string &content)
{
	map<char, string> symbols;
	symbols['<'] = " &lt ";
	symbols['>'] = " &gt ";
	symbols['&'] = " &amp ";
	symbols['\n'] = " </br> ";
	symbols['\t'] = " &nbsp&nbsp&nbsp&nbsp ";
	for (auto i = begin(content); i != end(content); ++i)
	{
		if (symbols.find(*i) != end(symbols))
		{
			content = content.substr(i, 0);
		}
	}
}


int main(int argc, char* argv[])
{
	string content;
	string pattern("^[a-zA-Z]([a-zA-Z0-9_])*[\\D]");
	regex expression(pattern);
	map<string, int> mots;
	ifstream infile("Source.cpp");
	while (!infile.eof())
		content += infile.get();

	RemplacerTout(content);

	istringstream iss(content);

	vector<string> words;
	copy(istream_iterator<string>(iss),
		istream_iterator<string>(),
		back_inserter(words));

	AddSpanToKeywords(words);

	ofstream outfile("Source.cpp.html");
	for (auto itt = begin(words); itt != end(words); ++itt)
		outfile << *itt << ' ';
	outfile.close();

	infile.close();
	infile.open("Source.cpp.html");
	for (string s; infile >> s;)
	{
		if (regex_match(s, expression))
			mots[s]++;
	}
	infile.close();
	ofstream stats("Stats.txt");
	for (auto s = mots.begin(); s != mots.end(); s++)
		stats << s->first << " : " << s->second << endl;
	stats.close();
	
}
