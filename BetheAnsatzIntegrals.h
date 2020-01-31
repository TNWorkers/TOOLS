#ifndef INTEGRATION_LIEBWU
#define INTEGRATION_LIEBWU

#include "gsl_integration.h"
#include <gsl/gsl_sf_bessel.h>

namespace BetheAnsatz{

struct Integrals
{
	static double u;
	static double e0_integrand (double x, void*)
	{
		return -4.*gsl_sf_bessel_J0(x)*gsl_sf_bessel_J1(x)/(x*(1.+exp(x*2.*u)));
	}
	
	static double gap_integrand (double x, void*)
	{
		return (x==0.)? 0.:2.*gsl_sf_bessel_J1(x)*exp(-x*u)/(x*cosh(x*u));
	}
};
double Integrals::u=0.;

double e0 (double U_input, double err=1e-9)
{
	Integrals::u = 0.25*U_input;
	return seminfigrate(Integrals::e0_integrand, 0., err,err);
}

double gap (double U_input, double err=1e-9)
{
	Integrals::u = 0.25*U_input;
	return -2. + 2.*Integrals::u + seminfigrate(Integrals::gap_integrand, 0., err,err);
}

// Essler p. 218
double spinon_v (double U_input)
{
	double x = 2.*M_PI/U_input;
	return (U_input==0.)? 2.:2.*gsl_sf_bessel_I1(x)/gsl_sf_bessel_I0(x);
}

};

#endif
