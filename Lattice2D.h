#ifndef LATTICE
#define LATTICE

#include <array>
#include <Eigen/Dense>

#include "numeric_limits.h"

enum LatticeType
	{
	 SQUARE=0,
	 TRIANG=1
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
		Site() {i[0]=0; i[1]=0;}
		Site(int x, int y) {i[0]=x; i[1]=y;}
		array<int,dim> i;
		int operator[] ( std::size_t index ) const {return i[index];}
		int& operator[] ( std::size_t index ) {return i[index];}
	};

	bool ARE_NEIGHBORS(Site i, Site j, std::string atom_i, std::string atom_j) const;

	int size(int d) const {return L[d];}
	int volume() const { int out=1; for (const auto Li:L) {out*=Li;} return out; }

	std::string name() const {return name_;}

	std::unordered_map<std::string,Eigen::Matrix<double,dim,1> > unitCell;
protected:
	
	array<size_t,dim> L;
	
	//unit vectors of the lattice
	array<Eigen::Matrix<double,dim,1>,dim> a;

	//unit vectors of the reciprocal lattice
	array<Eigen::Matrix<double,dim,1>,dim> b;

	
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
			unitCell[""]=Eigen::Matrix<double,dim,1>::Zero();
		}
	else if (type == LatticeType::TRIANG)
		{
			name_ = "Triangular lattice";
			a[0] << 1 , 0;
			a[1] << 0.5 , std::sqrt(3.)*0.5 ;
			neighbor_difference = a[0].norm();
			unitCell[""]=Eigen::Matrix<double,dim,1>::Zero();
		}
	// cout << "a[0]: " << a[0].transpose() << "\ta[1]: " << a[1].transpose() << endl;
	Eigen::Rotation2D<double> rot90(0.5*M_PI);
	Eigen::Matrix<double,dim,dim> rotate90 = rot90.toRotationMatrix();
	b[0] = 2*M_PI * rotate90 * a[1] / a[0].dot(rotate90*a[1]);
	b[1] = 2*M_PI * rotate90 * a[0] / a[1].dot(rotate90*a[0]);
	// cout << "b[0]: " << b[0].transpose() << "\tb[1]: " << b[1].transpose() << endl;
	
	assert(abs(a[0].dot(b[0])-2*M_PI) <1.e-12);
	assert(abs(a[1].dot(b[1])-2*M_PI) <1.e-12);
	assert(abs(a[0].dot(b[1])) <1.e-12);
	assert(abs(a[1].dot(b[0])) <1.e-12);
}

bool Lattice2D::
ARE_NEIGHBORS(Site i, Site j, std::string atom_i="", std::string atom_j="") const
{
	auto it_i = unitCell.find(atom_i);
	auto it_j = unitCell.find(atom_j);
	assert(it_i != unitCell.end() and it_j != unitCell.end() and "You specified an atom in the unit cell which does not exist.");
	auto Ri = i[0]*a[0] + i[1]*a[1] + unitCell.at(atom_i);
	auto Rj = j[0]*a[0] + j[1]*a[1] + unitCell.at(atom_j);
	double distance = (Ri - Rj).norm();
	if (abs(distance-neighbor_difference) < mynumeric_limits<double>::epsilon()) {return true;}
	if (PERIODIC[1] and L[1] > 2)
		{
			//shift R_i into the super cell above the current super cell
			auto Ri_upshift = i[0]*a[0] + (i[1]+L[1])*a[1] + unitCell.at(atom_i);
			double distance_upshift = (Ri_upshift - Rj).norm();
			if (abs(distance_upshift-neighbor_difference) < mynumeric_limits<double>::epsilon()) {return true;}

			//shift R_i into the super cell below the current super cell
			auto Ri_downshift = i[0]*a[0] + (i[1]-L[1])*a[1] + unitCell.at(atom_i);
			double distance_downshift = (Ri_downshift - Rj).norm();
			if (abs(distance_downshift-neighbor_difference) < mynumeric_limits<double>::epsilon()) {return true;}
		}
	return false;
}

#endif
