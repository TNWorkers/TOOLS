#ifndef MEMCALC
#define MEMCALC

#include <map>

#include <Eigen/Dense>
#include <Eigen/SparseCore>
//#include <unsupported/Eigen/CXX11/Tensor>


enum MEMUNIT {kB, MB, GB}; // "b" is ambiguous, hence "byte"

std::map<MEMUNIT,double> memUnitVal = {{kB,1024.}, {MB,1048576.}, {GB,1073741824.}};

// dense vector
template<typename Scalar>
inline double calc_memory (const Eigen::Matrix<Scalar,Eigen::Dynamic,1> &V, MEMUNIT memunit_input=GB)
{
	return V.rows() * sizeof(Scalar) / memUnitVal[memunit_input];
}

// dense matrix
template<typename Scalar>
inline double calc_memory (const Eigen::Matrix<Scalar,Eigen::Dynamic,Eigen::Dynamic> &M, MEMUNIT memunit_input=GB)
{
	return M.rows() * M.cols() * sizeof(Scalar) / memUnitVal[memunit_input];
}

//// dense tensor
//template<typename Scalar, int Rank>
//inline double calc_memoryT (const Eigen::Tensor<Scalar,Rank,Eigen::ColMajor,Eigen::Index> &T, MEMUNIT memunit_input=GB)
//{
//	double out=Scalar(1.);
//	for (std::size_t leg=0; leg<Rank; leg++)
//	{
//		out *= T.dimension(leg);
//	}
//	return out * sizeof(Scalar) / memUnitVal[memunit_input];
//}

// N scalars
template<typename Scalar>
inline double calc_memory (std::size_t N, MEMUNIT memunit_input=GB)
{
	return N * sizeof(Scalar) / memUnitVal[memunit_input];
}

// sparse matrix
template<typename Scalar, int Options, typename Index>
inline double calc_memory (const Eigen::SparseMatrix<Scalar,Options,Index> &M, MEMUNIT memunit_input=GB)
{
	return (M.nonZeros()*sizeof(Scalar) + (M.nonZeros()+M.outerSize()+1.)*sizeof(Index)) / memUnitVal[memunit_input];
}

// sparse vector
template<typename Scalar, typename Index>
inline double calc_memory (const Eigen::SparseVector<Scalar,0,Index> &V, MEMUNIT memunit_input=GB)
{
	return V.nonZeros() * (sizeof(Scalar)+sizeof(Index)) / memUnitVal[memunit_input];
}

#endif
