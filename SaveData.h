#ifndef SAVEDATA
#define SAVEDATA

#include <map>
#include <initializer_list>

#include <Eigen/Dense>

struct SaveData
{
	SaveData(){};
	
	void add_integers (std::initializer_list<std::string> labels)
	{
		for (const auto &label:labels)
		{
			intg.insert(std::pair<std::string,int>(label,0));
		}
	}
	
	void add_scalars (std::initializer_list<std::string> labels)
	{
		for (const auto &label:labels)
		{
			scal.insert(std::pair<std::string,double>(label,0.));
		}
	}
	
	void add_vectors (int Nrows, std::initializer_list<std::string> labels)
	{
		for (const auto &label:labels)
		{
			VectorXd v(Nrows); v.setZero();
			vec.insert(std::pair<std::string,VectorXd>(label,v));
		}
	}
	
	void add_matrices (int Nrows, int Ncols, std::initializer_list<std::string> labels)
	{
		for (const auto &label:labels)
		{
			MatrixXd A(Nrows,Ncols); A.setZero();
			mat.insert(std::pair<std::string,MatrixXd>(label,A));
		}
	}
	
	void save (HDF5Interface &target, string group="")
	{
		for (auto it=intg.begin(); it!=intg.end(); ++it)
		{
			target.save_scalar(it->second,it->first,group);
		}
		for (auto it=scal.begin(); it!=scal.end(); ++it)
		{
			target.save_scalar(it->second,it->first,group);
		}
		for (auto it=vec.begin(); it!=vec.end(); ++it)
		{
			target.save_vector(it->second,it->first,group);
		}
		for (auto it=mat.begin(); it!=mat.end(); ++it)
		{
			target.save_matrix(it->second,it->first,group);
		}
	}
	
	std::map<std::string,int>      intg;
	std::map<std::string,double>   scal;
	std::map<std::string,VectorXd> vec;
	std::map<std::string,MatrixXd> mat;
	
};



#endif
