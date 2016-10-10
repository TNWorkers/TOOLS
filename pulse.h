#ifndef PULSE
#define PULSE

#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef USE_HDF5_STORAGE
#include "HDF5Interface.h"
#endif

template<typename Scalar, typename TimeScalar>
class Gauss
{
public:
	Gauss() {};
	Gauss(Scalar width_in, Scalar offset_in) {width = width_in; offset = offset_in;};
	Scalar value(TimeScalar t) {return std::exp(-std::pow((t-offset)/width,2));};
	std::string info() {std::stringstream ss; ss << "Gaussian Envelope (width=" << width << ", offset=" << offset << ")"; return ss.str();}
private:
	Scalar width;
	Scalar offset;
};

template<typename Scalar, typename TimeScalar, typename Envelope >
class Pulse
{
public:
	Pulse( Scalar omega_in, Scalar amp_in, Scalar tMax_in, Envelope env_in );

	Scalar value( TimeScalar t );
	void save( std::string fName );
	std::string info();
private:
	Scalar omega;
	Scalar amplitude;
	Scalar tMax;
	Envelope env;
};

template<typename Scalar, typename TimeScalar,typename Envelope>
Pulse<Scalar,TimeScalar,Envelope>::
Pulse (Scalar omega_in, Scalar amp_in, Scalar tMax_in, Envelope env_in)
{
	omega = omega_in;
	amplitude = amp_in;
	tMax = tMax_in;
	env = env_in;
}

template<typename Scalar, typename TimeScalar, typename Envelope>
Scalar Pulse<Scalar,TimeScalar,Envelope>::
value (TimeScalar t)
{
	return amplitude*env.value(t)*std::cos(omega*t);
}

template<typename Scalar, typename TimeScalar, typename Envelope>
std::string Pulse<Scalar,TimeScalar,Envelope>::
info ()
{
	std::stringstream ss;
	ss << env.info() << ", Pulse(frequency=" << omega << ",amplitude=" << amplitude << ")";
	return ss.str();
}

template<typename Scalar, typename TimeScalar, typename Envelope>
void Pulse<Scalar,TimeScalar,Envelope>::
save (std::string fName)
{
	double dt = 2*tMax/10000;
	std::vector<TimeScalar> ts;
	std::vector<Scalar> pulse;
	for (double t=-tMax; t<tMax; t+=dt)
	{
		ts.push_back(t);
		pulse.push_back(value(t));
	}
#ifdef USE_HDF5_STORAGE
	fName+=".h5";
	HDF5Interface target(fName, WRITE);
	std::string tName="time";
	std::string pName="pulse";
	target.save_vector(ts.data(),ts.size(),tName.c_str());
	target.save_vector(pulse.data(),pulse.size(),pName.c_str());
#else
	fName+=".dat";
	std::ofstream target(fName);
	for (size_t i=0; i<ts.size(); i++)
	{
		target << ts[i] << "\t" << pulse[i] << std::endl;
	}
	target.close();	
#endif
}


#endif
