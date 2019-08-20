#ifndef MYSTRINGSTUFF
#define MYSTRINGSTUFF

#include <iostream>
#include <iomanip>
#include <vector>
#include <clocale>
#include <cstdlib>
#include <iomanip>

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
	std::flush(std::cout);
}

std::string make_string()
{
	return "";
}

template<typename FirstType, typename... Types>
std::string make_string (FirstType first, Types ... rest)
{
	std::ostringstream out;
	out << first << make_string(rest...);
	return out.str();
}

void correct_foldername (std::string &fname)
{
	if (fname.back() != '/') {fname += "/";}
	if (fname.at(0) != '/' and fname.at(0) != '.') {fname = "./"+fname;}
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
	out << std::setfill('0') << std::setw(N_zeros) << x;
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

std::string remove_color(const std::string &s)
{
	std::string tmp;
	std::string copy=s;
	std::size_t pos_beg = 0ul;
	while (pos_beg != std::string::npos)
	{
		tmp=copy;
		pos_beg = tmp.find("\033[");
		if (pos_beg != std::string::npos)
		{
			std::size_t pos_end = tmp.find("m",pos_beg);
			if (pos_end != std::string::npos) {tmp.erase(pos_beg,pos_beg+pos_end+1);}
			else {throw std::runtime_error("remove_color(): error while determine the string length of colored strings.");}
			copy=tmp;
		}
	}
	return copy;
}

/*
* Erase all Occurrences of given substring from main string.
*/
void eraseAllSubStr (std::string &mainStr, const std::string &toErase)
{
	size_t pos = std::string::npos;
	// Search for the substring in string in a loop untill nothing is found
	while ((pos  = mainStr.find(toErase) )!= std::string::npos)
	{
		// If found then erase it from string
		mainStr.erase(pos, toErase.length());
	}
}

std::string remove_termcolor (const std::string &mainStr)
{
	std::string out = mainStr;
	eraseAllSubStr(out, "[1m");
	eraseAllSubStr(out, "[00m");
	eraseAllSubStr(out, "[4m");
	return out;
}

std::size_t strlen_mb (const std::string& s)
{
	std::string s_without_color = remove_color(s);
	std::setlocale(LC_ALL, "en_US.UTF-8");
    std::size_t result = 0;
    const char* ptr = s_without_color.data();
    const char* end = ptr + s_without_color.size();
    std::mblen(NULL, 0); // reset the conversion state
    while (ptr < end) {
        int next = std::mblen(ptr, end-ptr);
        if (next == -1) {
            throw std::runtime_error("strlen_mb(): conversion error");
        }
        ptr += next;
        ++result;
    }
    return result;
}
#endif
