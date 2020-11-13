#ifndef HDF5INTERFACE
#define HDF5INTERFACE

#include <memory>
#include <assert.h>
#include <filesystem>
#include <H5Cpp.h> // compile with -lhdf5 -lhdf5_cpp

#include <Eigen/Dense>
#include <Eigen/SparseCore>

#ifdef HDF5_WITH_TENSOR
#include <unsupported/Eigen/CXX11/Tensor>
#endif

// conversion into native types of HDF5
template<typename T> inline H5::PredType native_type() {return H5::PredType::NATIVE_DOUBLE;}

template<> inline H5::PredType native_type<int>() {return H5::PredType::NATIVE_INT;}
template<> inline H5::PredType native_type<double>(){return H5::PredType::NATIVE_DOUBLE;}
template<> inline H5::PredType native_type<unsigned long>() {return H5::PredType::NATIVE_ULONG;}
template<> inline H5::PredType native_type<long>() {return H5::PredType::NATIVE_LONG;}
template<> inline H5::PredType native_type<long long>() {return H5::PredType::NATIVE_LLONG;}
template<> inline H5::PredType native_type<unsigned long long>() {return H5::PredType::NATIVE_ULLONG;}
template<> inline H5::PredType native_type<unsigned>() {return H5::PredType::NATIVE_UINT;}
template<> inline H5::PredType native_type<short>() {return H5::PredType::NATIVE_SHORT;}
template<> inline H5::PredType native_type<unsigned short>() {return H5::PredType::NATIVE_USHORT;}
template<> inline H5::PredType native_type<float>() {return H5::PredType::NATIVE_FLOAT;}
template<> inline H5::PredType native_type<long double>() {return H5::PredType::NATIVE_LDOUBLE;}

template<> inline H5::PredType native_type<const char*>(){return H5::PredType::NATIVE_CHAR;}
template<> inline H5::PredType native_type<std::string>(){return H5::PredType::NATIVE_CHAR;}
template<> inline H5::PredType native_type<signed char>(){return H5::PredType::NATIVE_SCHAR;}
template<> inline H5::PredType native_type<unsigned char>(){return H5::PredType::NATIVE_UCHAR;}

enum FILE_ACCESS_MODE {READ, WRITE, REWRITE};

class HDF5Interface
{
	typedef Eigen::Index Index;
	#ifdef HDF5_WITH_TENSOR
	template<typename ScalarType, Index Nl> using TensorType = Eigen::Tensor<ScalarType,Nl,Eigen::ColMajor,Index>;
	#endif
	template<typename ScalarType> using MatrixType = Eigen::Matrix<ScalarType,Eigen::Dynamic,Eigen::Dynamic>;
	template<typename ScalarType> using VectorType = Eigen::Matrix<ScalarType,Eigen::Dynamic,1>;
	
public:
	HDF5Interface () {};
	HDF5Interface (std::string filename_input, FILE_ACCESS_MODE mode_input);
	/* ~HDF5Interface(); */
	
	static bool IS_VALID_HDF5(std::string filename)
	{
	  return H5::H5File::isHdf5(filename.c_str());
	};
	
	void switch_to(FILE_ACCESS_MODE mode_input);
	void close();
	
	void create_group(std::string grp_name);
	bool HAS_GROUP(std::string grp_name);
	
	std::vector<std::string> get_groups() const;
	
	template<typename ScalarType> void save_scalar (ScalarType x, std::string setname, std::string grp_name="");
	template<typename ScalarType> void load_scalar (ScalarType &x, std::string setname, std::string grp_name="");
	
	template<typename ScalarType> void save_vector (const ScalarType * vec, const size_t size, const char * setname);
	template<typename ScalarType> void load_vector (const char * setname, ScalarType * vec[]);
	
	template<typename ScalarType> void save_matrix (const MatrixType<ScalarType> &mat, std::string setname, std::string grp_name="");
	template<typename ScalarType> void load_matrix (MatrixType<ScalarType> &mat, std::string setname, std::string grp_name="");
	
