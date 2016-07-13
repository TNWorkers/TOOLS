#ifndef STOPWATCH
#define STOPWATCH

#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
namespace std
{

enum TIME_UNIT {MILLISECONDS, SECONDS, MINUTES, HOURS, DAYS};

template<typename ClockClass=chrono::high_resolution_clock>
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
	chrono::time_point<ClockClass> t_start, t_end;
	string filename;
	bool SAVING_TO_FILE;
};

template<typename ClockClass>
Stopwatch<ClockClass>::
Stopwatch()
{
	SAVING_TO_FILE = false;
	start();
}

template<typename ClockClass>
double Stopwatch<ClockClass>::
time (TIME_UNIT u)
{
	t_end = ClockClass::now();

	if (u == MILLISECONDS)
	{
		chrono::duration<double, ratio<1,1000> > dt = t_end-t_start;
		return dt.count();
	}
	else if (u == SECONDS)
	{
		chrono::duration<double, ratio<1,1> > dt = t_end-t_start;
		return dt.count();
	}
	else if (u == MINUTES)
	{
		chrono::duration<double, ratio<60,1> > dt = t_end-t_start;
		return dt.count();
	}
	else if (u == HOURS)
	{
		chrono::duration<double, ratio<3600,1> > dt = t_end-t_start;
		return dt.count();
	}
	else if (u == DAYS)
	{
		chrono::duration<double, ratio<86400,1> > dt = t_end-t_start;
		return dt.count();
	}
}

template<typename ClockClass>
Stopwatch<ClockClass>::
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

template<typename ClockClass>
inline void Stopwatch<ClockClass>::
start()
{
	t_start = ClockClass::now();
}

template<typename ClockClass>
template<typename ThemeType>
void Stopwatch<ClockClass>::
check (ThemeType theme)
{
	cout << info(theme) << endl;
}

template<typename ClockClass>
template<typename ThemeType>
string Stopwatch<ClockClass>::
info (ThemeType theme, bool RESTART)
{
	t_end = ClockClass::now();

	chrono::duration<double, ratio<1,1> > dtTest = t_end-t_start;

	stringstream ss;
	if (dtTest.count()<60.)
	{
		chrono::duration<double, ratio<1,1> > dt = t_end-t_start;
		ss << theme << ": " << dt.count() << " #s";
	}
	else if (dtTest.count()>=60. && dtTest.count()<3600.)
	{
		chrono::duration<double, ratio<60,1> > dt = t_end-t_start;
		ss << theme << ": " << dt.count() << " #min";
	}
	else if (dtTest.count()>=3600. && dtTest.count()<86400.)
	{
		chrono::duration<double, ratio<3600,1> > dt = t_end-t_start;
		ss << theme << ": " << dt.count() << " #h";
	}
	else
	{
		chrono::duration<double, ratio<86400,1> > dt = t_end-t_start;
		ss << theme << ": " << dt.count() << " #d";
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

template<typename ClockClass>
inline void Stopwatch<ClockClass>::
check()
{
	check("");
}

template<typename ClockClass>
inline string Stopwatch<ClockClass>::
info()
{
	return info("");
}
	
} //end namespace std

#endif
