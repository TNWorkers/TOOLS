#ifndef GEOMETRY2D
#define GEOMETRY2D

enum TRAVERSE2D {CHESSBOARD, SNAKE};

class Geometry2D
{
public:
	
	/**constant coupling λ*/
	Geometry2D (TRAVERSE2D path_input, int Lx_input, int Ly_input, double lambda=1., bool PERIODIC_Y=false);
	
	/**constant couplings λx and λy*/
	Geometry2D (TRAVERSE2D path_input, int Lx_input, int Ly_input, double lambda_x, double lambda_y, bool PERIODIC_Y=false);
	
	/**arbitrary couplings (coupling_y is of size Ly+1 and the last element sets the boundary condition)*/
	Geometry2D (TRAVERSE2D path_input, int Lx_input, int Ly_input, const ArrayXd coupling_x, const ArrayXd coupling_y);
	
	/**return hopping matrix*/
	ArrayXXd hopping() const {return HoppingMatrix;}
	
	/**return hopping matrix*/
	static vector<vector<std::pair<size_t,double> > > rangeFormat (const ArrayXXd &hop);
	
	/**access x,y(index)*/
	inline pair<int,int> operator() (int i)        const {return coord.at(i);}
	
	/**access index(x,y)*/
	inline int           operator() (int i, int j) const {return index.at(make_pair(i,j));}
	
	/**all x coordinates at a given iy*/
	ArrayXd x_row (int iy) const;
	
	/**average distance of the hopping matrix*/
	static double avgd (const ArrayXXd &hop);
	
	/**standard deviation of the distance in the hopping matrix*/
	static double sigma (const ArrayXXd &hop);
	
	/**maximum hopping distance of the hopping matrix*/
	static double maxd (const ArrayXXd &hop);
	
	/**maximum value of the hopping matrix*/
	static double maxval (const ArrayXXd &hop);
	
	/**minimum value of the hopping matrix*/
	static double minval (const ArrayXXd &hop);
	
	static string hoppingInfo (const ArrayXXd &hop);
	
	/**Coefficients for the Fourier transform in y-direction.*/
	vector<complex<double> > FTy_phases (int ix_fixed, int iky, bool PARITY) const;
	
private:
	
	TRAVERSE2D path;
	int Lx=1, Ly=1;
	
	ArrayXXd HoppingMatrix;
	
	map<pair<int,int>,int> index;
	map<int,pair<int,int>> coord;
	
	void fill_HoppingMatrix (const ArrayXd coupling_x, const ArrayXd coupling_y);
};

Geometry2D::
Geometry2D(TRAVERSE2D path_input, int Lx_input, int Ly_input, double lambda, bool PERIODIC_Y)
:path(path_input), Lx(Lx_input), Ly(Ly_input)
{
	ArrayXd coupling_x(Lx);
	ArrayXd coupling_y;
	
	if (PERIODIC_Y)
	{
		coupling_y.resize(Ly+1);
	}
	else
	{
		coupling_y.resize(Ly);
	}
	
	coupling_x = lambda;
	coupling_y = lambda;
	
	fill_HoppingMatrix(coupling_x,coupling_y);
}

Geometry2D::
Geometry2D(TRAVERSE2D path_input, int Lx_input, int Ly_input, double lambda_x, double lambda_y, bool PERIODIC_Y)
:path(path_input), Lx(Lx_input), Ly(Ly_input)
{
	ArrayXd coupling_x(Lx);
	ArrayXd coupling_y;
	
	if (PERIODIC_Y)
	{
		coupling_y.resize(Ly+1);
	}
	else
	{
		coupling_y.resize(Ly);
	}
	
	coupling_x = lambda_x;
	coupling_y = lambda_y;
	
	fill_HoppingMatrix(coupling_x,coupling_y);
}

Geometry2D::
Geometry2D(TRAVERSE2D path_input, int Lx_input, int Ly_input, const ArrayXd coupling_x, const ArrayXd coupling_y)
:path(path_input), Lx(Lx_input), Ly(Ly_input)
{
	fill_HoppingMatrix(coupling_x,coupling_y);
}

void Geometry2D::
fill_HoppingMatrix (const ArrayXd coupling_x, const ArrayXd coupling_y)
{
	if (Lx==1) {assert(path != SNAKE and "Must use Lx>=2 with the SNAKE geometry!");}
	
	HoppingMatrix.resize(Lx*Ly,Lx*Ly); HoppingMatrix.setZero();
	
	// Mirrors the y coordinate to create a snake.
	auto mirror = [this] (int iy) -> int
	{
		vector<int> v(Ly);
		iota (begin(v),end(v),0);
		reverse(v.begin(),v.end());
		return v[iy];
	};
	
	for (int ix=0; ix<Lx; ++ix)
	for (int iy=0; iy<Ly; ++iy)
	for (int jx=0; jx<Lx; ++jx)
	for (int jy=0; jy<Ly; ++jy)
	{
		int iy_=iy, jy_=jy;
		if (path == SNAKE)
		{
			// mirror even y only
			iy_ = (ix%2==0)? iy : mirror(iy);
			jy_ = (jx%2==0)? jy : mirror(jy);
		}
		// the index is calculated normally:
		int index_i = iy+Ly*ix;
		int index_j = jy+Ly*jx;
		
		// but is stored together with the mirrored y_:
		if (jx == 0 and jy == 0)
		{
			index[make_pair(ix,iy_)] = index_i;
			coord[index_i] = make_pair(ix,iy_);
//			cout << "ix=" << ix << ", iy_=" << iy_ << ", index_i=" << index_i << endl;
		}
		
		if (abs(ix-jx) == 1 and (iy_==jy_))
		{
			HoppingMatrix(index_i,index_j) += coupling_x(ix);
		}
		else if (abs(iy_-jy_) == 1 and (ix==jx))
		{
			HoppingMatrix(index_i,index_j) += coupling_y(iy_);
		}
		else if (abs(iy_-jy_) == Ly-1 and (ix==jx) and Ly>2)
		{
			HoppingMatrix(index_i,index_j) += coupling_y(Ly);
		}
	}
}

vector<vector<std::pair<size_t,double> > > Geometry2D::
rangeFormat (const ArrayXXd &hop)
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

ArrayXd Geometry2D::
x_row (int iy) const
{
	ArrayXd out(Lx);
	for (int ix=0; ix<Lx; ++ix)
	{
		out(ix) = index.at(make_pair(ix,iy));
	}
	return out;
}

vector<complex<double> > Geometry2D::
FTy_phases (int x, int iky, bool PARITY) const
{
	vector<complex<double> > out(Lx*Ly);
	for (int l=0; l<Lx*Ly; ++l)
	{
		out[l] = 0.;
	}
	
	double sign = pow(-1.,PARITY);
	double ky = iky * 2.*M_PI/Ly;
	
//	cout << "iky=" << iky << ", ky=" << ky << endl;
	
	for (int y=0; y<Ly; ++y)
	{
		int i = index.at(make_pair(x,y));
		
		out[i] = exp(sign*1.i*ky*static_cast<double>(y)) / sqrt(Ly);
		
//		cout << "ky=" << ky << ", x=" << x << ", y=" << y << ", i=" << i << ", index.at(make_pair(x,y))=" << index.at(make_pair(x,y)) << ", val=" << out[i] << endl;
	}
	return out;
}

double Geometry2D::
avgd (const ArrayXXd &hop)
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
sigma (const ArrayXXd &hop)
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
maxd (const ArrayXXd &hop)
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
maxval (const ArrayXXd &hop)
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
minval (const ArrayXXd &hop)
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
hoppingInfo (const ArrayXXd &hop)
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
