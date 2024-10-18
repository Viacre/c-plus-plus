#pragma once

#include "std_lib_facilities.h"

namespace cxx
{

// Token kind
const char number = 'N';
const char exit = 'Q';
const char help = 'H';
const char print = ';';
const char definition = 'D';
const char assignment = 'A';
const char variable = 'V';
// Keyword
const string define_key = "define";
const string assign_key = "set!";
const string exit_key = "exit";
const string help_key = "help";

struct Token
{
	char kind; // what kind of token
	double value; // for numbers: a value
	string name;

	Token() = delete;
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
	is_defined(string var);
	double
	define(string var, double val);

private:
	vector<Variable> var_table;
};

}
