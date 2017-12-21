#include <array>

enum TIMESTEPPER_MODE {ONE_DT, TWO_DT};

class TimeStepper
{
public:
	
	TimeStepper(){};
	TimeStepper (double tmin_input, double tmax_input, double dt_input);
	
	// dt12[0] if crit_input(t)==true, otherwise dt12[1]
	TimeStepper (double tmin_input, double tmax_input, std::array<double,2> dt12_input, bool (*crit_input)(double));
	
	double operator*() {return t;}
	
	double step()
	{
		double dt_full = fullstep(t);
		double tnew = t+dt_full;
		
		return (tnew<tmax)? dt_full:tmax-t;
	}
	
	double fullstep (double tval)
	{
		if (MODE == ONE_DT)
		{
			return dt;
		}
		else if (MODE == TWO_DT)
		{
			return (crit(tval))? dt12[0]:dt12[1];
		}
	}
	
	void operator++()
	{
		tcmp += fullstep(t);
		t += step();
	};
	
	TimeStepper& operator= (const double &trhs) {t=trhs; return *this;}
	bool operator<= (const double &trhs) {
//	cout << "tcmp=" << tcmp << ", trhs=" << trhs << ", bool=" << (tcmp<=trhs) << endl;
	return tcmp<=trhs;
	}
	bool operator== (const double &trhs) {return tcmp==trhs;}
	
	double begin();
	double end();
	
private:
	
	TIMESTEPPER_MODE MODE;
	
	double tmin, tmax, dt;
	double t;
	double tcmp; // needed for comparison tcmp>tmax to ensure correct endpoint of iterator
	
	bool (*crit)(double);
	std::array<double,2> dt12;
};

TimeStepper::
TimeStepper (double tmin_input, double tmax_input, double dt_input)
:tmin(tmin_input), tmax(tmax_input), dt(dt_input)
{
	t = tmin;
	tcmp = tmin;
	MODE = ONE_DT;
}

TimeStepper::
TimeStepper (double tmin_input, double tmax_input, std::array<double,2> dt12_input, bool (*crit_input)(double))
:tmin(tmin_input), tmax(tmax_input), dt12(dt12_input), crit(crit_input)
{
	t = tmin;
	tcmp = tmin;
	MODE = TWO_DT;
}

double TimeStepper::
begin()
{
	return tmin;
}

double TimeStepper::
end()
{
	return tmax+fullstep(tmax);
}
