#ifndef INTERVALITERATOR
#define INTERVALITERATOR

//#include <gsl/gsl_spline.h>
//#include "gsl_integration.h"
#include <iostream>
#include <complex>

#include <Eigen/Dense>

#include "StringStuff.h"
#include "SimpleListInitializer.h"

inline double uniformGrid (int index, double xmin, double xmax, int xpoints)
{
	double step = (xmax-xmin)/(xpoints-1);
	return xmin + index*step;
}

class IntervalIterator
{
friend class IntervalIntWrapper;
friend double area (IntervalIterator &It, double err_abs_input, double err_rel_input);
friend void normalize (IntervalIterator &It, double err_abs_input, double err_rel_input);

public:

	IntervalIterator(){};
	IntervalIterator (double xmin_input, double xmax_input, int xpoints_input, double (*genfunc_input)(int,double,double,int)=uniformGrid);
	
	int index() const;
	double value() const;
	int points() const {return xpoints;};
	
	double operator*() const {return value();};
	
	int begin (int datasets=1);
	int end();
	void operator++() {++curr_index;};
	void operator--() {--curr_index;};
	
	IntervalIterator& operator = (const int &comp) {curr_index=comp; return *this;}
	inline double operator [] (int index) const {return data(index,0);}
	inline double operator () (int index) const {return data(index,0);}
	inline double coeffRef (int index) const {return data.coeffRef(index,0);};

	bool operator <  (const int &comp) {return(curr_index< comp)?true:false;}
	bool operator >  (const int &comp) {return(curr_index> comp)?true:false;}
	bool operator <= (const int &comp) {return(curr_index<=comp)?true:false;}
	bool operator >= (const int &comp) {return(curr_index>=comp)?true:false;}
	
	bool operator == (const int &comp) {return(curr_index==comp)?true:false;}
	bool operator != (const int &comp) {return(curr_index!=comp)?true:false;}
	
	void insert (double x)
	{
		if (data.cols() != 2) {data.conservativeResize(data.rows(),2);}
		data(curr_index,0) = genfunc(curr_index,xmin,xmax,xpoints);
		data(curr_index,1) = x;
	}
	
	void insert (const Eigen::VectorXd &v)
	{
		if (data.cols() != v.rows()+1) {data.conservativeResize(data.rows(),v.rows()+1);}
		data(curr_index,0) = genfunc(curr_index,xmin,xmax,xpoints);
		data.row(curr_index).tail(v.rows()) = v.transpose();
	}
	
	//void operator << (double d) {insert(d);}
	//void operator << (const complex<double> &x) {insert({x.real(), x.imag()});}
	
	SimpleListInitializer operator << (double x)
	{
//		data.conservativeResize(data.rows(),2);
//		data(curr_index,0) = genfunc(curr_index,xmin,xmax,xpoints);
		data(curr_index,1) = x;
		return SimpleListInitializer(&data,curr_index,2);
	}
	
	SimpleListInitializer operator << (std::complex<double> x)
	{
//		data.conservativeResize(data.rows(),3);
//		data(curr_index,0) = genfunc(curr_index,xmin,xmax,xpoints);
		data(curr_index,1) = x.real();
		data(curr_index,2) = x.imag();
		return SimpleListInitializer(&data,curr_index,3);
	}
	
	#ifndef DONT_USE_INITLIST
	void insert (initializer_list<double> line)
	{
		if (data.cols()!=line.size()) {data.conservativeResize(data.rows(),line.size()+1);}
		data(curr_index,0) = genfunc(curr_index,xmin,xmax,xpoints);
		int i=1;
		for (auto d=line.begin(); d!=line.end(); ++d, ++i)
		{
			data(curr_index,i) = *d;
		}
	}
	#endif
	
	void insert (std::complex<double> x) {insert({x.real(), x.imag()});}
	
	void save (std::string dumpfile);
	void save (std::string dumpfile, int i);
	void save (std::string dumpfile, int imin, int imax);
	void save_EigenMatrix (std::string dumpfile, const Eigen::MatrixXd &M);
	void save_values (std::string dumpfile);
	