	template<typename ScalarType> void save_vector (const VectorType<ScalarType> &vec, std::string setname, std::string grp_name="");
	template<typename ScalarType> void load_vector (VectorType<ScalarType> &vec, std::string setname, std::string grp_name="");
	
	#ifdef HDF5_WITH_TENSOR
	template<typename ScalarType, Index Nl> void save_tensor (const TensorType<ScalarType,Nl> &ten, std::string setname, std::string grp_name="");
	template<typename ScalarType, Index Nl> void load_tensor (TensorType<ScalarType,Nl> &ten, std::string setname);
	#endif
	
	void save_char (std::string salvandum, const char * setname);
	void load_char (const char * setname, std::string &c);
	
	std::size_t get_vector_size (const char * setname);
	
	bool CHECK (std::string dataset)
	{
		return H5Lexists(file->getId(), dataset.c_str(), H5P_DEFAULT) > 0;
	}
	
private:
	
	FILE_ACCESS_MODE MODE;
	std::string filename;
	std::unique_ptr<H5::H5File> file;
	
};

HDF5Interface::
HDF5Interface (std::string filename_input, FILE_ACCESS_MODE mode_input)
:filename(filename_input)
{
	switch_to(mode_input);
}

/* HDF5Interface:: */
/* ~HDF5Interface() */
/* { */
/*         file.reset(); */
/* } */

inline void HDF5Interface::
switch_to (FILE_ACCESS_MODE mode_input)
{
	MODE = mode_input;
	if      (MODE == WRITE) { file = std::make_unique<H5::H5File>(filename.c_str(), H5F_ACC_TRUNC); }
	else if (MODE == READ)  { file = std::make_unique<H5::H5File>(filename.c_str(), H5F_ACC_RDONLY); }
	else if (MODE == REWRITE) { file = std::make_unique<H5::H5File>(filename.c_str(), H5F_ACC_RDWR); }
}

void HDF5Interface::
close()
{
	file->close();
}

void HDF5Interface::
create_group (std::string grp_name)
{
	if (!HAS_GROUP(grp_name))
	{
		try {file->createGroup(grp_name.c_str());}
		catch(...) {}
	}
}

bool HDF5Interface::
HAS_GROUP (std::string grp_name)
{
//	bool out = true;
//	try {file->openGroup(grp_name.c_str());}
//	catch(...) {out = false;}
//	return out;
	return H5Lexists(file->getId(), grp_name.c_str(), H5P_DEFAULT) > 0;
}

std::vector<std::string> HDF5Interface::
get_groups () const
{
        hsize_t nobjs = file->getNumObjs();
        std::vector<std::string> out(nobjs);

        H5std_string obj_name;
        for (std::size_t i = 0; i < nobjs; i++)
                {
                        out[i] = file->getObjnameByIdx(i);
                }
        return out;
}

template<typename ScalarType>
void HDF5Interface::
save_matrix (const Eigen::Matrix<ScalarType,Eigen::Dynamic,Eigen::Dynamic> &mat, std::string setname, std::string grp_name)
{
	assert(MODE==WRITE or MODE==REWRITE);
	//Need to switch the layout here, because HDF5 uses RowMajor.
	Eigen::Matrix<ScalarType,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> switch_to_row = mat;
	constexpr std::size_t dimensions_size = 2ul;
//	std::array<Index,dimensions_size> tmp; // not used??
	
	hsize_t dimensions[dimensions_size]; dimensions[0] = mat.rows(); dimensions[1] = mat.cols();
	H5::DataSpace space(dimensions_size, dimensions);
	H5::IntType datatype(native_type<ScalarType>());
	
	H5::DataSet dataset;
	if (grp_name != "")
	{
		std::string fullPath = "/" + grp_name;
		H5::Group * g = new H5::Group(file->openGroup(fullPath.c_str()));
		dataset = g->createDataSet(setname.c_str(), datatype, space);
		delete g;
	}
	else
	{
		dataset = file->createDataSet(setname.c_str(), datatype, space);
	}
	dataset.write(switch_to_row.data(), native_type<ScalarType>());
}

