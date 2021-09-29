#ifndef LATTICE
#define LATTICE

#include <array>
#include <Eigen/Dense>

#include "numeric_limits.h"

enum LatticeType
{
	SQUARE=0,
	TRIANG_XC=1,
	TRIANG_YC=2,
	TRIANG=3
};

std::ostream& operator<< (std::ostream& s, LatticeType lattice)
{
	if      (lattice==SQUARE)     {s << "square";}
	else if (lattice==TRIANG)     {s << "triangular";}
	else if (lattice==TRIANG_XC)     {s << "triangularXC";}
	else if (lattice==TRIANG_YC)     {s << "triangularYC";}
	return s;
}

// template<size_t dim=2>
class Lattice2D
{
	static const std::size_t dim=2;
public:
	
	/**Constructors*/
	Lattice2D () {};
	Lattice2D (array<size_t,dim> L_input, array<bool,dim> PERIODIC_input, LatticeType type_input=LatticeType::SQUARE, size_t furthest_neighbor=1ul);
	
	struct IndexSite
	{
		IndexSite() {i[0]=0; i[1]=0;}
		IndexSite(int x, int y) {i[0]=x; i[1]=y;}
		array<int,dim> i;
		int operator[] ( std::size_t index ) const {return i[index];}
		int& operator[] ( std::size_t index ) {return i[index];}
	};

	bool ARE_NEIGHBORS(IndexSite i, IndexSite j, std::string atom_i, std::string atom_j, size_t range=1) const;

	std::vector<pair<IndexSite,std::string> > neighbors(IndexSite i, std::string atom_i, size_t range=1) const;
	
	int size(int d) const {return L[d];}
	int volume() const { int out=1; for (const auto Li:L) {out*=Li;} return out; }

	std::string name() const {return name_;}

	std::unordered_map<std::string,Eigen::Matrix<double,dim,1> > unitCell;

	Eigen::Matrix<double,dim,1> getSite(const IndexSite& i) const;

	Eigen::Matrix<double,dim,1> getSite(int ix, int iy) const
	{
		return getSite(IndexSite(ix,iy));
	}
	
	//unit vectors of the lattice
	array<Eigen::Matrix<double,dim,1>,dim> a;

	//unit vectors of the reciprocal lattice
	array<Eigen::Matrix<double,dim,1>,dim> b;

	LatticeType type() const {return type_;}
	
protected:
	
	array<size_t,dim> L;
	
	//origin of the coordinate system
	Eigen::Matrix<double,dim,1> origin;

	vector<double> neighbor_distance;

	array<bool,dim> PERIODIC;

	std::string name_="Lattice";

	LatticeType type_;
};

