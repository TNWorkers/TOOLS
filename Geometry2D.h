#ifndef GEOMETRY2D
#define GEOMETRY2D

#include <numeric>

#include <Eigen/Dense>

#include "Lattice2D.h"

enum TRAVERSE2D {CHESSBOARD, SNAKE};

class Geometry2D
{
public:
	
	/**constant coupling λ*/
	Geometry2D (const Lattice2D &lattice_input, TRAVERSE2D path_input, vector<double> lambda={1.});
	
	// /**constant couplings λx and λy*/
	// Geometry2D (const Lattice<2> &lattice_input, TRAVERSE2D path_input, int Lx_input, int Ly_input, double lambda_x, double lambda_y, bool PERIODIC_Y=false);
	
	// /**arbitrary couplings (coupling_y is of size Ly+1 and the last element sets the boundary condition)*/
	// Geometry2D (const Lattice<2> &lattice_in, TRAVERSE2D path_input, int Lx_input, int Ly_input, const ArrayXd coupling_x, const ArrayXd coupling_y);
	
	/**return hopping matrix*/
	Eigen::ArrayXXd hopping(size_t range=1ul) const {assert(range>0 and range<=coupling_neighbor.size()); return HoppingMatrix[range-1];}
	
	/**return hopping matrix*/
	template<typename Scalar=double>
	static vector<vector<std::pair<size_t,Scalar> > > rangeFormat (const Eigen::Array<Scalar,Dynamic,Dynamic> &hop);
	
	/**access x,y,atom(index)*/
	inline tuple<int,int,std::string> operator() (int i)        const {return coord.at(i);}
	
	/**access index(x,y,atom)*/
	inline int           operator() (int i, int j, std::string atom="") const {return index.at(make_tuple(i,j,atom));}
	
	/**all x coordinates at a given iy*/
	Eigen::ArrayXd x_row (int iy) const;
	
	/**average distance of the hopping matrix*/
	static double avgd (const Eigen::ArrayXXd &hop);
	
	/**standard deviation of the distance in the hopping matrix*/
	static double sigma (const Eigen::ArrayXXd &hop);
	
	/**maximum hopping distance of the hopping matrix*/
	static double maxd (const Eigen::ArrayXXd &hop);
	
	/**maximum value of the hopping matrix*/
	static double maxval (const Eigen::ArrayXXd &hop);
	
	/**minimum value of the hopping matrix*/
	static double minval (const Eigen::ArrayXXd &hop);
	
	static string hoppingInfo (const Eigen::ArrayXXd &hop);
	
	static string hoppingInfo (const Eigen::ArrayXXcd &hop);
	
	/**Coefficients for the Fourier transform in y-direction.*/
	vector<complex<double> > FTy_phases (int ix_fixed, int iky, bool PARITY, std::string atom) const;

	std::size_t numberOfBonds() const {return number_of_bonds;}

	Lattice2D lattice() const {return lattice_;}
private:
	
	TRAVERSE2D path;
	
	vector<Eigen::ArrayXXd> HoppingMatrix;
	
	map<tuple<int,int,std::string>,int> index;
	map<int,tuple<int,int,std::string>> coord;
	
	void fill_HoppingMatrix (size_t range=1ul);

	Lattice2D lattice_;

	vector<double> coupling_neighbor;

	std::size_t number_of_bonds;
};

Geometry2D::
Geometry2D(const Lattice2D &lattice_input, TRAVERSE2D path_input, vector<double> lambda)
	:path(path_input), coupling_neighbor(lambda), lattice_(lattice_input)
{
	number_of_bonds = 0ul;
	HoppingMatrix.resize(coupling_neighbor.size());
	for (size_t range=0; range<coupling_neighbor.size(); range++)
	{
		fill_HoppingMatrix(range+1);
	}
	number_of_bonds = static_cast<size_t>(number_of_bonds/2);
}

// Geometry2D::
// Geometry2D(const Lattice<2> &lattice_input, TRAVERSE2D path_input, int Lx_input, int Ly_input, double lambda_x, double lambda_y, bool PERIODIC_Y, double coupling_triangular_input)
// :lattice(lattice_input),path(path_input), Lx(Lx_input), Ly(Ly_input), coupling_triangular(coupling_triangular_input)
// {
// 	if (abs(coupling_triangular) > std::numeric_limits<double>::epsilon()) { TRIANGULAR = true; }
	
