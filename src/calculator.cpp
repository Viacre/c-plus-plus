#include "Lexical.hpp"

using cxx::Token;
using cxx::Token_stream;
using cxx::Symbol_table;

Token_stream ts;
Symbol_table st;

double
expression();
double
primary();
double
term();
double
assignment();
double
definition();
double
statement();
void
calculate();
void
clean_up_mess();

// deal with numbers and parentheses
double
primary()
{
	Token t = ts.get();
	switch(t.kind)
	{
	case '(': // handle '(' expression ')'
	{
		double d = expression();
		t = ts.get();
		if(t.kind != ')')
			error("')' expected");
		return d;
	}
	case cxx::number:
		return t.value; // return the number's value
	case '-':
		return -primary();
	case '+':
		return primary();
	case cxx::variable:
		return st.get(t.name);
	default:
		error("primary expected");
	}
}

// deal with *, /, and %
double
term()
{
	double left = primary();
	Token t = ts.get(); // get the next token from token stream

	while(true)
	{
		switch(t.kind)
		{
		case '*':
			left *= primary();
			t = ts.get();
			break;
		case '/': {
			double d = primary();
			if(d == 0)
				error("divide by zero");
			left /= d;
			t = ts.get();
			break;
		}
		case '%': {
			double d = primary();
			if(d == 0)
				error("%: divide by zero");
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
expression()
{
	double left = term(); // read and evaluate a Term
	Token t = ts.get(); // get the next token from token stream

	while(true)
	{
		switch(t.kind)
		{
		case '+':
			left += term(); // evaluate Term and add
			t = ts.get();
			break;
		case '-':
			left -= term(); // evaluate Term and subtract
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
definition()
{
	Token t = ts.get();
	if(t.kind != cxx::variable)
		error("name expected in definition");
	string var_name = t.name;

	double d = expression();
	st.define(var_name, d);
	return d;
}

double
assignment()
{
	Token t = ts.get();
	if(t.kind != cxx::variable)
		error("name expected in assignment");
	string var_name = t.name;

	double d = expression();

	st.set(var_name, d);
	return d;
}

double
statement()
{
	Token t = ts.get();
	switch(t.kind)
	{
	case cxx::definition:
		return definition();
	case cxx::assignment:
		return assignment();
	default:
		ts.putback(t);
		return expression();
	}
}

void
clean_up_mess()
{
	ts.ignore(cxx::print);
}

//------------------------------------------------------------------------------

void
calculate() // expression evaluation loop
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
			cout << result << statement() << '\n';
		}
		catch(exception& e)
		{
			cerr << e.what() << "\n";
			clean_up_mess();
		}
}

int
main()
try
{
	cout << "Welcome to our simple calculator.\n"
		 << "Please enter expressions using floating-point numbers.\n";

	st.define("pi", 3.1415926535);
	st.define("e", 2.7182818284);

	calculate();

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