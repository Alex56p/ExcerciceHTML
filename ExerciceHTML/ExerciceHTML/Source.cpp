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

int main(int argc, char* argv[])
{
	regex expression("");

	map<string, int> mots;
	ifstream infile("Source.cpp");

	vector<string> words; 

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (string line; getline(infile, line);)
		words.push_back(line);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(t2 - t1).count();
	cout << "Créer le vecteur: " << duration << endl;

	infile.close();

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

	ofstream outfile("Source.cpp.html");

	t1 = high_resolution_clock::now();
	for (auto debut = begin(words); debut != end(words); ++debut)
	{
		*debut = regex_replace(*debut, regex{ "&" }, "&amp;");
		*debut = regex_replace(*debut, regex{ "<" }, "&lt;");
		*debut = regex_replace(*debut, regex{ ">" }, "&gt;");

		for (auto keywordPos = begin(keywords); keywordPos != end(keywords); ++keywordPos)
		{
			expression = "\\b" + *keywordPos + "\\b";

			*debut = regex_replace(*debut, expression, "<span style='color:blue'>" + *keywordPos + "</span>");
		}
		outfile << *debut << "<br>" << flush;
	}

	t2 = high_resolution_clock::now();

	duration = duration_cast<microseconds>(t2 - t1).count();
	cout << "Colorer: " << duration << endl;

	expression = "\\b[a-zA-Z_]([a-zA-Z_])*\\b";

	t1 = high_resolution_clock::now();
	for (string s; infile >> s;)
	{
		if (regex_match(s, expression))
			mots[s]++;
	}
	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>(t2 - t1).count();
	cout << "Stats: " << duration << endl;
	infile.close();
	ofstream stats("Stats.txt");
	for (auto s = mots.begin(); s != mots.end(); s++)
		stats << s->first << " : " << s->second << endl;
	stats.close();
	outfile.close();
}