// 	ArrayXd coupling_x(Lx);
// 	ArrayXd coupling_y;
	
// 	if (PERIODIC_Y)
// 	{
// 		coupling_y.resize(Ly+1);
// 	}
// 	else
// 	{
// 		coupling_y.resize(Ly);
// 	}
	
// 	coupling_x = lambda_x;
// 	coupling_y = lambda_y;
	
// 	fill_HoppingMatrix(coupling_x,coupling_y);
// }

// Geometry2D::
// Geometry2D(const Lattice<2> &lattice_input, TRAVERSE2D path_input, int Lx_input, int Ly_input, const ArrayXd coupling_x, const ArrayXd coupling_y, double coupling_triangular_input)
// :lattice(lattice_input),path(path_input), Lx(Lx_input), Ly(Ly_input), coupling_triangular(coupling_triangular_input)
// {
// 	if (abs(coupling_triangular) > std::numeric_limits<double>::epsilon()) { TRIANGULAR = true; }

// 	fill_HoppingMatrix(coupling_x,coupling_y);
// }

void Geometry2D::
fill_HoppingMatrix (size_t range)
{
	cout << "computing for range=" << range << endl;
	assert(range>0 and range<=coupling_neighbor.size());
	
	if (lattice_.size(0)==1) {assert(path != SNAKE and "Must use Lx>=2 with the SNAKE geometry!");}
	
	HoppingMatrix[range-1].resize(lattice_.volume()*lattice_.unitCell.size(),lattice_.volume()*lattice_.unitCell.size()); HoppingMatrix[range-1].setZero();
	if (coupling_neighbor[range-1] < 1.e-8) {return;}
	
	// Mirrors the y coordinate to create a snake.
	auto mirror = [this] (int iy) -> int
	{
		vector<int> v(lattice_.size(1));
		std::iota (begin(v),end(v),0);
		reverse(v.begin(),v.end());
		return v[iy];
	};

	for (int ix=0; ix<lattice_.size(0); ++ix)
	for (int iy=0; iy<lattice_.size(1); ++iy)
	for (int jx=0; jx<lattice_.size(0); ++jx)
	for (int jy=0; jy<lattice_.size(1); ++jy)
	{
		int countCellAtoms_i=-1;
		for (const auto &[atom_i,position_i] : lattice_.unitCell)
		{
			countCellAtoms_i++;
			int countCellAtoms_j=-1;
			for (const auto &[atom_j,position_j] : lattice_.unitCell)
			{
				countCellAtoms_j++;
				int iy_=iy, jy_=jy;
				if (path == SNAKE)
				{
					// mirror even y only
					iy_ = (ix%2==0)? iy : mirror(iy);
					jy_ = (jx%2==0)? jy : mirror(jy);
				}
				// the index is calculated normally:
				int index_i = iy*lattice_.unitCell.size()+lattice_.size(1)*lattice_.unitCell.size()*ix+countCellAtoms_i;
				int index_j = jy*lattice_.unitCell.size()+lattice_.size(1)*lattice_.unitCell.size()*jx+countCellAtoms_j;
		
				// but is stored together with the mirrored y_:
				if (jx == 0 and jy == 0)
				{
					index[make_tuple(ix,iy_,atom_i)] = index_i;
					coord[index_i] = make_tuple(ix,iy_,atom_i);
//			cout << "ix=" << ix << ", iy_=" << iy_ << ", index_i=" << index_i << endl;
				}

				if (lattice_.ARE_NEIGHBORS( {ix,iy_},{jx,jy_}, atom_i, atom_j, range))
				{
					HoppingMatrix[range-1](index_i,index_j) += coupling_neighbor[range-1];
					number_of_bonds++;
				}
			}
		}
	}
	assert(number_of_bonds%2 == 0);
}

template<typename Scalar>
vector<vector<std::pair<size_t,Scalar> > > Geometry2D::
rangeFormat (const Eigen::Array<Scalar,Dynamic,Dynamic> &hop)
{
	vector<vector<std::pair<size_t,Scalar> > > out(hop.rows());
	
	for (int i=0; i<hop.rows(); ++i)
	for (int j=i; j<hop.cols(); ++j)
	{
		if (abs(hop(i,j)) > ::mynumeric_limits<double>::epsilon())
		{
			out[i].push_back(make_pair(abs(j-i),hop(i,j)));
		}
	}
	
	return out;
}