	void reset (double xmin_input, double xmax_input, int xpoints_input);
	
	void print_status();
	string info();
	
	Eigen::VectorXd get_values() const;
	Eigen::MatrixXd get_data() const;
	void set_data (const Eigen::MatrixXd &data_input);
	inline int rows() {return data.rows();}
	
	double forward_step();
	double backward_step();
	
private:
	
	Eigen::MatrixXd data;
	
	double xmin;
	double xmax;
	int xpoints;
	int curr_index;
	std::string dumpfile;
	
	double (*genfunc)(int,double,double,int);
	
	double function (double x, void*);
};

IntervalIterator::
IntervalIterator(double xmin_input, double xmax_input, int xpoints_input, double (*genfunc_input)(int,double,double,int))
{
	genfunc = genfunc_input;
	reset(xmin_input, xmax_input, xpoints_input);
}

void IntervalIterator::
reset (double xmin_input, double xmax_input, int xpoints_input)
{
	xmin = xmin_input;
	xmax = xmax_input;
	xpoints = xpoints_input;
	data.resize(xpoints,2);
	for (int ix=0; ix<xpoints; ++ix) {data(ix,0) = genfunc(ix,xmin,xmax,xpoints);}
	curr_index=0;
}

//inline int IntervalIterator::
//begin()
//{
//	//data.conservativeResize(xpoints,1);
//	data.rightCols(data.cols()-1).setZero();
//	return 0;
//}

inline int IntervalIterator::
begin (int datasets)
{
	data.conservativeResize(xpoints,datasets+1);
	data.rightCols(data.cols()-1).setZero();
	return 0;
}

inline int IntervalIterator::
end()
{
	return xpoints;
}

inline double IntervalIterator::
value() const
{
	return genfunc(curr_index,xmin,xmax,xpoints);
}

inline int IntervalIterator::
index() const
{
	return curr_index;
}

void IntervalIterator::
save (std::string dumpfile)
{
	std::ofstream file(dumpfile);
	int Nrows = min(curr_index+1,static_cast<int>(data.rows()));
	file << setprecision(14);
	for (size_t r=0; r<Nrows; ++r)
	{
		for (size_t c=0; c<data.cols(); ++c)
		{
			file << data(r,c);
			if (c != data.cols()-1)
			{
				file << "\t";
			}
		}
		file << endl;
	}
	file.close();
}

void IntervalIterator::
save (std::string dumpfile, int i)
{
	std::ofstream file(dumpfile);
	int Nrows = min(curr_index+1,static_cast<int>(data.rows()));
	Eigen::MatrixXd temp(Nrows,2);
	temp.col(0) = data.col(0).head(Nrows);
	temp.col(1) = data.col(i).head(Nrows);
	save_EigenMatrix(dumpfile,temp);
}

void IntervalIterator::
save (std::string dumpfile, int imin, int imax)
{
	int Nrows = min(curr_index+1,static_cast<int>(data.rows()));
	Eigen::MatrixXd temp(Nrows,1+imax-imin+1);
	temp.col(0) = data.col(0).head(Nrows);
	temp.block(0,1, Nrows,imax-imin+1) = data.block(0,imin, Nrows,imax-imin+1);
	save_EigenMatrix(dumpfile,temp);
}

void IntervalIterator::
save_EigenMatrix (std::string dumpfile, const Eigen::MatrixXd &M)
{
	std::ofstream file(dumpfile);
	file << setprecision(14);
	for (int i=0; i<M.rows(); ++i)
	{
		for (int j=0; j<M.cols(); ++j)
		{
			file << M(i,j);
			if (j != M.cols()-1) file << "\t";
		}
		if (i != M.rows()-1) file << endl;
	}
	file.close();
}

void IntervalIterator::
save_values (std::string dumpfile)
{
	std::ofstream file(dumpfile);
	file << setprecision(14) << data.col(0) << endl;
	file.close();
}

