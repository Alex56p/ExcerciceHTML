#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <algorithm>
using namespace std;

string AddSpanToKeywords(map<int, string> content)
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
	
	string retour = "";
	for (int i = 0; i < content.size(); ++i)
	{
		bool trouve = false;
		for (int j = 0; j < keywords.size(); j++)
		{
			if (keywords.at(j) == content[i] && !trouve)
			{
				trouve = true;
				retour += "<span style='color:blue'>" + content[i] + "</span>";
			}

		}
		if (!trouve)
		{
			retour += content[i];
		}
	}
	return retour;
}

//void AddSpanToKeywords(string& content, string keyword)
//{
//	auto word = content.find(keyword);
//	string oldContent = content;
//	if (word != std::string::npos)
//	{
//		content = oldContent.substr(0, word);
//		content += "<span style='color:blue'>" + keyword + "</span>";
//		content += oldContent.substr(word + keyword.length(), oldContent.length());
//	}
//}

void AddWhiteSpaces(string& content)
{
	auto word = content.find('\t');
	string oldContent = content;
	if (word != std::string::npos)
	{
		content = oldContent.substr(0, word);
		content += "&nbsp&nbsp&nbsp&nbsp";
		content += oldContent.substr(word + 1, oldContent.length());
	}
}

void RemplacerTout(string &content)
{
	for (int i = 0; i != content.size(); ++i)
	{
		if (content[i] == '<')
		{
			content.replace(i, 1, "&lt");
			i += 2;
		}
		else if (content[i] == '>')
		{
			content.replace(i, 1, "&gt");
			i += 2;
		}
		else if (content[i] == '&')
		{
			content.replace(i, 1, "&amp");
			i += 2;
		}
		else if (content[i] == '\n')
		{
			content.replace(i, 1, "</br>");
			i += 4;
		}
		else if (content[i] == '\t')
		{
			content.replace(i, 1, "&nbsp&nbsp&nbsp&nbsp");
			i += 19;
		}
	}
	
}


int main()
{
	map<string, int> words;
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
	ifstream infile;
	ofstream outfile;
	string content;
	string pattern("[a-zA-Z_]([a-zA-Z0-9_])*");
	regex expression(pattern);

	map<int, string> mots;

	int i = 0;
	infile.open("Source.cpp");
	for (string s; infile >> s;)
	{
		mots[i] = s;
		++i;
	}

	/*while (!infile.eof())
	{
		char c = infile.get();
		content += c;
	}*/

	RemplacerTout(content);
	content = AddSpanToKeywords(mots);
	AddWhiteSpaces(content);


	outfile.open("myCode.html");
	outfile << content;
}
