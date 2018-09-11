#ifndef INTEGRATION_LIEBWU
#define INTEGRATION_LIEBWU

#include "gsl_integration.h"
#include <gsl/gsl_sf_bessel.h>

struct LiebWu
{
	static double t;
	static double U;	
	static double e0_integrand (double x, void*)
	{
		return -4.*t*gsl_sf_bessel_J0(x)*gsl_sf_bessel_J1(x)/x/(1.+exp(x*(U/t)*0.5));
	}
};
double LiebWu::U=0.;
double LiebWu::t=1.;

double LiebWu_e0 (double U_input, double t_input = 1., double err=1e-9)
{
	LiebWu::U = U_input;
	LiebWu::t = t_input;
	return seminfigrate(LiebWu::e0_integrand, 0., err,err);
}

#endif
