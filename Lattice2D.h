#ifndef LATTICE
#define LATTICE

#include <array>
#include <Eigen/Dense>

#include "numeric_limits.h"

enum LatticeType
	{
	 SQUARE,
	 TRIAG
	};

// template<size_t dim=2>
class Lattice2D
{
	static const std::size_t dim=2;
public:
	
	/**Constructors*/
	Lattice2D () {};
	Lattice2D (array<size_t,dim> L_input, array<bool,dim> PERIODIC_input, LatticeType type_input=LatticeType::SQUARE);
	
	struct Site
	{
		array<int,dim> i;
		int operator[] ( std::size_t index ) const {return i[index];}
		int& operator[] ( std::size_t index ) {return i[index];}
	};

	bool ARE_NEIGHBORS(Site i, Site j) const;

	int size(int d) const {return L[d];}
	int volume() const { int out=1; for (const auto Li:L) {out*=Li;} return out; }

	std::string name() const {return name_;}
protected:
	
	array<size_t,dim> L;

	//unit vectors of the lattice
	array<Eigen::Matrix<double,dim,1>,dim> a;
	
	//origin of the coordinate system
	Eigen::Matrix<double,dim,1> origin;

	double neighbor_difference;

	array<bool,dim> PERIODIC;

	std::string name_="Lattice";

	LatticeType type;
};

// template<size_t dim>
Lattice2D::
Lattice2D(array<size_t,2> L_input, array<bool,2> PERIODIC_input, LatticeType type_input)
	:L(L_input), PERIODIC(PERIODIC_input), type(type_input)
{
	origin.setZero();
	if (type == LatticeType::SQUARE)
		{
			name_ = "Square lattice";
			a[0] << 1 , 0;
			a[1] << 0 , 1;
			neighbor_difference = a[0].norm();
		}
	else if (type == LatticeType::TRIAG)
		{
			name_ = "Triangular lattice";
			a[0] << 1 , 0;
			a[1] << 0.5 , std::sqrt(3.)*0.5 ;
			neighbor_difference = a[0].norm();
		}
}

bool Lattice2D::
ARE_NEIGHBORS(Site i, Site j) const
{
	auto Ri = i[0]*a[0] + i[1]*a[1];
	auto Rj = j[0]*a[0] + j[1]*a[1];
	double distance = (Ri - Rj).norm();
	if (abs(distance-neighbor_difference) < mynumeric_limits<double>::epsilon()) {return true;}
	if (PERIODIC[1] and L[1] > 2)
		{
			//shift R_i into the super cell above the current super cell
			auto Ri_upshift = i[0]*a[0] + (i[1]+L[1])*a[1];
			double distance_upshift = (Ri_upshift - Rj).norm();
			if (abs(distance_upshift-neighbor_difference) < mynumeric_limits<double>::epsilon()) {return true;}

			//shift R_i into the super cell below the current super cell
			auto Ri_downshift = i[0]*a[0] + (i[1]-L[1])*a[1];
			double distance_downshift = (Ri_downshift - Rj).norm();
			if (abs(distance_downshift-neighbor_difference) < mynumeric_limits<double>::epsilon()) {return true;}
		}
	return false;
}

#endif
