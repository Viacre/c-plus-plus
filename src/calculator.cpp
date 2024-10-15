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
declaration();
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
	case cxx::name:
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

// declare a variable called "name” with the initial value "expression”
double
declaration()
{
	Token t = ts.get();
	if(t.kind != cxx::name)
		error("name expected in declaration");
	string var_name = t.name;
	Token t2 = ts.get();
	if(t2.kind != '=')
		error("= missing in declaration of ", var_name);
	double d = expression();
	st.declare(var_name, d);
	return d;
}

double
statement()
{
	Token t = ts.get();
	switch(t.kind)
	{
	case cxx::let:
		return declaration();
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
	while(cin)
		try
		{
			cout << cxx::prompt;
			Token t = ts.get();
			while(t.kind == cxx::print)
				t = ts.get(); // first discard all “prints”
			if(t.kind == cxx::quit)
				return;

			ts.putback(t);
			cout << cxx::result << statement() << '\n';
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

	st.declare("pi", 3.1415926535);
	st.declare("e", 2.7182818284);

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