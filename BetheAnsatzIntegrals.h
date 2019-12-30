#ifndef INTEGRATION_LIEBWU
#define INTEGRATION_LIEBWU

#include "gsl_integration.h"
#include <gsl/gsl_sf_bessel.h>

struct BetheAnsatzIntegrals
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
double BetheAnsatzIntegrals::u=0.;

double BetheAnsatz_e0 (double U_input, double err=1e-9)
{
	BetheAnsatzIntegrals::u = 0.25*U_input;
	return seminfigrate(BetheAnsatzIntegrals::e0_integrand, 0., err,err);
}

double BetheAnsatz_gap (double U_input, double err=1e-9)
{
	BetheAnsatzIntegrals::u = 0.25*U_input;
	return -2. + 2.*BetheAnsatzIntegrals::u + seminfigrate(BetheAnsatzIntegrals::gap_integrand, 0., err,err);
}

#endif