// template<size_t dim>
Lattice2D::
Lattice2D(array<size_t,2> L_input, array<bool,2> PERIODIC_input, LatticeType type_input, size_t furthest_neighbor)
:L(L_input), PERIODIC(PERIODIC_input), type_(type_input)
{
	assert(furthest_neighbor >=1);
	origin.setZero();
	neighbor_distance.resize(furthest_neighbor);
	if (type_ == LatticeType::SQUARE)
		{
			name_ = "Square lattice";
			a[0] << 1. , 0.;
			a[1] << 0. , 1.;
			neighbor_distance[0] = a[0].norm();
			if (furthest_neighbor == 2 and L[1] > 1) {neighbor_distance[1] = (a[0]+a[1]).norm();}
			else if (furthest_neighbor == 2 and L[1] == 1) {neighbor_distance[1] = (2*a[0]).norm();}
			unitCell[""]=Eigen::Matrix<double,dim,1>::Zero();
		}
	else if (type_ == LatticeType::TRIANG_XC or type_ == LatticeType::TRIANG)
		{
			if (type_ == LatticeType::TRIANG_XC) {name_ = "Triangular lattice (XC)";}
			if (type_ == LatticeType::TRIANG) {name_ = "Triangular lattice";}
			a[0] << 1. , 0.;
			a[1] << 0.5 , std::sqrt(3.)*0.5 ;
			neighbor_distance[0] = a[0].norm();
			if (furthest_neighbor == 2 and L[1] > 1) {neighbor_distance[1] = (a[0]+a[1]).norm();}
			else if (furthest_neighbor == 2 and L[1] == 1) {neighbor_distance[1] = (2*a[0]).norm();}
			unitCell[""]=Eigen::Matrix<double,dim,1>::Zero();
		}
	else if (type_ == LatticeType::TRIANG_YC)
		{
			name_ = "Triangular lattice (YC)";
			a[0] << std::sqrt(3.)*0.5 , 0.5 ;
			a[1] << 0. , 1.;
			neighbor_distance[0] = a[0].norm();
			if (furthest_neighbor == 2 and L[1] > 1) {neighbor_distance[1] = (a[0]+a[1]).norm();}
			else if (furthest_neighbor == 2 and L[1] == 1) {neighbor_distance[1] = (2*a[0]).norm();}
			unitCell[""]=Eigen::Matrix<double,dim,1>::Zero();
		}
	Eigen::Rotation2D<double> rot90(0.5*M_PI);
	Eigen::Matrix<double,dim,dim> rotate90 = rot90.toRotationMatrix();
	b[0] = 2*M_PI * rotate90 * a[1] / a[0].dot(rotate90*a[1]);
	b[1] = 2*M_PI * rotate90 * a[0] / a[1].dot(rotate90*a[0]);
	
	assert(abs(a[0].dot(b[0])-2*M_PI) <1.e-12);
	assert(abs(a[1].dot(b[1])-2*M_PI) <1.e-12);
	assert(abs(a[0].dot(b[1])) <1.e-12);
	assert(abs(a[1].dot(b[0])) <1.e-12);
}

Eigen::Matrix<double,2,1> Lattice2D::
getSite(const IndexSite& i) const
{
	Eigen::Matrix<double,2,1> out;
	if (type_ == LatticeType::TRIANG or type_ == LatticeType::SQUARE)
	{
		out = i[0]*a[0] + i[1]*a[1];
	}
	else if (type_ == LatticeType::TRIANG_XC)
	{
		out = (i[0]-std::floor(static_cast<double>(i[1])/2.))*a[0] + i[1]*a[1];
	}
	else if (type_ == LatticeType::TRIANG_YC)
	{
		out = i[0]*a[0] + (i[1]-std::floor(static_cast<double>(i[0])/2.))*a[1];
	}
	else {assert(false and "Unknown LatticeType.");}
	return out;
}

std::vector<pair<Lattice2D::IndexSite,std::string> > Lattice2D::
neighbors(IndexSite i, std::string atom_i="", size_t range) const
{
	std::vector<pair<IndexSite,std::string> > out;
	assert(range <= neighbor_distance.size() and range > 0);
	auto it_i = unitCell.find(atom_i);
	assert(it_i != unitCell.end() and "You specified an atom in the unit cell which does not exist.");
	for (int jx=0; jx<size(0); ++jx)
	for (int jy=0; jy<size(1); ++jy)
	for (const auto &[atom_j,position_j] : unitCell)
	{
		IndexSite j(jx,jy);
		if (ARE_NEIGHBORS(i,j,atom_i,atom_j,range)) {out.push_back(std::make_pair(j,atom_j));}
	}
	return out;
}