Eigen::ArrayXd Geometry2D::
x_row (int iy) const
{
	Eigen::ArrayXd out(lattice_.size(0)*lattice_.unitCell.size());
	for (int ix=0; ix<lattice_.size(0); ++ix)
	for (const auto &[atom,position]: lattice_.unitCell)
	{
		out(ix) = index.at(make_tuple(ix,iy,atom));
	}
	return out;
}

vector<complex<double> > Geometry2D::
FTy_phases (int x, int iky, bool PARITY, std::string atom="") const
{
	vector<complex<double> > out(lattice_.volume());
	for (int l=0; l<lattice_.volume(); ++l)
	{
		out[l] = 0.;
	}
	
	double sign = pow(-1.,PARITY);
	double ky = iky * 2.*M_PI/lattice_.size(1);
	
//	cout << "iky=" << iky << ", ky=" << ky << endl;
	
	for (int y=0; y<lattice_.size(1); ++y)
	{
		int i = index.at(make_tuple(x,y,atom));
		
		out[i] = exp(sign*1.i*ky*static_cast<double>(y)) / sqrt(lattice_.size(1));
		
//		cout << "ky=" << ky << ", x=" << x << ", y=" << y << ", i=" << i << ", index.at(make_pair(x,y))=" << index.at(make_pair(x,y)) << ", val=" << out[i] << endl;
	}
	return out;
}

double Geometry2D::
avgd (const Eigen::ArrayXXd &hop)
{
	double res = 0.;
	
	Eigen::ArrayXd x(hop.rows()); x = 0;
	
	for (int i=0; i<hop.rows(); ++i)
	for (int j=i; j<hop.cols(); ++j)
	{
		if (abs(hop(i,j)) > ::mynumeric_limits<double>::epsilon()) x(i) += abs(j-i);
	}
	
	return x.sum()/x.rows();
}

double Geometry2D::
sigma (const Eigen::ArrayXXd &hop)
{
	double res = 0.;
	
	Eigen::ArrayXd x(hop.rows()); x = 0;
	
	for (int i=0; i<hop.rows(); ++i)
	for (int j=i; j<hop.cols(); ++j)
	{
		if (abs(hop(i,j)) > ::mynumeric_limits<double>::epsilon()) x(i) += abs(j-i);
	}
	
	double avg = x.sum()/x.rows();
	double var = ((x-avg)*(x-avg)).sum()/x.rows();
	
	return sqrt(var);
}

double Geometry2D::
maxd (const Eigen::ArrayXXd &hop)
{
	double res = 0;
	
	for (int i=0; i<hop.rows(); ++i)
	for (int j=i; j<hop.cols(); ++j)
	{
		if (abs(hop(i,j)) > ::mynumeric_limits<double>::epsilon() and abs(j-i)>res) res = abs(j-i);
	}
	
	return res;
}

double Geometry2D::
maxval (const Eigen::ArrayXXd &hop)
{
	double res = std::numeric_limits<double>::lowest();
	
	for (int i=0; i<hop.rows(); ++i)
	for (int j=i; j<hop.cols(); ++j)
	{
		if (hop(i,j) > res) res = hop(i,j);
	}
	
	return res;
}

double Geometry2D::
minval (const Eigen::ArrayXXd &hop)
{
	double res = std::numeric_limits<double>::max();
	
	for (int i=0; i<hop.rows(); ++i)
	for (int j=i; j<hop.cols(); ++j)
	{
		if (hop(i,j) < res) res = hop(i,j);
	}
	
	return res;
}

string Geometry2D::
hoppingInfo (const Eigen::ArrayXXd &hop)
{
	stringstream ss;
	ss << "avgd=" << round(avgd(hop),2)
	   << ",σd=" << round(sigma(hop),2) 
	   << ",maxd=" << round(maxd(hop),2) 
	   << ",minv=" << minval(hop)
	   << ",maxv=" << maxval(hop);
	return ss.str();
}

string Geometry2D::
hoppingInfo (const Eigen::ArrayXXcd &hop)
{
	stringstream ss;
	ss << ",minRe=" << minval(hop.real())
	   << ",maxRe=" << maxval(hop.real())
	   << ",minIm=" << minval(hop.imag())
	   << ",maxIm=" << maxval(hop.imag());
	return ss.str();
}

#endif
