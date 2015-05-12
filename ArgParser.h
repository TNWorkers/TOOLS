#ifndef ARGPARSER
#define ARGPARSER

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <map>
#include <vector>
#include <iterator>
using namespace std;

class BadConversion : public runtime_error
{
public:

	BadConversion (const string &s) : runtime_error(s) {}
};

template<typename NumType>
inline NumType convert_to_num (const string &s)
{
	istringstream ss(s);
	NumType x;
	if (!(ss>>x)) {throw BadConversion("convert_to_numeric(\""+s+"\")");}
	return x;
}

void find_and_replace (string& source, string const& find, string const& replace)
{
	for(std::string::size_type i = 0; (i = source.find(find, i)) != std::string::npos;)
	{
		source.replace(i, find.length(), replace);
		i += replace.length() - find.length() + 1;
	}
}

class ArgParser
{
public:
	
	ArgParser (int argc_input, char** argv_input);
	
	string operator [] (string keyword) {return dictionary[keyword];}; // brackets [] = string
	int    operator () (string keyword) {return get<int>(keyword);}; // parentheses () = int
	
	template<typename Scalar> Scalar get (string keyword, Scalar default_val=Scalar());
	template<typename Scalar> vector<Scalar> get_list (string keyword);
	
	bool test (string key) const;
	
	string info() const;
	string str() const;

private:

	map<string,string> dictionary;
};

ArgParser::
ArgParser (int argc_input, char** argv_input)
{
	vector<string> args(argv_input+1, argv_input+argc_input);
	
	for (int i=0; i<args.size(); ++i)
	{
		if (args[i].substr(0,1) == "-")
		{
			args[i].erase(0,1);
			if (i!=args.size()-1) // in the middle of arglist
			{
				if (args[i+1].substr(0,1) != "-")
				{
					dictionary.insert(pair<string,string>(args[i],args[i+1])); // store -arg val
				}
				else
				{
					dictionary.insert(pair<string,string>(args[i],"1")); // store -arg 1 (true)
				}
			}
			else // at end of arglist
			{
				dictionary.insert(pair<string,string>(args[i],"1")); // store -arg 1 (true)
			}
		}
	}
}

string ArgParser::
info() const
{
	stringstream ss;
	ss << "ArgParser: ";
	for (auto it=dictionary.begin(); it!=dictionary.end(); ++it)
	{
		ss << it->first << "=" << it->second;
		if (it!=--dictionary.end()) {ss << ", ";}
	}
	return ss.str();
}

string ArgParser::
str() const
{
	stringstream ss;
	for (auto it=dictionary.begin(); it!=dictionary.end(); ++it)
	{
		ss << it->first << "=" << it->second;
		if (it!=--dictionary.end()) {ss << "_";}
	}
	return ss.str();
}

bool ArgParser::
test (string key) const
{
	return (dictionary.find(key) != dictionary.end())? true : false;
}

template<typename Scalar>
Scalar ArgParser::
get (string keyword, Scalar default_val)
{
	return (dictionary.find(keyword)!=dictionary.end()) ? convert_to_num<Scalar>(dictionary[keyword]) : static_cast<Scalar>(default_val);
	// If not in dictionary, return 0 (false) by default
}

template<typename Scalar>
vector<Scalar> ArgParser::
get_list (string keyword)
{
	vector<string> Vstr;
	vector<Scalar> Vnum;
	
	string arglist = dictionary[keyword]; // arglist="1,2,3"
	find_and_replace(arglist,","," ");
	istringstream ss(arglist);
	copy(istream_iterator<string>(ss), istream_iterator<string>(), back_inserter< vector<string> >(Vstr)); // Vout[0]="1", Vout[1]="2", Vout[2]="3"
	
	// convert to int
	for(auto it=Vstr.begin(); it!=Vstr.end(); ++it)
	{
		istringstream iss(*it);
		Scalar temp;
		iss >> temp;
		Vnum.push_back(temp);
	}
	
	return Vnum;
}

#endif
