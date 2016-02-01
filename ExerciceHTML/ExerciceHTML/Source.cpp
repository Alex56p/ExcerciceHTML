#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <algorithm>
#include <sstream>
using namespace std;

string AddSpanToKeywords(vector<string>& words)
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
	for (int i = 0; i < words.size(); ++i)
	{
		bool trouve = false;
		for (int j = 0; j < keywords.size(); j++)
		{
			//METTRE UN FIND ICI
			if (keywords.at(j) == words[i] && !trouve)
			{
				trouve = true;
				words[i] = "<span style='color:blue'>" + words[i] + "</span>";
			}
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

void RemplacerTout(string &content)
{
	for (int i = 0; i != content.size(); ++i)
	{
		if (content[i] == '<')
		{
			content.replace(i, 1, " &lt ");
			i += 4;
		}
		else if (content[i] == '>')
		{
			content.replace(i, 1, " &gt ");
			i += 4;
		}
		else if (content[i] == '&')
		{
			content.replace(i, 1, " &amp ");
			i += 4;
		}
		else if (content[i] == '\n')
		{
			content.replace(i, 1, " </br> ");
			i += 6;
		}
		else if (content[i] == '\t')
		{
			content.replace(i, 1, " &nbsp&nbsp&nbsp&nbsp ");
			i += 21;
		}
	}
	
}


int main()
{
	string content;
	//string pattern("[a-zA-Z_]([a-zA-Z0-9_])*");
	//regex expression(pattern);

	ifstream infile("Source.cpp");
	while (!infile.eof())
	{
		char c = infile.get();
		content += c;
	}

	RemplacerTout(content);

	istringstream iss(content);

	vector<string> words;
	copy(istream_iterator<string>(iss),
		istream_iterator<string>(),
		back_inserter(words));

	AddSpanToKeywords(words);

	content = "";

	for (int i = 0; i < words.size(); i++)
	{
		content += words[i] + " ";
	}

	ofstream outfile("myCode.html");
	if (outfile)
	{
		outfile << content;

	}
}
