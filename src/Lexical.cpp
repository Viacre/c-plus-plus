#include "Lexical.hpp"

namespace cxx
{

TokenCategory
CategorizeToken(char id)
{
	if(isdigit(id) || id == '.')
		return TokenCategory::Number;
	if(ispunct(id))
		return TokenCategory::Punctuator;
	return TokenCategory::Identifier;
}

// The constructor just sets full to indicate that the buffer is empty:
Token_stream::Token_stream() : full(false), buffer(0)
{}

// The putback() member function puts its argument back into the Token_stream's buffer:
void
Token_stream::putback(Token t)
{
	if(full)
		error("putback() into a full buffer");
	buffer = t; // copy t to buffer
	full = true; // buffer is now full
}


Token
Token_stream::get()
{
	if(full)
	{ // do we already have a Token ready?
		// remove token from buffer
		full = false;
		return buffer;
	}

	char ch;
	cin >> ch; // note that >> skips whitespace (space, newline, tab, etc.)

	switch(ch)
	{
	case print: // deal with Symbol
	case quit:
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case '=':
		return Token{ch}; // let each character represent itself
	case '.': // deal with Number
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9': {
		cin.putback(ch); // put digit back into the input stream
		double val;
		cin >> val; // read a floating-point number
		return Token{number, val};
	}
	default: // deal with Keyword and Variable
		if(isalpha(ch))
		{
			string s;
			s += ch;
			while(cin.get(ch) && (isalpha(ch) || isdigit(ch)))
				s += ch;
			cin.putback(ch);
			if(s == declkey)
				return Token{let};
			return Token{name, s};
		}
		error("token_istream::get: Bad token");
	}
}

void
Token_stream::ignore(char c)
{ // c represents the kind of Token
	// first look in buffer:
	if(full && c == buffer.kind)
	{
		full = false;
		return;
	}
	full = false;
	// now search input:
	char ch = 0;
	while(cin >> ch)
		if(ch == c)
			return;
}

double
Symbol_table::get(string s)
{
	for(const auto& v: var_table)
		if(v.name == s)
			return v.value;
	error("get: undefined name ", s);
}

void
Symbol_table::set(string s, double d)
{ // set the Variable named s to d
	for(auto& v: var_table)
		if(v.name == s)
		{
			v.value = d;
			return;
		}
	error("set_value: undefined variable ", s);
}

bool
Symbol_table::is_declare(string var)
{
	for(const auto& v: var_table)
		if(v.name == var)
			return true;
	return false;
}

double
Symbol_table::declare(string var, double val)
{
	if(is_declare(var))
		error(var, " declared twice");
	var_table.push_back(Variable{var, val});
	return val;
}


}
