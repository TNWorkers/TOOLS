#ifndef RRLOGGER
#define RRLOGGER

#include <ctime>
#include <iostream>
#include <fstream>
using namespace std;

#include "ArgParser.h"
#include "StringStuff.h"

class Logger
{
public:

	Logger(){};
	Logger (string filename_input, string subfolder="./");
	Logger (const ArgParser arg, string subfolder="./");
	~Logger();
	
	void set (string filename_input, string subfolder);
	void append();
	void write();
	
	// std::endl is a function template:
	Logger &operator<< (ostream& (*f)(ostream&));
	
	stringstream stream;

private:

	void construct();

	string filename;
};

Logger::
Logger (string filename_input, string subfolder)
{
	set(filename_input,subfolder);
}

Logger::
Logger (const ArgParser arg, string subfolder)
{
	filename = make_string("./",subfolder,"/",arg.str(),".log");
	construct();
}

void Logger::
set (string filename_input, string subfolder_input)
{
	string subfolder = subfolder_input;
//	if (subfolder.back() != '/') {subfolder += "/";}
//	if (subfolder.at(0) != '.' and subfolder.at(1) != '/') {subfolder = "./"+subfolder;}
	make_foldername(subfolder);
	filename = make_string(subfolder,filename_input);
	cout << "logfile: " << filename << endl;
	construct();
}

void Logger::
construct()
{
//	logfile.open(filename);
	time_t now = time(NULL);
	stream << asctime(localtime(&now)) << "\n";
	write();
}

Logger::
~Logger()
{
	time_t now = time(NULL);
	stream << "\n" << asctime(localtime(&now));
	append();
//	logfile.close();
}

void Logger::
append()
{
//	ofstream logfile;
//	logfile.open(filename,ios_base::app);
//	logfile << val;
//	logfile.close();
	
//	logfile << val;

	ofstream logfile;
	logfile.open(filename,ios_base::app);
	logfile << stream.str();
	logfile.close();
	stream.str("");
}

void Logger::
write()
{
//	ofstream logfile;
//	logfile.open(filename);
//	logfile << val;
//	logfile.close();
	
//	logfile << val;

	ofstream logfile;
	logfile.open(filename);
	logfile << stream.str();
	logfile.close();
	stream.str("");
}

// std::endl is a function template:
Logger & Logger::operator << (ostream& (*f)(ostream&))
{
	f(cout);
//	f(logfile);
	stream << "\n";
	append();
	return *this;
}

template <class T>
Logger & operator << (Logger &log, T val)
{
	log.stream << val;
	cout << val;
	return log;
};

#endif
