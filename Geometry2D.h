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
	Geometry2D (const Lattice2D &lattice_input, TRAVERSE2D path_input, double lambda=1.);
	
	// /**constant couplings λx and λy*/
	// Geometry2D (const Lattice<2> &lattice_input, TRAVERSE2D path_input, int Lx_input, int Ly_input, double lambda_x, double lambda_y, bool PERIODIC_Y=false);
	
	// /**arbitrary couplings (coupling_y is of size Ly+1 and the last element sets the boundary condition)*/
	// Geometry2D (const Lattice<2> &lattice_in, TRAVERSE2D path_input, int Lx_input, int Ly_input, const ArrayXd coupling_x, const ArrayXd coupling_y);
	
	/**return hopping matrix*/
	Eigen::ArrayXXd hopping() const {return HoppingMatrix;}
	
	/**return hopping matrix*/
	static vector<vector<std::pair<size_t,double> > > rangeFormat (const Eigen::ArrayXXd &hop);
	
	/**access x,y(index)*/
	inline pair<int,int> operator() (int i)        const {return coord.at(i);}
	
	/**access index(x,y)*/
	inline int           operator() (int i, int j) const {return index.at(make_pair(i,j));}
	
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
	
	/**Coefficients for the Fourier transform in y-direction.*/
	vector<complex<double> > FTy_phases (int ix_fixed, int iky, bool PARITY) const;
	
private:
	
	TRAVERSE2D path;
	
	Eigen::ArrayXXd HoppingMatrix;
	
	map<pair<int,int>,int> index;
	map<int,pair<int,int>> coord;
	
	void fill_HoppingMatrix ();

	Lattice2D lattice;

	double coupling_neighbor;
};

Geometry2D::
Geometry2D(const Lattice2D &lattice_input, TRAVERSE2D path_input, double lambda)
	:path(path_input), coupling_neighbor(lambda), lattice(lattice_input)
{
	fill_HoppingMatrix();
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
fill_HoppingMatrix ()
{
	if (lattice.size(0)==1) {assert(path != SNAKE and "Must use Lx>=2 with the SNAKE geometry!");}
	
	HoppingMatrix.resize(lattice.volume(),lattice.volume()); HoppingMatrix.setZero();
	
	// Mirrors the y coordinate to create a snake.
	auto mirror = [this] (int iy) -> int
	{
		vector<int> v(lattice.size(1));
		std::iota (begin(v),end(v),0);
		reverse(v.begin(),v.end());
		return v[iy];
	};
	
	for (int ix=0; ix<lattice.size(0); ++ix)
	for (int iy=0; iy<lattice.size(1); ++iy)
	for (int jx=0; jx<lattice.size(0); ++jx)
	for (int jy=0; jy<lattice.size(1); ++jy)
	{
		int iy_=iy, jy_=jy;
		if (path == SNAKE)
		{
			// mirror even y only
			iy_ = (ix%2==0)? iy : mirror(iy);
			jy_ = (jx%2==0)? jy : mirror(jy);
		}
		// the index is calculated normally:
		int index_i = iy+lattice.size(1)*ix;
		int index_j = jy+lattice.size(1)*jx;
		
		// but is stored together with the mirrored y_:
		if (jx == 0 and jy == 0)
		{
			index[make_pair(ix,iy_)] = index_i;
			coord[index_i] = make_pair(ix,iy_);
//			cout << "ix=" << ix << ", iy_=" << iy_ << ", index_i=" << index_i << endl;
		}
		
		if (lattice.ARE_NEIGHBORS( {ix,iy},{jx,jy} ))
		{
			HoppingMatrix(index_i,index_j) += coupling_neighbor;
		}
		

		// if (abs(ix-jx) == 1 and (iy_==jy_))
		// {
		// 	HoppingMatrix(index_i,index_j) += coupling_x(ix);
		// }
		// else if (abs(iy_-jy_) == 1 and (ix==jx))
		// {
		// 	HoppingMatrix(index_i,index_j) += coupling_y(iy_);
		// }
		// else if (TRIANGULAR and ((ix-jx == 1 and iy_-jy_ == -1) or (ix-jx == -1 and iy_-jy_ == 1)))
		// {
		// 	HoppingMatrix(index_i,index_j) += coupling_triangular;
		// }
		// // else if (abs(iy_-jy_) == Ly-1 and (ix==jx) and Ly>2)
		// // {
		// // 	HoppingMatrix(index_i,index_j) += coupling_y(Ly);
		// // }
		// else if (TRIANGULAR and Ly>2 and ( (iy_-jy_ == Ly-1 and (ix-jx == -static_cast<int>(Ly/2)+1 or ix-jx == -static_cast<int>(Ly/2))) or (iy_-jy_ == 1-Ly and (ix-jx == static_cast<int>(Ly/2)-1 or ix-jx == static_cast<int>(Ly/2))) )  )
		// {
		// 	HoppingMatrix(index_i,index_j) += coupling_triangular;
		// }

	}
}

vector<vector<std::pair<size_t,double> > > Geometry2D::
rangeFormat (const Eigen::ArrayXXd &hop)
{
	vector<vector<std::pair<size_t,double> > > out(hop.rows());
	
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
	Eigen::ArrayXd out(lattice.size(0));
	for (int ix=0; ix<lattice.size(0); ++ix)
	{
		out(ix) = index.at(make_pair(ix,iy));
	}
	return out;
}

vector<complex<double> > Geometry2D::
FTy_phases (int x, int iky, bool PARITY) const
{
	vector<complex<double> > out(lattice.volume());
	for (int l=0; l<lattice.volume(); ++l)
	{
		out[l] = 0.;
	}
	
	double sign = pow(-1.,PARITY);
	double ky = iky * 2.*M_PI/lattice.size(1);
	
//	cout << "iky=" << iky << ", ky=" << ky << endl;
	
	for (int y=0; y<lattice.size(1); ++y)
	{
		int i = index.at(make_pair(x,y));
		
		out[i] = exp(sign*1.i*ky*static_cast<double>(y)) / sqrt(lattice.size(0));
		
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

#endif
