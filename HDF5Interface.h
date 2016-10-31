#ifndef HDF5INTERFACE
#define HDF5INTERFACE

#include <memory>
#include <assert.h>
#include <H5Cpp.h> // compile with -lhdf5 -lhdf5_cpp

#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <unsupported/Eigen/CXX11/Tensor>

#include <eigen3-hdf5.hpp>
// conversion into native types of HDF5
template<typename T> inline H5::PredType native_type() {}

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
public:
	HDF5Interface () {};
	HDF5Interface (std::string filename_input, FILE_ACCESS_MODE mode_input);
	// ~HDF5Interface();
	
	void switch_to(FILE_ACCESS_MODE mode_input);
	void create_group(std::string grp_name);
	void close();
	
	template<typename ScalarType> void save_scalar (ScalarType x, const char * setname);
	template<typename ScalarType> void load_scalar (const char * setname, ScalarType & x);

	template<typename ScalarType> void save_vector (const ScalarType * vec, const size_t size, const char * setname);
	template<typename ScalarType> void load_vector (const char * setname, ScalarType * vec[]);

	template<typename ScalarType> void save_matrix (const Eigen::Matrix<ScalarType,Eigen::Dynamic,Eigen::Dynamic> &mat, std::string setname, std::string grp_name="");
	template<typename ScalarType> void load_matrix (Eigen::Matrix<ScalarType,Eigen::Dynamic,Eigen::Dynamic> &mat, std::string setname, std::string grp_name="");

	template<typename ScalarType, size_t Nl> void save_tensor (const Eigen::Tensor<ScalarType,Nl> &ten, std::string setname);
	template<typename ScalarType, size_t Nl> void load_tensor (Eigen::Tensor<ScalarType,Nl> &ten, std::string setname);

	void save_char (std::string salvandum, const char * setname);
	void load_char (const char * setname, std::string &c);

	size_t get_vector_size (const char * setname);

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

// HDF5Interface::
// ~HDF5Interface()
// {
// 	delete file;
// }

inline void HDF5Interface::
switch_to (FILE_ACCESS_MODE mode_input)
{
	MODE = mode_input;
	if      (MODE == WRITE) { file = std::make_unique<H5::H5File>(filename.c_str(), H5F_ACC_TRUNC); }
	else if (MODE == READ)  { file = std::make_unique<H5::H5File>(filename.c_str(), H5F_ACC_RDONLY); }
	else if (MODE == REWRITE) { file = std::make_unique<H5::H5File>(filename.c_str(), H5F_ACC_RDWR); }
}

void HDF5Interface::
create_group(std::string grp_name)
{
	file->createGroup(grp_name.c_str());
}

void HDF5Interface::
close()
{
	file->close();
}

template<typename ScalarType>
void HDF5Interface::
save_matrix (const Eigen::Matrix<ScalarType,Eigen::Dynamic,Eigen::Dynamic> &mat, std::string setname, std::string grp_name)
{
	assert(MODE==WRITE or MODE==REWRITE);
	if (grp_name != "")
	{
		std::string fullPath = "/" + grp_name;
		H5::Group * g = new H5::Group(file->openGroup(fullPath.c_str()));
		EigenHDF5::save(*g, setname, mat);
		delete g;
		return;
	}
	EigenHDF5::save(*(this->file), setname, mat);
}

template<typename ScalarType>
void HDF5Interface::
load_matrix ( Eigen::Matrix<ScalarType,Eigen::Dynamic,Eigen::Dynamic>  &mat, std::string setname, std::string grp_name)
{
	assert(MODE==READ);
	if (grp_name != "")
	{
		std::string fullPath = "/" + grp_name;
		H5::Group * g = new H5::Group(file->openGroup(fullPath.c_str()));
		EigenHDF5::load(*g, setname, mat);
		delete g;
		return;
	}
	EigenHDF5::load(*(this->file), setname, mat);
}

template<typename ScalarType, size_t Nl>
void HDF5Interface::
save_tensor (const Eigen::Tensor<ScalarType,Nl> &ten, std::string setname)
{
	assert(MODE==WRITE);
	const std::size_t dimensions_size = Nl;
	std::array<size_t,Nl> tmp;
	
	const hsize_t dimensions[dimensions_size];
	for (size_t i=0; i<Nl; i++)
	{
		dimensions[i] = ten.dimension(i);
	}
	H5::DataSpace space(dimensions_size, dimensions);
	H5::IntType datatype(native_type<ScalarType>());

}

template<typename ScalarType,size_t Nl>
void HDF5Interface::
load_tensor (Eigen::Tensor<ScalarType,Nl>  &ten, std::string setname)
{
	assert(MODE==READ);
}

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
load_scalar (const char * setname, ScalarType & x)
{
	assert(MODE==READ);
	H5::DataSet dataset = file->openDataSet(setname);
	H5::DataSpace dataspace = dataset.getSpace();
	hsize_t length[] = {1};
	H5::DataSpace double_memspace(1,length);
	dataset.read(&x, native_type<ScalarType>(), double_memspace, dataspace);
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
save_scalar (ScalarType x, const char * setname)
{
	assert(MODE==WRITE or MODE==REWRITE);
	hsize_t length[] = {1};
	H5::DataSpace space(1,length);
	H5::IntType datatype(native_type<ScalarType>());
	H5::DataSet dataset = file->createDataSet(setname, datatype, space);
	ScalarType x_as_array[] = {x};
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
