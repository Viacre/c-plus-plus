#include "Lexical.hpp"

using cxx::Token;
using cxx::Token_stream;
using cxx::Symbol_table;

Symbol_table st;

double
expression(Token_stream&);

double
primary(Token_stream&);

double
term(Token_stream&);

double
assignment(Token_stream&);

double
definition(Token_stream&);

double
statement(Token_stream&);

void
calculate(Token_stream&);

void
clean_up_mess(Token_stream&);

// deal with numbers and parentheses
double
primary(Token_stream& ts)
{
	Token t = ts.get();
	switch(t.kind)
	{
	case '(': // handle '(' expression ')'
	{
		double d = expression(ts);
		t = ts.get();
		if(t.kind != ')')
			throw runtime_error("')' expected");
		return d;
	}
	case cxx::number:
		return t.value; // return the number's value
	case '-':
		return -primary(ts);
	case '+':
		return primary(ts);
	case cxx::variable:
		return st.get(t.name);
	default:
		throw runtime_error("primary expected");
	}
}

// deal with *, /, and %
double
term(Token_stream& ts)
{
	double left = primary(ts);
	Token t = ts.get(); // get the next token from token stream

	while(true)
	{
		switch(t.kind)
		{
		case '*':
			left *= primary(ts);
			t = ts.get();
			break;
		case '/': {
			double d = primary(ts);
			if(d == 0)
				throw runtime_error("divide by zero");
			left /= d;
			t = ts.get();
			break;
		}
		case '%': {
			double d = primary(ts);
			if(d == 0)
				throw runtime_error("%: divide by zero");
			left = fmod(left, d);
			t = ts.get();
			break;
		}
		default:
			ts.putback(t); // put t back into the token stream
			return left;
		}
	}
}

// deal with + and -
double
expression(Token_stream& ts)
{
	double left = term(ts); // read and evaluate a Term
	Token t = ts.get(); // get the next token from token stream

	while(true)
	{
		switch(t.kind)
		{
		case '+':
			left += term(ts); // evaluate Term and add
			t = ts.get();
			break;
		case '-':
			left -= term(ts); // evaluate Term and subtract
			t = ts.get();
			break;
		default:
			ts.putback(t); // put t back into the token stream
			return left; // finally: no more + or -: return the answer
		}
	}
}

// define a variable called "name” with the initial value "expression”
double
definition(Token_stream& ts)
{
	Token t = ts.get();
	if(t.kind != cxx::variable)
		throw runtime_error("name expected in definition");
	string var_name = t.name;

	double d = expression(ts);
	st.define(var_name, d);
	return d;
}

double
assignment(Token_stream& ts)
{
	Token t = ts.get();
	if(t.kind != cxx::variable)
		throw runtime_error("name expected in assignment");
	string var_name = t.name;

	double d = expression(ts);

	st.set(var_name, d);
	return d;
}

double
statement(Token_stream& ts)
{
	Token t = ts.get();
	switch(t.kind)
	{
	case cxx::definition:
		return definition(ts);
	case cxx::assignment:
		return assignment(ts);
	default:
		ts.putback(t);
		return expression(ts);
	}
}

void
clean_up_mess(Token_stream& ts)
{
	ts.ignore(cxx::print);
}

//------------------------------------------------------------------------------

void
calculate(Token_stream& ts) // expression evaluation loop
{
	const string prompt = "> ";
	const string result = "= ";

	while(cin)
		try
		{
			cout << prompt;
			Token t = ts.get();
			while(t.kind == cxx::print)
				t = ts.get(); // first discard all “prints”
			if(t.kind == cxx::exit)
				return;

			ts.putback(t);
			cout << result << statement(ts) << '\n';
		}
		catch(exception& e)
		{
			cerr << e.what() << "\n";
			clean_up_mess(ts);
		}
}

int
main()
try
{
	cout << "Welcome to our simple calculator.\n"
		 << "Please enter expressions using floating-point numbers.\n";

	Token_stream ts;
	st.define("pi", 3.1415926535);
	st.define("e", 2.7182818284);

	calculate(ts);

	keep_window_open();
	return 0;
}
catch(exception& e)
{
	cerr << "error: " << e.what() << '\n';
	keep_window_open("~~");
	return 1;
}
catch(...)
{
	cerr << "Oops: unknown exception!\n";
	keep_window_open("~~");
	return 2;
}

//------------------------------------------------------------------------------