template<typename ScalarType>
void HDF5Interface::
load_matrix (Eigen::Matrix<ScalarType,Eigen::Dynamic,Eigen::Dynamic>  &mat, std::string setname, std::string grp_name)
{
	assert(MODE==READ);
	H5::DataSet dataset;
	if (grp_name != "")
	{
		std::string fullPath = "/" + grp_name;
		H5::Group * g = new H5::Group(file->openGroup(fullPath.c_str()));
		dataset = g->openDataSet(setname);
		delete g;
	}
	else
	{
		dataset = file->openDataSet(setname);
	}
	H5::DataSpace dataspace = dataset.getSpace();
	
	constexpr std::size_t dimensions_size = static_cast<std::size_t>(2);
	hsize_t dimensions[dimensions_size];
	int ndims = dataspace.getSimpleExtentDims( dimensions, NULL);
	H5::DataSpace memspace(2,dimensions);
	
	//Need to use a Rowmajor matrix here and convert afterwards, because HDF5 us RowMajor storage order.
	Eigen::Matrix<ScalarType,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> temp(dimensions[0],dimensions[1]);
	
	dataset.read(temp.data(), native_type<ScalarType>(), memspace, dataspace);
	
	mat = temp;
}

template<typename ScalarType>
void HDF5Interface::
save_vector (const Eigen::Matrix<ScalarType,Eigen::Dynamic,1> &vec, std::string setname, std::string grp_name)
{
	assert(MODE==WRITE or MODE==REWRITE);
	constexpr std::size_t dimensions_size = 1ul;
	
	hsize_t dimensions[dimensions_size]; dimensions[0] = vec.rows();
	H5::DataSpace space(dimensions_size, dimensions);
	H5::IntType datatype(native_type<ScalarType>());
	
	H5::DataSet dataset;
	if (grp_name != "")
	{
		std::string fullPath = "/" + grp_name;
		H5::Group * g = new H5::Group(file->openGroup(fullPath.c_str()));
		dataset = g->createDataSet(setname.c_str(), datatype, space);
		delete g;
	}
	else
	{
		dataset = file->createDataSet(setname.c_str(), datatype, space);
	}
	dataset.write(vec.data(), native_type<ScalarType>());
}

template<typename ScalarType>
void HDF5Interface::
load_vector (Eigen::Matrix<ScalarType,Eigen::Dynamic,1>  &vec, std::string setname, std::string grp_name)
{
	assert(MODE==READ);
	H5::DataSet dataset;
	if (grp_name != "")
	{
		std::string fullPath = "/" + grp_name;
		H5::Group * g = new H5::Group(file->openGroup(fullPath.c_str()));
		dataset = g->openDataSet(setname);
		delete g;
	}
	else
	{
		dataset = file->openDataSet(setname);
	}
	H5::DataSpace dataspace = dataset.getSpace();
	
	constexpr std::size_t dimensions_size = 1ul;
	hsize_t dimensions[dimensions_size];
	int ndims = dataspace.getSimpleExtentDims(dimensions, NULL);
	H5::DataSpace memspace(1,dimensions);
	
	Eigen::Matrix<ScalarType,Eigen::Dynamic,1> temp(dimensions[0]);
	
	dataset.read(temp.data(), native_type<ScalarType>(), memspace, dataspace);
	
	vec = temp;
}

