#ifndef Lexical_hpp_
#define Lexical_hpp_ 1

#include "std_lib_facilities.h"

namespace cxx
{

enum class TokenCategory
{
	// Variable, Keyword
	Identifier,
	Number,
	Punctuator
};

TokenCategory
CategorizeToken(char id);

const char number = '8';
const char quit = 'q';
const char print = ';';
const char let = 'L';
const char name = 'a';
const string declkey = "let";
const string assignment = "set!";
const string prompt = "> ";
const string result = "= ";

struct Token
{
	using symbol_type = string;
	using number_type = double;
	using value_type = variant<symbol_type, number_type>;

	char kind; // what kind of token
	double value; // for numbers: a value
	string name;

	// Token() = delete;
	explicit Token(char ch) : kind(ch), value(0) // Punctuator
	{}
	Token(char ch, double val) : kind(ch), value(val) // Number
	{}
	Token(char ch, string n) : kind(ch), name(n) // Identifier
	{}
};

class Token_stream
{
public:
	Token_stream(); // make a Token_stream that reads from cin

	Token
	get(); // get a Token (get() is defined elsewhere)
	void
	putback(Token t); // put a Token back
	void
	ignore(char c);

private:
	bool full; // is there a Token in the buffer?
	Token buffer; // here is where we keep a Token put back using putback()
};

struct Variable
{
	string name;
	double value;
	bool is_const;
	Variable(string n, double v) : name(n), value(v)
	{}
};

class Symbol_table
{
public:
	double
	get(string s);
	void
	set(string s, double d);
	bool
	is_declare(string var);
	double
	declare(string var, double val);

private:
	vector<Variable> var_table;
};

}

#endif