bool Lattice2D::
ARE_NEIGHBORS(IndexSite i, IndexSite j, std::string atom_i="", std::string atom_j="", size_t range) const
{
	assert(range <= neighbor_distance.size() and range > 0);
	auto it_i = unitCell.find(atom_i);
	auto it_j = unitCell.find(atom_j);
	assert(it_i != unitCell.end() and it_j != unitCell.end() and "You specified an atom in the unit cell which does not exist.");
	Eigen::Matrix<double,dim,1> Ri = getSite(i) + unitCell.at(atom_i);
	Eigen::Matrix<double,dim,1> Rj = getSite(j) + unitCell.at(atom_j);
	double distance = (Ri - Rj).norm();
	if (abs(distance-neighbor_distance[range-1]) < mynumeric_limits<double>::epsilon()) {return true;}

	if (PERIODIC[1] and L[1] > 2)
		{
			//shift R_i into the super cell above the current super cell
			Eigen::Matrix<double,dim,1> Ri_upshift = getSite(i[0],i[1]+L[1]) + unitCell.at(atom_i);
			double distance_upshift = (Ri_upshift - Rj).norm();
			if (abs(distance_upshift-neighbor_distance[range-1]) < mynumeric_limits<double>::epsilon()) {return true;}

			//shift R_i into the super cell below the current super cell
			Eigen::Matrix<double,dim,1> Ri_downshift = getSite(i[0],i[1]-static_cast<int>(L[1])) + unitCell.at(atom_i);
			double distance_downshift = (Ri_downshift - Rj).norm();
			if (abs(distance_downshift-neighbor_distance[range-1]) < mynumeric_limits<double>::epsilon()) {return true;}
		}
	if (PERIODIC[0] and L[0] > 2)
		{
			//shift R_i into the super cell right of the current super cell
			Eigen::Matrix<double,dim,1> Ri_rightshift = getSite(i[0]+L[0],i[1]) + unitCell.at(atom_i);
			double distance_rightshift = (Ri_rightshift - Rj).norm();
			if (abs(distance_rightshift-neighbor_distance[range-1]) < mynumeric_limits<double>::epsilon()) {return true;}

			//shift R_i into the super cell left of the current super cell
			Eigen::Matrix<double,dim,1> Ri_leftshift = getSite(i[0]-static_cast<int>(L[0]),i[1]) + unitCell.at(atom_i);
			double distance_leftshift = (Ri_leftshift - Rj).norm();
			if (abs(distance_leftshift-neighbor_distance[range-1]) < mynumeric_limits<double>::epsilon()) {return true;}			
		}
	if ( (PERIODIC[0] and L[0] > 2) and (PERIODIC[1] and L[1] > 2) )
		{
			//shift R_i into the super cell above and right of the current super cell
			Eigen::Matrix<double,dim,1> Ri_ru_shift = getSite(i[0]+L[0],i[1]+L[1]) + unitCell.at(atom_i);
			double distance_ru_shift = (Ri_ru_shift - Rj).norm();
			if (abs(distance_ru_shift-neighbor_distance[range-1]) < mynumeric_limits<double>::epsilon()) {return true;}

			//shift R_i into the super cell above and left of the current super cell
			Eigen::Matrix<double,dim,1> Ri_lu_shift = getSite(i[0]-static_cast<int>(L[0]),i[1]+L[1]) + unitCell.at(atom_i);
			double distance_lu_shift = (Ri_lu_shift - Rj).norm();
			if (abs(distance_lu_shift-neighbor_distance[range-1]) < mynumeric_limits<double>::epsilon()) {return true;}

			//shift R_i into the super cell below and right of the current super cell
			Eigen::Matrix<double,dim,1> Ri_rb_shift = getSite(i[0]+L[0],i[1]-static_cast<int>(L[1])) + unitCell.at(atom_i);
			double distance_rb_shift = (Ri_rb_shift - Rj).norm();
			if (abs(distance_rb_shift-neighbor_distance[range-1]) < mynumeric_limits<double>::epsilon()) {return true;}

			//shift R_i into the super cell below and left of the current super cell
			Eigen::Matrix<double,dim,1> Ri_lb_shift = getSite(i[0]-static_cast<int>(L[0]),i[1]-static_cast<int>(L[1])) + unitCell.at(atom_i);
			double distance_lb_shift = (Ri_lb_shift - Rj).norm();
			if (abs(distance_lb_shift-neighbor_distance[range-1]) < mynumeric_limits<double>::epsilon()) {return true;}
		}
	return false;
}

#endif
