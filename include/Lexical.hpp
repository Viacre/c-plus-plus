#pragma once

#include "std_lib_facilities.h"

namespace cxx
{

// Token kind
constexpr char number = 'N';
constexpr char exit = 'Q';
constexpr char help = 'H';
constexpr char print = ';';
constexpr char definition = 'D';
constexpr char assignment = 'A';
constexpr char variable = 'V';
// Keyword
constexpr string_view define_key = "define";
constexpr string_view assign_key = "set";
constexpr string_view exit_key = "exit";
constexpr string_view help_key = "help";

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

class Environment
{
public:
	using BindingMap = unordered_map<string, double>;

	Environment() = default;
	
	bool
	IsDefined(string_view id) const;
	double
	Lookup(string_view id) const;

	
	void
	SetValue(string_view id, double val);
	void
	Define(string_view id, double val);

private:
	BindingMap bindings;
};

}
