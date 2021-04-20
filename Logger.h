#ifndef RRLOGGER
#define RRLOGGER

#include <ctime>
#include <iostream>
#include <fstream>

#include "ArgParser.h"
#include "StringStuff.h"

class Logger
{
	typedef std::string string;
public:

	Logger (){};
	
	Logger (bool APPEND_IN, bool SHOW_START_TIME_IN = true, bool SHOW_END_TIME_IN = true)
	:APPEND(APPEND_IN), SHOW_START_TIME(SHOW_START_TIME_IN), SHOW_END_TIME(SHOW_END_TIME_IN)
	{};
	
	Logger (string filename_input, string subfolder="./");
	
	Logger (const ArgParser arg, string subfolder="./");
	
	~Logger();
	
	void set (string filename_input, string subfolder, bool APPEND_TIME_TO_FILENAME = false);
	void append();
	void write();
	void set_options(bool APPEND_IN, bool SHOW_START_TIME_IN, bool SHOW_END_TIME_IN)
	{
		APPEND = APPEND_IN; SHOW_START_TIME = SHOW_START_TIME_IN; SHOW_END_TIME = SHOW_END_TIME_IN;
	}
	
	// std::endl is a function template:
	Logger &operator<< (std::ostream& (*f)(std::ostream&));
	
	std::stringstream stream;
	
private:
	
	void construct();
	
	string filename;
	
	bool SHOW_START_TIME = true;
	bool SHOW_END_TIME = true;
	bool APPEND = false;
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
set (string filename_input, string subfolder_input, bool APPEND_TIME_TO_FILENAME)
{
	string subfolder = subfolder_input;
	correct_foldername(subfolder);
	
	string filename_corrected = filename_input;
	
	if (APPEND_TIME_TO_FILENAME)
	{
		time_t now;
		time(&now);
		tm * curr_tm = localtime(&now);
		char now_str_array[100];
		
		// write timestring to now_str
		strftime(now_str_array, 50, "%Y-%m-%dT%H:%M:%S", curr_tm);
		string now_str(now_str_array);
		
		// if logfile ends in '.log', insert timestring before '.log'
		if (filename_corrected[filename_corrected.length()-1] == 'g' and
		    filename_corrected[filename_corrected.length()-2] == 'o' and
		    filename_corrected[filename_corrected.length()-3] == 'l' and
		    filename_corrected[filename_corrected.length()-4] == '.'
		   )
		{
			filename_corrected.pop_back();
			filename_corrected.pop_back();
			filename_corrected.pop_back();
			filename_corrected.pop_back();
		}
		filename_corrected = make_string(filename_corrected,"_start=",now_str,".log");
	}
	else
	{
		// if logfile doesn't end in '.log', append '.log'
		if (filename_corrected[filename_corrected.length()-1] != 'g' and
		    filename_corrected[filename_corrected.length()-2] != 'o' and
		    filename_corrected[filename_corrected.length()-3] != 'l' and
		    filename_corrected[filename_corrected.length()-4] != '.'
		   )
		{
			filename_corrected += ".log";
		}
	}
	
	filename = make_string(subfolder,filename_corrected);
	
	std::cout << "logfile: " << filename << std::endl;
	construct();
}

void Logger::
construct()
{
//	logfile.open(filename);
	if (SHOW_START_TIME)
	{
		time_t now = time(NULL);
		stream << asctime(localtime(&now)) << "\n";
	}
	if (APPEND) {append();}
	else {write();}
}

Logger::
~Logger()
{
	if(SHOW_END_TIME)
	{
		time_t now = time(NULL);
		stream << "\n" << asctime(localtime(&now));
	}
	append();
//	logfile.close();
}

void Logger::
append()
{
//	std::ofstream logfile;
//	logfile.open(filename,ios_base::app);
//	logfile << val;
//	logfile.close();
	
//	logfile << val;
	
	std::ofstream logfile;
	logfile.open(filename,std::ios_base::app);
	logfile << remove_termcolor(stream.str());
	logfile.close();
	stream.str("");
}

void Logger::
write()
{
//	std::ofstream logfile;
//	logfile.open(filename);
//	logfile << val;
//	logfile.close();
	
//	logfile << val;
	
	std::ofstream logfile;
	logfile.open(filename);
	logfile << stream.str();
	logfile.close();
	stream.str("");
}

// std::endl is a function template:
Logger & Logger::operator << (std::ostream& (*f)(std::ostream&))
{
	f(std::cout);
//	f(logfile);
	stream << "\n";
	append();
	return *this;
}

template <class T>
Logger & operator << (Logger &log, T val)
{
	log.stream << val;
	std::cout << val;
	return log;
};

#endif
