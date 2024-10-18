#include "Lexical.hpp"

namespace cxx
{

// The constructor just sets full to indicate that the buffer is empty:
Token_stream::Token_stream() : full(false), buffer(0)
{}

// The putback() member function puts its argument back into the Token_stream's buffer:
void
Token_stream::putback(Token t)
{
	if(full)
		error("token_stream::putback() into a full buffer");
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
	cin >> ch;

	switch(ch)
	{
	case print:
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case '=':
		return Token{ch}; // let each character represent itself
	case '.':
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
			while(cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '!'))
				s += ch;
			cin.putback(ch);

			if(s == define_key)
				return Token{definition};
			if(s == assign_key)
				return Token{assignment};
			if(s == exit_key)
				return Token{exit};

			return Token{variable, s};
		}
		error("token_stream::get: Bad token");
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
	error("set: undefined variable ", s);
}

bool
Symbol_table::is_defined(string var)
{
	for(const auto& v: var_table)
		if(v.name == var)
			return true;
	return false;
}

double
Symbol_table::define(string var, double val)
{
	if(is_defined(var))
		error(var, " defined twice");
	var_table.push_back(Variable{var, val});
	return val;
}


}