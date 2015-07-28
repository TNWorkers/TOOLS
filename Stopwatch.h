#ifndef STOPWATCH
#define STOPWATCH

#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <sstream>
using namespace std;

enum TIME_UNIT {MILLISECONDS, SECONDS, MINUTES, HOURS, DAYS};

class Stopwatch
{
public:

	Stopwatch();
	Stopwatch (string filename_input);
	
	double time (TIME_UNIT u=SECONDS);

	void start();

	template<typename ThemeType> string info (ThemeType theme, bool RESTART=true);
	string info();
	
	template<typename ThemeType> void check (ThemeType theme);
	void check();
	
private:

	struct timeval t_start, t_end;
	string filename;
	bool SAVING_TO_FILE;
};

Stopwatch::
Stopwatch()
{
	SAVING_TO_FILE = false;
	start();
}

double Stopwatch::
time (TIME_UNIT u)
{
	gettimeofday(&t_end,NULL);
	double dt = (t_end.tv_sec-t_start.tv_sec)*1.+(t_end.tv_usec-t_start.tv_usec)*0.000001;
	
	if (u == MILLISECONDS)
	{
		return 1e-3*dt;
	}
	else if (u == SECONDS)
	{
		return dt;
	}
	else if (u == MINUTES)
	{
		return dt/60.;
	}
	else if (u == HOURS)
	{
		return dt/3600.;
	}
	else if (u == DAYS)
	{
		return dt/86400.;
	}
}

Stopwatch::
Stopwatch (string filename_input)
{
	filename = filename_input;
	ofstream file(filename,ios::trunc); // delete file contents
	fstream outfile;
	outfile.open(filename, fstream::app|fstream::out);
	outfile.close();
	SAVING_TO_FILE = true;
	start();
}

inline void Stopwatch::
start()
{
	gettimeofday(&t_start,NULL);
}

template<typename ThemeType>
void Stopwatch::
check (ThemeType theme)
{
	cout << info(theme) << endl;
}

template<typename ThemeType>
string Stopwatch::
info (ThemeType theme, bool RESTART)
{
	gettimeofday(&t_end,NULL);
	double dt = (t_end.tv_sec-t_start.tv_sec)*1.+(t_end.tv_usec-t_start.tv_usec)*0.000001;
	
	stringstream ss;
	if (dt<60.)
	{
		ss << theme << ": " << dt << " #s";
	}
	else if (dt>=60. && dt<3600.)
	{
		ss << theme << ": " << dt/60. << " #min";
	}
	else if (dt>=3600. && dt<86400.)
	{
		ss << theme << ": " << dt/3600. << " #h";
	}
	else
	{
		ss << theme << ": " << dt/86400. << " #d";
	}
	
	if (SAVING_TO_FILE == true)
	{
		fstream outfile;
		outfile.open(filename, fstream::app|fstream::out);
		outfile << ss.str() << endl;
		outfile.close();
	}
	
	if (RESTART) {start();}
	return ss.str();
}

inline void Stopwatch::
check()
{
	check("");
}

inline string Stopwatch::
info()
{
	return info("");
}

#endif