#ifdef HDF5_WITH_TENSOR
template<typename ScalarType, Eigen::Index Nl>
void HDF5Interface::
save_tensor (const TensorType<ScalarType,Nl> &ten, std::string setname, std::string grp_name)
{
	assert(MODE==WRITE);
	
	//Need to switch the layout here, because HDF5 uses RowMajor.
	std::array<Index,Nl> shuffle_dims;
	Index n=Nl-1;
	std::generate(shuffle_dims.begin(),shuffle_dims.end(),[&n]{ return n--; });
	Eigen::Tensor<ScalarType,Nl,Eigen::RowMajor,Index> switch_to_row = ten.swap_layout().shuffle(shuffle_dims); 

	constexpr std::size_t dimensions_size = static_cast<std::size_t>(Nl);
//	std::array<Index,dimensions_size> tmp; // not used??
	
	hsize_t dimensions[dimensions_size];
	for (std::size_t i=0; i<Nl; i++)
	{
		dimensions[i] = ten.dimension(i);
	}
	H5::DataSpace space(dimensions_size, dimensions);
	H5::IntType datatype(native_type<ScalarType>());

	H5::DataSet dataset;
	if (grp_name != "")
	{
		std::string fullPath = "/" + grp_name;
		H5::Group * g = new H5::Group(file->openGroup(fullPath.c_str()));
		dataset = g->createDataSet(setname.c_str(), datatype, space);
		delete g;
	}
	else
	{
		dataset = file->createDataSet(setname.c_str(), datatype, space);
	}
	dataset.write(switch_to_row.data(), native_type<ScalarType>());
}

template<typename ScalarType,Eigen::Index Nl>
void HDF5Interface::
load_tensor (TensorType<ScalarType,Nl>  &ten, std::string setname)
{
	assert(MODE==READ);

	H5::DataSet dataset = file->openDataSet(setname);
	H5::DataSpace dataspace = dataset.getSpace();

	constexpr std::size_t dimensions_size = static_cast<std::size_t>(Nl);	
	hsize_t dimensions[dimensions_size];
	int ndims = dataspace.getSimpleExtentDims( dimensions, NULL);
	H5::DataSpace memspace(Nl,dimensions);

	std::array<Index,Nl> dims;
	for (std::size_t i=0; i<Nl; i++)
	{
		dims[i] = static_cast<Index>(dimensions[i]);
	}

	//Need to use a Rowmajor tensor here and convert afterwards, because HDF5 us RowMajor storage order.
	Eigen::Tensor<ScalarType,Nl,Eigen::RowMajor,Index> temp(dims);
	dataset.read(temp.data(), native_type<ScalarType>(), memspace, dataspace);

	std::array<Index,Nl> shuffle_dims;
	Index n=Nl-1;
	std::generate(shuffle_dims.begin(),shuffle_dims.end(),[&n]{ return n--; });
	ten = temp.swap_layout().shuffle(shuffle_dims);
}
#endif

template<typename ScalarType>
void HDF5Interface::
save_vector (const ScalarType * vec, const size_t size, const char * setname)
{
	// write uncompressed
	assert(MODE==WRITE);
	hsize_t length[] = {size};
	H5::DataSpace space(1,length);
	H5::IntType datatype(native_type<ScalarType>());
	H5::DataSet dataset = file->createDataSet(setname, datatype, space);
	dataset.write(vec, native_type<ScalarType>());

	// write compressed
	/*hsize_t cdims[1];
	cdims[0] = (int)sqrt(size);
	H5::DSetCreatPropList ds_creatplist;
	ds_creatplist.setChunk(1,cdims);
	ds_creatplist.setDeflate(6);
	
	hsize_t length[] = {size};
	H5::DataSpace space(1,length);
	H5::IntType datatype(H5::PredType::NATIVE_DOUBLE);
	H5::DataSet dataset = file->createDataSet(setname, datatype, space, ds_creatplist);
	dataset.write(vec, H5::PredType::NATIVE_DOUBLE);*/
	
	// write autocompressed for large file sizes
	/*assert(MODE==WRITE);
	hsize_t length[] = {size};
	H5::DataSpace space(1,length);
	H5::IntType datatype(H5::PredType::NATIVE_DOUBLE);
	H5::DataSet dataset;
	
	if (size<65536) // don't compress below 512kB
	{
		dataset = file->createDataSet(setname, datatype, space);
	}
	else
	{
		hsize_t cdims[1];
		cdims[0] = max(1024,(int)sqrt(size)); // set chunk size
		H5::DSetCreatPropList ds_creatplist;
		ds_creatplist.setChunk(1,cdims);
		ds_creatplist.setDeflate(6);
		dataset = file->createDataSet(setname, datatype, space, ds_creatplist);
	}
	dataset.write(vec, H5::PredType::NATIVE_DOUBLE);*/
}

