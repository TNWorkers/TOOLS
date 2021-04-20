#ifndef TERMINAL_PLOT_H_
#define TERMINAL_PLOT_H_

#if __has_include("plot.hpp")
#include "plot.hpp"
#else
#define DONT_USE_TERMINALPLOT
#endif

#include <cmath>
#include <iostream>
#include <string>
#include <numeric>

#include "Ticker.h"

struct Dummy {};
struct PlotParams
{
	#ifdef DONT_USE_TERMINALPLOT
	Dummy color;
	#else
	plot::Color color = plot::palette::steelblue;
	#endif
	std::string label = "";
	std::size_t cellCols = 70;
	std::size_t cellRows = 21;
};

namespace TerminalPlot
{		
	template<typename Container>
	void plot(const Container &x, const Container &y, const PlotParams &params)
	{
		#ifdef DONT_USE_TERMINALPLOT
		std::cout << "X: "; for (const auto &i:x) {std::cout << i << " ";} std::cout << std::endl;
		std::cout << "Y: "; for (const auto &i:y) {std::cout << i << " ";} std::cout << std::endl;
		#else
		plot::TerminalInfo term;
		term.detect();
		plot::Coord canvasCellCols = params.cellCols;
		plot::Coord canvasCellRows = params.cellRows;
		plot::Size canvasCellSize(canvasCellCols, canvasCellRows);

		const auto [it_xmin,it_xmax] = std::minmax_element(std::begin(x), std::end(x));
		const auto [it_ymin,it_ymax] = std::minmax_element(std::begin(y), std::end(y));
		typename Container::value_type xmin = *it_xmin;
		typename Container::value_type xmax = *it_xmax;
		typename Container::value_type ymin = *it_ymin;
		typename Container::value_type ymax = *it_ymax;
		Ticker xticker(xmin,xmax,"X");
		Ticker yticker(ymin,ymax,"Y");
		typename Container::value_type xmin_rnd = xticker.min();
		typename Container::value_type xmax_rnd = xticker.max();
		typename Container::value_type ymin_rnd = yticker.min();
		typename Container::value_type ymax_rnd = yticker.max();
		plot::RealCanvas<plot::BrailleCanvas, typename Container::value_type> canvas({ {xmin_rnd,ymin_rnd}, {xmax_rnd,ymax_rnd} }, canvasCellSize, term);
		
		//compute ticks and labels
		std::vector<std::size_t> xticks_int(9);
		std::size_t xstep = std::floor(params.cellCols/8);
		std::generate(xticks_int.begin(), xticks_int.end(), [xstep, n=-xstep] () mutable {return n+=xstep;});
		std::vector<std::size_t> yticks_int(5);
		std::size_t ystep = std::floor(params.cellRows/4);
		std::generate(yticks_int.begin(), yticks_int.end(), [ystep, n=-ystep] () mutable {return n+=ystep;});
		std::cout << "xticks: "; for (const auto tick:xticks_int) {std::cout << tick << " ";} std::cout << std::endl;
		std::cout << "yticks: "; for (const auto tick:yticks_int) {std::cout << tick << " ";} std::cout << std::endl;
		auto unmap = [&canvas, xmin_rnd](const std::vector<std::size_t> &ticks_int, bool X) {
					   Container out(ticks_int.size());
					   for (std::size_t i=0; i<ticks_int.size(); i++)
					   {
						   if (X) { out[i] = canvas.unmap(plot::Point(2*ticks_int[i],0)).x; }
						   else   { out[i] = canvas.unmap(plot::Point(0,4*ticks_int[i])).y; }
					   }
					   return out;
				   };
		
		Container xticks = unmap(xticks_int,true);
		Container yticks = unmap(yticks_int,false);
		std::vector<std::pair<std::size_t,std::string> > xtickLabels = xticker.labels(xticks, xticks_int);
		std::vector<std::pair<std::size_t,std::string> > ytickLabels = yticker.labels(yticks, yticks_int);
				
		std::vector<plot::GenericPoint<typename Container::value_type> > points;
		for (std::size_t i=0; i<x.size(); i++)
		{
			points.push_back(plot::GenericPoint<typename Container::value_type>(x[i],y[i]));
		}
		
		canvas.path(params.color, points.begin(), points.end());
		std::cout << plot::margin(1, 2, 0, 2, plot::frame(xticks_int, yticks_int, xtickLabels, ytickLabels, params.label, plot::Align::Center, {plot::BorderStyle::Double, true}, &canvas, term)) << std::flush;
		#endif
	}

	template<typename Container>
	void plot(const Container &y, const PlotParams &params)
	{
		Container x(y.size());
		for (size_t i=0; i<x.size(); i++)
		{
			x[i] = typename Container::value_type(i);
		}
		plot(x,y,params);
	}

	template<typename Scalar>
	void plot(const Eigen::Array<Scalar,-1,1> &y, const PlotParams &params)
	{
		std::vector<Scalar> yvec(y.size());
		for (size_t i=0; i<y.size(); i++)
		{
			yvec[i] = y(i);
		}
		std::vector<Scalar> x(y.size());
		for (size_t i=0; i<x.size(); i++)
		{
			x[i] = Scalar(i);
		}
		#ifdef DONT_USE_TERMINALPLOT
		std::cout << y.transpose() << std::endl;
		#else
		plot(x,yvec,params);
		#endif
	}
}
#endif

