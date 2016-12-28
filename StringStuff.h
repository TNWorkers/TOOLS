#ifndef MYSTRINGSTUFF
#define MYSTRINGSTUFF

#include <iostream>
#include <iomanip>
#include <vector>

void draw_progressBar (int len, double percent)
{
	std::cout << "\x1B[2K"; // erase the entire current line
	std::cout << "\x1B[0E"; // move to the beginning of the current line
	std::string progress;
	for (int i=0; i<len; ++i)
	{
		if (i < static_cast<int>(len*percent)) {progress += "=";}
		else                                   {progress += " ";}
	}
	std::cout << "[" << progress << "] " << (static_cast<int>(100*percent)) << "%";
	flush(std::cout);
}

std::string make_string()
{
	return "";
}

template<typename FirstType, typename ... Types>
std::string make_string (FirstType first, Types ... rest)
{
	std::ostringstream out;
	out << first << make_string(rest...);
	return out.str();
}

void make_foldername (std::string &fname)
{
	if (fname.back() != '/') {fname += "/";}
	if      (fname.at(0) != '.' and fname.at(1) != '/') {fname = "./"+fname;}
	else if (fname.at(0) != '.' and fname.at(1) == '/') {fname = "."+fname;}
}

template<typename T>
std::string str (std::vector<T> V)
{
	std::ostringstream out;
	for (int i=0; i<V.size()-1; ++i)
	{
		out << V[i] << ",";
	}
	out << V[V.size()-1];
	return out.str();
}

template<typename Scalar, typename IntType>
std::string pad_zeros (Scalar x, IntType N_zeros)
{
	std::ostringstream out;
	out << std::setfill('0') << setw(N_zeros) << x;
	return out.str();
}

template<typename IntType>
std::string round (double x, IntType N_decimals)
{
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(N_decimals) << x; //+0.5/pow(10,N_decimals); // +0.5/pow(10,N_decimals) to round up
	std::string s = ss.str();
	if (N_decimals > 0 && s[s.find_last_not_of('0')] == '.')
	{
		s.erase(s.size()-N_decimals+1);
	}
	return s;
}

#endif