template<typename ScalarType>
void HDF5Interface::
load_vector (const char * setname, ScalarType * vec[])
{
	assert(MODE==READ);
	H5::DataSet dataset = file->openDataSet(setname);
	H5::DataSpace dataspace = dataset.getSpace();
	hsize_t length[1];
	int ndims = dataspace.getSimpleExtentDims(length,NULL);
	H5::DataSpace memspace(1,length);
	dataset.read(*vec, native_type<ScalarType>(), memspace, dataspace);
}

template<typename ScalarType>
void HDF5Interface::
load_scalar (ScalarType &x, std::string setname, std::string grp_name)
{
	assert(MODE==READ);
	H5::DataSet dataset;
	if (grp_name != "")
	{
		std::string fullPath = "/" + grp_name;
		H5::Group * g = new H5::Group(file->openGroup(fullPath.c_str()));
		dataset = g->openDataSet(setname);
		delete g;
	}
	else
	{
		dataset = file->openDataSet(setname);
	}
	
	H5::DataSpace dataspace = dataset.getSpace();
	hsize_t length[] = {1};
	H5::DataSpace double_memspace(1,length);
	ScalarType tmp;
	dataset.read(&tmp, native_type<ScalarType>(), double_memspace, dataspace);
	x = tmp;
}

size_t HDF5Interface::
get_vector_size (const char * setname)
{
	assert(MODE==READ);
	H5::DataSet dataset = file->openDataSet(setname);
	H5::DataSpace dataspace = dataset.getSpace();
	hsize_t length[1];
	int ndims = dataspace.getSimpleExtentDims(length,NULL);
	return length[0];
}

template<typename ScalarType>
void HDF5Interface::
save_scalar (ScalarType x, std::string setname, std::string grp_name)
{
	assert(MODE==WRITE or MODE==REWRITE);
	hsize_t length[] = {1};
	H5::DataSpace space(1,length);
	H5::IntType datatype(native_type<ScalarType>());
	ScalarType x_as_array[] = {x};
	
	H5::DataSet dataset;
	if (grp_name != "")
	{
		std::string fullPath = "/" + grp_name;
		H5::Group * g = new H5::Group(file->openGroup(fullPath.c_str()));
		dataset = g->createDataSet(setname.c_str(), datatype, space);
		delete g;
	}
	else
	{
		dataset = file->createDataSet(setname.c_str(), datatype, space);
	}
	dataset.write(x_as_array, native_type<ScalarType>());
}

void HDF5Interface::
save_char (std::string salvandum, const char * setname)
{
	assert(MODE==WRITE or MODE==REWRITE);
	hsize_t length[] = {1};
	H5::DataSpace space(1,length);
	H5::StrType datatype(0,H5T_VARIABLE);
	H5::DataSet dataset = file->createDataSet(setname, datatype, space);
	const char * this_sucks_hairy_balls_but_it_works[1] = {0};
	this_sucks_hairy_balls_but_it_works[0] = salvandum.c_str();
	dataset.write((void*)this_sucks_hairy_balls_but_it_works, datatype);
}

void HDF5Interface::
load_char (const char * setname, std::string &c)
{
	assert(MODE==READ);
	H5::DataSet dataset = file->openDataSet(setname);
	H5::DataType datatype = dataset.getDataType();
	//H5::DataSpace dataspace = dataset.getSpace();
	//hsize_t length[] = {1};
	//H5::DataSpace double_memspace(1,length);
	const char * this_sucks_hairy_balls_but_it_works[1] = {0};
	dataset.read((void*)this_sucks_hairy_balls_but_it_works, datatype);
	c = this_sucks_hairy_balls_but_it_works[0];
}

#endif
