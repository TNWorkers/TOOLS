#ifndef SIMPLE_FILER_CLASS
#define SIMPLE_FILER_CLASS

#include <initializer_list>
#include <Eigen/Core>

class Filer
{
public:
	
	Filer (string name_input)
	:name(name_input)
	{
		// flush file:
		object.open(name,ios::trunc);
		object.close();
	}
	
	template<typename Scalar1, typename Scalar2>
	void write (Scalar1 x1, Scalar2 x2)
	{
		object.open(name,ios::app);
		object << x1 << "\t" << x2 << endl;
		object.close();
	}
	
	template<typename Scalar1, typename Scalar2>
	void write (Scalar1 x1, const initializer_list<Scalar2> &line)
	{
		object.open(name,ios::app);
		object << x1 << "\t";
		for (auto i=line.begin(); i!=line.end(); ++i)
		{
			object << *i << "\t";
		};
		object << endl;
		object.close();
	}
	
	template<typename Scalar1, typename Scalar2>
	void write (Scalar1 x1, Eigen::Array<Scalar2,Eigen::Dynamic,1> vec)
	{
		object.open(name,ios::app);
		object << x1 << "\t";
		for (auto i=0; i<vec.rows(); ++i)
		{
			object << vec(i);
			if (i!=vec.rows()-1)
			{
				object << "\t";
			}
		};
		object << endl;
		object.close();
	}
	
	void store (double x, double val)
	{
		abscissa.push_back(x);
		data.push_back(val);
	}
	
	void store (double x, const vector<double> &val)
	{
		abscissa.push_back(x);
		multi_data.push_back(val);
	}
	
	void store (double x, const Eigen::Array<double,Eigen::Dynamic,1> vec)
	{
		vector<double> stdvec;
		stdvec.resize(vec.size());
		Eigen::Array<double,Eigen::Dynamic,1>::Map(&stdvec[0], vec.size()) = vec;
		store(x,stdvec);
	}
	
	void store (double x1, const initializer_list<double> &val)
	{
		vector<double> stdvec;
		for (auto i=val.begin(); i!=val.end(); ++i)
		{
			stdvec.push_back(*i);
		};
		store(x1,stdvec);
	}
	
	void save()
	{
		object.open(name);
		if (data.size() > 0)
		{
			for (int i=0; i<abscissa.size(); ++i)
			{
				object << abscissa[i] << "\t" << data[i] << endl;
			}
		}
		if (multi_data.size() > 0)
		{
			for (int i=0; i<abscissa.size(); ++i)
			{
				object << abscissa[i] << "\t";
				for (int j=0; j<multi_data[i].size(); ++j)
				{
					object << multi_data[i][j] << "\t";
				}
				object << endl;
			}
		}
		object.close();
	}
	
private:
	
	string name;
	ofstream object;
	
	vector<double> abscissa;
	vector<double> data;
	vector<vector<double> > multi_data;
};

#endif
