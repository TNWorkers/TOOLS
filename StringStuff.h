#ifndef MYSTRINGSTUFF
#define MYSTRINGSTUFF

#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

void draw_progressBar (int len, double percent)
{
	cout << "\x1B[2K"; // erase the entire current line
	cout << "\x1B[0E"; // move to the beginning of the current line
	string progress;
	for (int i=0; i<len; ++i)
	{
		if (i < static_cast<int>(len*percent)) {progress += "=";}
		else                                   {progress += " ";}
	}
	cout << "[" << progress << "] " << (static_cast<int>(100*percent)) << "%";
	flush(cout);
}

string make_string()
{
	return "";
}

template<typename FirstType, typename ... Types>
string make_string (FirstType first, Types ... rest)
{
	ostringstream out;
	out << first << make_string(rest...);
	return out.str();
}

void make_foldername (string &fname)
{
	if (fname.back() != '/') {fname += "/";}
	if      (fname.at(0) != '.' and fname.at(1) != '/') {fname = "./"+fname;}
	else if (fname.at(0) != '.' and fname.at(1) == '/') {fname = "."+fname;}
}

template<typename T>
string str (vector<T> V)
{
	ostringstream out;
	for (int i=0; i<V.size()-1; ++i)
	{
		out << V[i] << ",";
	}
	out << V[V.size()-1];
	return out.str();
}

template<typename Scalar, typename IntType>
string pad_zeros (Scalar x, IntType N_zeros)
{
	ostringstream out;
	out << setfill('0') << setw(N_zeros) << x;
	return out.str();
}

template<typename IntType>
string round (double x, IntType N_decimals)
{
	ostringstream ss;
	ss << fixed << setprecision(N_decimals) << x; //+0.5/pow(10,N_decimals); // +0.5/pow(10,N_decimals) to round up
	string s = ss.str();
	if (N_decimals > 0 && s[s.find_last_not_of('0')] == '.')
	{
		s.erase(s.size()-N_decimals+1);
	}
	return s;
}

#endif