void IntervalIterator::
print_status()
{
	draw_progressBar(20,static_cast<double>(curr_index+1.)/static_cast<double>(xpoints));
	if (curr_index==end()-1) {cout << "\x1B[2K" << "\x1B[0E";}
}

string IntervalIterator::
info()
{
	std::stringstream ss;
	std::string xpoints_str;// = static_cast<ostringstream*>(&(ostringstream()<<xpoints))->str();
	ss << "Interval from " << xmin << " to " << xmax << ": "
	     << pad_zeros(curr_index+1, xpoints_str.size()) << "/" << xpoints
	     << ", x=" << round(data(curr_index,0),1);
	if (data.cols() > 1)
	{
		ss << ", f(x) = " << data.block(curr_index,1, 1,data.cols()-1);
//		for (int i=1; i<data.cols(); ++i)
//		{
//			cout << data(curr_index,i) << "\t";
//		}
	}
	return ss.str();
}

double IntervalIterator::
function (double x, void*)
{
//	double * x_axis;
//	double * y_value;
//	x_axis  = (double*) malloc (xpoints * sizeof * x_axis);
//	y_value = (double*) malloc (xpoints * sizeof * y_value);

//	for (int i=0; i<xpoints; ++i)
//	{
//		x_axis[i] = data(i,0);
//		y_value[i] = data(i,1);
//	}
//	
//	gsl_interp_accel * acc_int = gsl_interp_accel_alloc();
//	gsl_spline * spline_int = gsl_spline_alloc (gsl_interp_akima, xpoints);
//	
//	gsl_spline_init (spline_int, x_axis, y_value, xpoints);
//	
//	return gsl_spline_eval(spline_int, x, acc_int);
	return 1.;
}

inline double IntervalIterator::
forward_step()
{
	if (curr_index<=data.rows()-2)
	{
		return data(curr_index+1,0)-data(curr_index,0);
	}
	else
	{
		return 0.;
	}
}

inline double IntervalIterator::
backward_step()
{
	if (curr_index>=1)
	{
		return data(curr_index,0)-data(curr_index-1,0);
	}
	else
	{
		return 0.;
	}
}

inline Eigen::VectorXd IntervalIterator::
get_values() const
{
	return data.col(0);
}

inline Eigen::MatrixXd IntervalIterator::
get_data() const
{
	return data;
}

inline void IntervalIterator::
set_data (const Eigen::MatrixXd &data_input)
{
	assert(data.rows() == data_input.rows());
	data.conservativeResize(data_input.rows(), data_input.cols()+1);
	data.block(0,1, data_input.rows(),data_input.cols()) = data_input;
}

//-------------------------------------------------------------------
class IntervalIntWrapper
{
public:

	static double function_glue (double x, void *){void * empty; return fObj->function(x,empty);}
	static void setObj (IntervalIterator &obj) {fObj = &obj;}

private:

	static IntervalIterator * fObj;
};

IntervalIterator * IntervalIntWrapper::fObj=NULL;
//-------------------------------------------------------------------

// integrate datapoints in given interval and normalize datapoints to this area

double area (IntervalIterator &It, double err_abs_input=1e-7, double err_rel_input=1e-7)
{
//	IntervalIntWrapper::setObj(It);
//	
//	integration_params PARAM;
//	//PARAM.xl=It.data(0,0); PARAM.xr=It.data(It.data.n_rows-1,0); PARAM.yl=It.data(0,1); PARAM.yr=It.data(It.data.n_rows-1,1);
//	PARAM.xl=It.data(0,0); PARAM.xr=It.data(It.data.rows()-1,0); PARAM.yl=It.data(0,1); PARAM.yr=It.data(It.data.rows()-1,1);  
//	PARAM.err_abs=err_abs_input; PARAM.err_rel=err_rel_input;
//	PARAM.GKrule=3;
//	
//	return integrate(&IntervalIntWrapper::function_glue, PARAM);
	return 1.;
}

void normalize (IntervalIterator &It, double err_abs_input=1e-7, double err_rel_input=1e-7)
{
	double A = area(It, err_abs_input,err_rel_input);
	It.data.col(1) = It.data.col(1)/A;
}

#endif
