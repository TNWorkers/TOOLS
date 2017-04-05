#ifndef STOPWATCH
#define STOPWATCH

#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>

#include "macros.h"

enum TIME_UNIT {MILLISECONDS, SECONDS, MINUTES, HOURS, DAYS};

template<typename ClockClass=std::chrono::high_resolution_clock>
class Stopwatch
{
public:
	Stopwatch();
	Stopwatch (std::string filename_input);
	
	double time (TIME_UNIT u=SECONDS);

	void start();

	template<typename ThemeType> std::string info (ThemeType theme, bool RESTART=true);
	inline std::string info();
	
	template<typename ThemeType> void check (ThemeType theme);
	void check();

#ifdef HELPERS_HAS_CONSTEXPR
	template<typename ThemeType, class F, class... ArgTypes>
	std::result_of_t<F&&(ArgTypes&&...)> runTime(ThemeType theme, F&& f, ArgTypes&&... args);
#endif	
	
private:
	std::chrono::time_point<ClockClass> t_start, t_end;
	std::string filename;
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
Stopwatch<ClockClass>::
Stopwatch (std::string filename_input)
{
	filename = filename_input;
	std::ofstream file(filename,std::ios::trunc); // delete file contents
	std::fstream outfile;
	outfile.open(filename, std::fstream::app|std::fstream::out);
	outfile.close();
	SAVING_TO_FILE = true;
	start();
}

template<typename ClockClass>
double Stopwatch<ClockClass>::
time (TIME_UNIT u)
{
	t_end = ClockClass::now();

	if (u == MILLISECONDS)
	{
		std::chrono::duration<double, std::ratio<1,1000> > dt = t_end-t_start;
		return dt.count();
	}
	else if (u == SECONDS)
	{
		std::chrono::duration<double, std::ratio<1,1> > dt = t_end-t_start;
		return dt.count();
	}
	else if (u == MINUTES)
	{
		std::chrono::duration<double, std::ratio<60,1> > dt = t_end-t_start;
		return dt.count();
	}
	else if (u == HOURS)
	{
		std::chrono::duration<double, std::ratio<3600,1> > dt = t_end-t_start;
		return dt.count();
	}
	else if (u == DAYS)
	{
		std::chrono::duration<double, std::ratio<86400,1> > dt = t_end-t_start;
		return dt.count();
	}
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
	std::cout << info(theme) << std::endl;
}

template<typename ClockClass>
template<typename ThemeType>
std::string Stopwatch<ClockClass>::
info (ThemeType theme, bool RESTART)
{
	t_end = ClockClass::now();

	std::chrono::duration<double, std::ratio<1,1> > dtTest = t_end-t_start;

	std::stringstream ss;
	if (dtTest.count()<60.)
	{
		std::chrono::duration<double, std::ratio<1,1> > dt = t_end-t_start;
		ss << theme << ": " << dt.count() << " #s";
	}
	else if (dtTest.count()>=60. && dtTest.count()<3600.)
	{
		std::chrono::duration<double, std::ratio<60,1> > dt = t_end-t_start;
		ss << theme << ": " << dt.count() << " #min";
	}
	else if (dtTest.count()>=3600. && dtTest.count()<86400.)
	{
		std::chrono::duration<double, std::ratio<3600,1> > dt = t_end-t_start;
		ss << theme << ": " << dt.count() << " #h";
	}
	else
	{
		std::chrono::duration<double, std::ratio<86400,1> > dt = t_end-t_start;
		ss << theme << ": " << dt.count() << " #d";
	}
	
	if (SAVING_TO_FILE == true)
	{
		std::fstream outfile;
		outfile.open(filename, std::fstream::app|std::fstream::out);
		outfile << ss.str() << std::endl;
		outfile.close();
	}
	
	if (RESTART) {start();}
	return ss.str();
}

#ifdef HELPERS_HAS_CONSTEXPR
template<typename ClockClass>
template <typename ThemeType,  class F, class... ArgTypes>
std::result_of_t<F&&(ArgTypes&&...)> Stopwatch<ClockClass>::
runTime(ThemeType theme, F&& f, ArgTypes&&... args)
{
	start();
	if constexpr ( std::is_void<std::result_of_t<F&&(ArgTypes&&...)> >::value )
				 {
					 return std::invoke(std::forward<F>(f),std::forward<ArgTypes>(args)...);
					 // if (SAVING_TO_FILE == false) { std::cout << info(theme) << std::endl; }
					 // else { info(theme); }
				 }
	else
	{
		std::result_of_t<F&&(ArgTypes&&...)> result = std::invoke(std::forward<F>(f),std::forward<ArgTypes>(args)...);
		if (SAVING_TO_FILE == false) { std::cout << info(theme) << std::endl; }
		else { info(theme); }
		return result;
	}
}
#endif

template<typename ClockClass>
inline void Stopwatch<ClockClass>::
check()
{
	check("");
}

template<typename ClockClass>
inline std::string Stopwatch<ClockClass>::
info()
{
	return info("");
}

#endif
