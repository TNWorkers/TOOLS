#ifndef HDF5EIGEN
#define HDF5EIGEN

#include <Eigen/Dense>
#include <Eigen/Sparse>
using namespace Eigen;
#include "HDF5Interface.h"
#include "Hilbert_typedefs.h"

void save_SparseMatrixXd (const Eigen::SparseMatrix<double> &M, std::string filename)
{
	HDF5Interface Writer(filename,WRITE);
	Writer.save_scalar(1,"SparseMatrixXd");
	Writer.save_scalar(M.nonZeros(),"nonZeros");
	Writer.save_scalar(M.rows(),"rows");
	Writer.save_scalar(M.cols(),"cols");
	Writer.save_vector(M.valuePtr(), M.nonZeros(), "values");
	
	int * row_array = (int*) malloc (M.nonZeros() * sizeof * row_array);
	int * col_array = (int*) malloc (M.nonZeros() * sizeof * col_array);

	int i=0;
	for (int k=0; k<M.outerSize(); ++k)
	for (SparseMatrixXd::InnerIterator Iit(M,k); Iit; ++Iit)
	{
		row_array[i] = Iit.row();
		col_array[i] = Iit.col();
		++i;
	}
	
	Writer.save_vector(row_array, M.nonZeros(), "index_row");
	Writer.save_vector(col_array, M.nonZeros(), "index_col");
	
	cout << "SparseMatrixXd saved to file " << filename << "!" << endl;
}

Eigen::SparseMatrix<double> load_SparseMatrixXd (std::string filename)
{
	HDF5Interface Reader(filename,READ);
	int rows, cols, nonZeros, SMdatatype;
	
	Reader.load_scalar("SparseMatrixXd",SMdatatype);
	assert(SMdatatype==1);
	
	Reader.load_scalar("rows",rows);
	Reader.load_scalar("cols",cols);
	Reader.load_scalar("nonZeros",nonZeros);
	
	int * row_read = (int*) malloc (nonZeros * sizeof * row_read);
	int * col_read = (int*) malloc (nonZeros * sizeof * col_read);
	double * val_read = (double*) malloc (nonZeros * sizeof * val_read);
	
	Reader.load_vector("index_row", &row_read);
	Reader.load_vector("index_col", &col_read);
	Reader.load_vector("values",    &val_read);
	
	typedef Eigen::Triplet<double> TripletList;
	std::vector<TripletList> T;
	T.reserve(nonZeros);
	for(int i=0; i<nonZeros; ++i) {T.push_back(TripletList(row_read[i], col_read[i], val_read[i]));}
	
	Eigen::SparseMatrix<double> Mout(rows,cols);
	Mout.setFromTriplets(T.begin(),T.end());
	return Mout;
}

enum EIGEN_TYPE_WRAPPER {VECTORXD, MATRIXXD, VECTORXCD, MATRIXXCD};
enum SCALAR_TYPE_WRAPPER {REAL, COMPLEX};
enum EIGEN_SAVE_FORMAT {SINGLE_SET=0, COLWISE=1};

template<typename T> inline EIGEN_TYPE_WRAPPER determine_EigenType(){}
template<>           inline EIGEN_TYPE_WRAPPER determine_EigenType<VectorXd> () {return VECTORXD;}
template<>           inline EIGEN_TYPE_WRAPPER determine_EigenType<MatrixXd> () {return MATRIXXD;}
template<>           inline EIGEN_TYPE_WRAPPER determine_EigenType<VectorXcd>() {return VECTORXCD;}
template<>           inline EIGEN_TYPE_WRAPPER determine_EigenType<MatrixXcd>() {return MATRIXXCD;}

template<typename EigenType>
void save_DenseMatrix (const EigenType &M, std::string filename, EIGEN_SAVE_FORMAT saveformat_input=SINGLE_SET)
{
	HDF5Interface Writer(filename,WRITE);
	
	Writer.save_scalar(M.rows(),"rows");
	Writer.save_scalar(M.cols(),"cols");
	Writer.save_scalar((int)saveformat_input,"colwise?");
	
	int size_factor=1;
	if      (determine_EigenType<EigenType>()==VECTORXD)  {Writer.save_scalar(1,"VectorXd");}
	else if (determine_EigenType<EigenType>()==MATRIXXD)  {Writer.save_scalar(1,"MatrixXd");}
	else if (determine_EigenType<EigenType>()==VECTORXCD) {Writer.save_scalar(1,"VectorXcd"); size_factor=2;}
	else if (determine_EigenType<EigenType>()==MATRIXXCD) {Writer.save_scalar(1,"MatrixXcd"); size_factor=2;}
	
	if (saveformat_input==SINGLE_SET)
	{
		Writer.save_vector((double*)M.data(), M.rows()*M.cols()*size_factor, "data");
	}
	else if (saveformat_input==COLWISE)
	{
		for (size_t i=0; i<M.cols(); ++i)
		{
			ostringstream strs; strs << "data_col_" << i;
			const char * dataset_name = strs.str().c_str();
			Writer.save_vector((double*)M.col(i).data(), M.rows()*size_factor, dataset_name);
		}
	}
}

template<typename EigenType>
void load_Eigen (std::string filename, EigenType &M)
{
	HDF5Interface Reader(filename,READ);
	size_t rows, cols;
	int datatype, format;
	Reader.load_scalar("rows",rows);
	Reader.load_scalar("cols",cols);

	// indirect check of correct datatype
	if      (determine_EigenType<EigenType>()==VECTORXD)  {Reader.load_scalar("VectorXd", datatype);}
	else if (determine_EigenType<EigenType>()==MATRIXXD)  {Reader.load_scalar("MatrixXd", datatype);}
	else if (determine_EigenType<EigenType>()==VECTORXCD) {Reader.load_scalar("VectorXcd",datatype);}
	else if (determine_EigenType<EigenType>()==MATRIXXCD) {Reader.load_scalar("MatrixXcd",datatype);}
	else {throw "Could not determine Eigen type in HDF5 file!";}
	
	Reader.load_scalar("colwise?",format);
	M.resize(rows,cols);
	
	if (format==(int)SINGLE_SET)
	{
		double * p = (double*)M.data();
		Reader.load_vector("data",&p);
	}
	else if (format==(int)COLWISE)
	{
		for (size_t i=0; i<M.cols(); ++i)
		{
			double * p = (double*)M.col(i).data();
			ostringstream strs; strs << "data_col_" << i;
			const char * dataset_name = strs.str().c_str();
			Reader.load_vector(dataset_name,&p);
		}
	}
}

#endif
