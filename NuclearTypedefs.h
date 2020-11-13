#ifndef NUCLEARTYPEDEFS
#define NUCLEARTYPEDEFS

#include <boost/rational.hpp>
#include <boost/algorithm/string.hpp>

#include <Eigen/Dense>

namespace Nuclear
{
	enum PARTICLE {NEUTRON=0, PROTON=1};
	enum JCOUPLING {PLUS=0, MINUS=1};
	
	std::ostream& operator<< (std::ostream& s, PARTICLE P)
	{
		if      (P==PROTON)  {s << "π";}
		else if (P==NEUTRON) {s << "ν";}
		return s;
	}
	
	std::ostream& operator<< (std::ostream& s, JCOUPLING JC)
	{
		if      (JC==PLUS)  {s << "+";}
		else if (JC==MINUS) {s << "-";}
		return s;
	}
	
	struct Eigenstate
	{
		double energy;
		Eigen::VectorXcd state;
		int l;
		int n;
		JCOUPLING JC;
	};
	
	struct less_than_energy
	{
		inline bool operator() (const Eigenstate& s1, const Eigenstate& s2)
		{
			return (s1.energy < s2.energy);
		}
	};
	
	void replace_halves (string &target)
	{
		boost::replace_all(target, "11/2", "¹¹⁄₂");
		boost::replace_all(target, "13/2", "¹³⁄₂");
		boost::replace_all(target, "15/2", "¹⁵⁄₂");
		boost::replace_all(target, "17/2", "¹⁷⁄₂");
		boost::replace_all(target, "1/2", "½");
		boost::replace_all(target, "3/2", "³⁄₂");
		boost::replace_all(target, "5/2", "⁵⁄₂");
		boost::replace_all(target, "7/2", "⁷⁄₂");
		boost::replace_all(target, "9/2", "⁹⁄₂");
	}
	
	int Ztable (const string &el)
	{
		int res = 0;
		if      (el == "O")  res = 8;
		else if (el == "Ca") res = 20;
		else if (el == "Ni") res = 28;
		else if (el == "Sn") res = 50;
		else if (el == "Pb") res = 82;
		return res;
	}
	
	std::array<string,9> specletter = {"s","p","d","f","g","h","i","j","k"};
	
	std::array<int,8> magix = {2,8,20,28,50,82,126,184};
	
	static string levlabel_half (int n, int l, JCOUPLING JC)
	{
		int j1x2 = (JC==PLUS)? 2*l+1 : 2*l-1;
		return make_string(n+1,specletter[l],boost::rational<int>(j1x2,2));
	}
	
	static string levlabel (int n, int l, JCOUPLING JC)
	{
		int j1x2 = (JC==PLUS)? 2*l+1 : 2*l-1;
		return make_string(n+1,specletter[l],j1x2);
	}
};

#endif
