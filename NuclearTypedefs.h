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
	
	int Ztable (const string &el, string wd="../TOOLS/")
	{
		std::array<string,104> PeriodicTable;
		PeriodicTable[0] = "NeutronStar";
		PeriodicTable[1] = "H";
		PeriodicTable[2] = "He";
		PeriodicTable[3] = "Li";
		PeriodicTable[4] = "Be";
		PeriodicTable[5] = "B";
		PeriodicTable[6] = "C";
		PeriodicTable[7] = "N";
		PeriodicTable[8] = "O";
		PeriodicTable[9] = "F";
		PeriodicTable[10] = "Ne";
		PeriodicTable[11] = "Na";
		PeriodicTable[12] = "Mg";
		PeriodicTable[13] = "Al";
		PeriodicTable[14] = "Si";
		PeriodicTable[15] = "P";
		PeriodicTable[16] = "S";
		PeriodicTable[17] = "Cl";
		PeriodicTable[18] = "Ar";
		PeriodicTable[19] = "K";
		PeriodicTable[20] = "Ca";
		PeriodicTable[21] = "Sc";
		PeriodicTable[22] = "Ti";
		PeriodicTable[23] = "V";
		PeriodicTable[24] = "Cr";
		PeriodicTable[25] = "Mn";
		PeriodicTable[26] = "Fe";
		PeriodicTable[27] = "Co";
		PeriodicTable[28] = "Ni";
		PeriodicTable[29] = "Cu";
		PeriodicTable[30] = "Zn";
		PeriodicTable[31] = "Ga";
		PeriodicTable[32] = "Ge";
		PeriodicTable[33] = "As";
		PeriodicTable[34] = "Se";
		PeriodicTable[35] = "Br";
		PeriodicTable[36] = "Kr";
		PeriodicTable[37] = "Rb";
		PeriodicTable[38] = "Sr";
		PeriodicTable[39] = "Y";
		PeriodicTable[40] = "Zr";
		PeriodicTable[41] = "Nb";
		PeriodicTable[42] = "Mo";
		PeriodicTable[43] = "Tc";
		PeriodicTable[44] = "Ru";
		PeriodicTable[45] = "Rh";
		PeriodicTable[46] = "Pd";
		PeriodicTable[47] = "Ag";
		PeriodicTable[48] = "Cd";
		PeriodicTable[49] = "In";
		PeriodicTable[50] = "Sn";
		PeriodicTable[51] = "Sb";
		PeriodicTable[52] = "Te";
		PeriodicTable[53] = "I";
		PeriodicTable[54] = "Xe";
		PeriodicTable[55] = "Cs";
		PeriodicTable[56] = "Ba";
		PeriodicTable[57] = "La";
		PeriodicTable[58] = "Ce";
		PeriodicTable[59] = "Pr";
		PeriodicTable[60] = "Nd";
		PeriodicTable[61] = "Pm";
		PeriodicTable[62] = "Sm";
		PeriodicTable[63] = "Eu";
		PeriodicTable[64] = "Gd";
		PeriodicTable[65] = "Tb";
		PeriodicTable[66] = "Dy";
		PeriodicTable[67] = "Ho";
		PeriodicTable[68] = "Er";
		PeriodicTable[69] = "Tm";
		PeriodicTable[70] = "Yb";
		PeriodicTable[71] = "Lu";
		PeriodicTable[72] = "Hf";
		PeriodicTable[73] = "Ta";
		PeriodicTable[74] = "W";
		PeriodicTable[75] = "Re";
		PeriodicTable[76] = "Os";
		PeriodicTable[77] = "Ir";
		PeriodicTable[78] = "Pt";
		PeriodicTable[79] = "Au";
		PeriodicTable[80] = "Hg";
		PeriodicTable[81] = "Tl";
		PeriodicTable[82] = "Pb";
		PeriodicTable[83] = "Bi";
		PeriodicTable[84] = "Po";
		PeriodicTable[85] = "At";
		PeriodicTable[86] = "Rn";
		PeriodicTable[87] = "Fr";
		PeriodicTable[88] = "Ra";
		PeriodicTable[89] = "Ac";
		PeriodicTable[90] = "Th";
		PeriodicTable[91] = "Pa";
		PeriodicTable[92] = "U";
		PeriodicTable[93] = "Np";
		PeriodicTable[94] = "Pu";
		PeriodicTable[95] = "Am";
		PeriodicTable[96] = "Cm";
		PeriodicTable[97] = "Bk";
		PeriodicTable[98] = "Cf";
		PeriodicTable[99] = "Es";
		PeriodicTable[100] = "Fm";
		PeriodicTable[101] = "Md";
		PeriodicTable[102] = "No";
		PeriodicTable[103] = "Lr";
		
		ifstream DataLoader(wd+"PeriodicTable.sav");
		string line;
		int res = 0;
		for (int i=0; i<PeriodicTable.size(); ++i)
		{
			if (PeriodicTable[i] == el)
			{
				res = i;
				break;
			}
		}
		return res;
	}
	
	std::array<string,13> specletter = {"s","p","d","f","g", // 1,3,5,7,9
	                                    "h","i","j","k","l", // 11,13,15,17,19
	                                    "m","n","o"}; // 21,23,25
	
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
