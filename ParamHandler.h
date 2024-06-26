#ifndef PARAMHANDLER
#define PARAMHANDLER

#include <any>
#include <set>
#include <tuple>
#include <map>
#include <initializer_list>
#include <typeinfo>
#include <typeindex>
#include <vector>
#include <string>

#include <Eigen/Dense>

template<typename Scalar>
struct param0d
{
	Scalar x;
	std::string label;
	inline Scalar operator() () const {return x;}
};

template<typename Scalar>
struct param1d
{
	Scalar x;
	Eigen::Array<Scalar,Eigen::Dynamic,1> a;
	std::string label;
	inline Scalar operator() (std::size_t i) const {return a(i);}
};

template<typename Scalar>
struct param2d
{
	Scalar x;
	Eigen::Array<Scalar,Eigen::Dynamic,Eigen::Dynamic> a;
	std::string label;
	inline Scalar operator() (std::size_t i, std::size_t j) const {return a(i,j);}
};

struct Param
{
	Param (std::string label_input, std::any value_input, size_t index_input=0)
	:label(label_input), value(value_input), index(index_input)
	{};
	
	Param (std::tuple<std::string,std::any> input, size_t index_input=0)
	:label(get<0>(input)), value(get<1>(input)), index(index_input)
	{};
	
	std::string   label;
	std::any value;
	size_t   index=0;
};

class ParamHandler
{
public:
	
	ParamHandler (const std::vector<Param> &p_list);
	ParamHandler (const std::vector<Param> &p_list, const std::map<std::string,std::any> &defaults_input);
	
	template<typename Scalar> Scalar get (const std::string label, const size_t index=0) const;
	template<typename Scalar> Scalar get_default (const std::string label) const;
	bool HAS (const std::string label, const size_t index=0) const;
	bool HAS_ANY_OF (const std::initializer_list<std::string> &labels, const size_t &index=0) const;
	bool HAS_NONE_OF (const std::initializer_list<std::string> &labels, const size_t &index=0) const;
	template<typename Scalar> bool ARE_ALL_ZERO (const std::initializer_list<std::string> &labels, const size_t &index=0) const;
	inline size_t size() const {return params.size();}
	
//	std::string info() const;
	
	template<typename Scalar> param0d<Scalar> fill_array0d (std::string label_def, std::string label_x, size_t loc=0) const;
	template<typename Scalar> param1d<Scalar> fill_array1d (std::string label_x, std::string label_a, size_t size_a, size_t loc=0) const;
	
	template<typename Scalar> param2d<Scalar> fill_array2d (std::string label_x, std::string label_a, size_t size_a, size_t loc=0) const;
	template<typename Scalar> param2d<Scalar> fill_array2d (std::string label_x, std::string label_a, std::array<size_t,2> size_a, size_t loc=0) const;
	
	template<typename Scalar> param2d<Scalar> fill_array2d (std::string label_x1, std::string label_x2, std::string label_a, size_t size_a, size_t loc, 
	                                                        bool PERIODIC=false) const;
	
private:
	
	Eigen::IOFormat arrayFormat;
	
	size_t calc_cellsize (const std::vector<Param> &p_list);
	
	std::vector<std::map<std::string,std::any> > params;
	std::map<std::string,std::any> defaults;
};

ParamHandler::
ParamHandler (const std::vector<Param> &p_list)
{
	params.resize(calc_cellsize(p_list));
	arrayFormat = Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, ",", ";", "", "", "{", "}");
	
	for (auto p:p_list)
	{
		params[p.index].insert(make_pair(p.label,p.value));
	}
}

ParamHandler::
ParamHandler (const std::vector<Param> &p_list, const std::map<std::string,std::any> &defaults_input)
:defaults(defaults_input)
{
	params.resize(calc_cellsize(p_list));
	arrayFormat = Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, ",", ";", "", "", "{", "}");
	
	for (auto p:p_list)
	{
		params[p.index].insert(make_pair(p.label,p.value));
	}
}

template<typename Scalar> 
Scalar ParamHandler::
get (const std::string label, size_t index) const
{
	assert(index < params.size());
	
	auto it = params[index].find(label);
	
	if (it != params[index].end())
	{
		return any_cast<Scalar>(it->second);
	}
	else
	{
		auto it0 = params[0].find(label);
		if (it0 != params[0].end())
		{
			return any_cast<Scalar>(it0->second);
		}
		else
		{
			return get_default<Scalar>(label);
		}
	}
}

template<typename Scalar> 
Scalar ParamHandler::
get_default (const std::string label) const
{
	auto it = defaults.find(label);
	if (it == defaults.end())
	{
		lout << "Cannot get default parameter " << label << "!" << std::endl;
		assert(it != defaults.end());
	}
	Scalar res;
	try
	{
		res = any_cast<Scalar>(it->second);
	}
	catch (const std::bad_any_cast& e)
	{
		lout << e.what() << ", parameter " << label << ", output type=" << typeid(Scalar).name() << std::endl;
		throw;
	}
	return res;
//	return any_cast<Scalar>(it->second);
}

bool ParamHandler::
HAS (const std::string label, const size_t index) const
{
	auto it = params[index].find(label);
	if (it != params[index].end())
	{
		return true;
	}
	else
	{
		auto it0 = params[0].find(label);
		if (it0 != params[0].end())
		{
			return true;
		}
	}
	return false;
}

bool ParamHandler::
HAS_ANY_OF (const std::initializer_list<std::string> &labels, const size_t &index) const
{
	std::vector<bool> res;
	for (const auto &label:labels) {res.push_back(HAS(label,index));}
	return (find(res.begin(), res.end(), true) != res.end())? true:false;
}

bool ParamHandler::
HAS_NONE_OF (const std::initializer_list<std::string> &labels, const size_t &index) const
{
	return !HAS_ANY_OF(labels,index);
}

template<typename Scalar>
bool ParamHandler::
ARE_ALL_ZERO (const std::initializer_list<std::string> &labels, const size_t &index) const
{
	std::vector<bool> res;
	for (const auto &label:labels) {res.push_back(get<Scalar>(label,index) == 0);}
	return (find(res.begin(), res.end(), false) != res.end())? false:true;
}

size_t ParamHandler::
calc_cellsize (const std::vector<Param> &p_list)
{
	if (p_list.size() == 0) return 1; // p_list = {} => use only defaults
	
	std::set<size_t> indices;
	
	for (auto p:p_list)
	{
		indices.insert(p.index);
	}
	
	for (auto i:indices)
	{
		assert(i<indices.size() and "Strange choice of cell indices!");
	}
	
	return indices.size();
}

template<typename Scalar>
param0d<Scalar> ParamHandler::
fill_array0d (std::string label_def, std::string label_x, size_t loc) const
{
	param0d<Scalar> res;
	res.x = get_default<Scalar>(label_def);
	
	if (HAS(label_x,loc))
	{
		res.x = get<Scalar>(label_x,loc);
		
		std::stringstream ss;
		ss << label_x << "=" << res.x;
		res.label = ss.str();
	}
	else if (HAS(label_def,loc))
	{
		res.x = get<Scalar>(label_def,loc);
	}
	
	return res;
}

template<typename Scalar>
param1d<Scalar> ParamHandler::
fill_array1d (std::string label_x, std::string label_a, size_t size_a, size_t loc) const
{
	assert(!(HAS(label_x) and HAS(label_a)));
	
	param1d<Scalar> res;
	
	res.a.resize(size_a);
	res.a.setZero();
	res.x = get_default<Scalar>(label_x);
	std::stringstream ss;
	
	if (HAS(label_x,loc))
	{
		res.x = get<Scalar>(label_x,loc);
		res.a = res.x;
		ss << label_x << "=" << res.x;
	}
	else if (HAS(label_a,loc))
	{
		res.a = get<Eigen::Array<Scalar,Eigen::Dynamic,1> >(label_a,loc);
		ss << label_a << "=" << res.a.format(arrayFormat);
	}
	
	if (HAS(label_x,loc) or HAS(label_a,loc))
	{
		res.label = ss.str();
	}
	else // default label != 0
	{
		if (res.x == get_default<Scalar>(label_x) and res.x != 0.)
		{
			ss << label_x << "=" << res.x << "(default)";
			res.label = ss.str();
		}
	}
	
	return res;
}

template<typename Scalar>
param2d<Scalar> ParamHandler::
fill_array2d (std::string label_x, std::string label_a, size_t size_a, size_t loc) const
{
	return fill_array2d<Scalar>(label_x, label_a, {{size_a, size_a}}, loc);
}

// hopping in x-direction
template<typename Scalar>
param2d<Scalar> ParamHandler::
fill_array2d (std::string label_x, std::string label_a, std::array<size_t,2> size_a, size_t loc) const
{
	assert(!(HAS(label_x) and HAS(label_a)));
	
	param2d<Scalar> res;
	
	auto set_a = [&res, &size_a] ()
	{
		res.a.resize(size_a[0],size_a[1]);
		res.a.setZero();
		// Same amount of orbitals: ladder
		if (size_a[0] == size_a[1]) {res.a.matrix().diagonal().setConstant(res.x);}
		// Different amount of orbitals: fully connected sites
		else                        {res.a.matrix().setConstant(res.x);}
	};
	
	res.x = get_default<Scalar>(label_x);
	set_a();
	
	std::stringstream ss;
	
	if (HAS(label_a,loc))
	{
		res.a = get<Eigen::Array<Scalar,Eigen::Dynamic,Eigen::Dynamic> >(label_a,loc);
		ss << label_a << "=" << res.a.format(arrayFormat);
		res.label = ss.str();
	}
	else if (HAS(label_x,loc))
	{
		res.x = get<Scalar>(label_x,loc);
		ss << label_x << "=" << res.x;
		set_a();
	}
	
	if (HAS(label_x,loc) or HAS(label_a,loc))
	{
		res.label = ss.str();
	}
	else // default label != 0
	{
		if (res.x == get_default<Scalar>(label_x) and res.x != 0.)
		{
			ss << label_x << "=" << res.x << "(default)";
			res.label = ss.str();
		}
	}
	
	return res;
}

// hopping in y-direction
template<typename Scalar>
param2d<Scalar> ParamHandler::
fill_array2d (std::string label_x1, std::string label_x2, std::string label_a, size_t size_a, size_t loc, bool PERIODIC) const
{
	assert(!(HAS(label_x1) and HAS(label_a)));
	assert(!(HAS(label_x2) and HAS(label_a)));
	
	param2d<Scalar> res;
	
	auto set_a = [&res, &size_a, &PERIODIC] ()
	{
		res.a.resize(size_a,size_a);
		res.a.setZero();
		if(size_a > 1)
		{
			res.a.matrix().template diagonal<1>().setConstant(0.5*res.x);
			res.a.matrix().template diagonal<-1>() = res.a.matrix().template diagonal<1>();
		}
		if (PERIODIC and size_a > 2)
		{
			res.a(0,size_a) = 0.5*res.x;
			res.a(size_a,0) = 0.5*res.x;
		}
	};
	
	if (HAS(label_x1,loc))
	{
		res.x = get_default<Scalar>(label_x1);
	}
	else
	{
		res.x = get_default<Scalar>(label_x2);
	}
	set_a();
	
	std::stringstream ss;
	
	if (HAS(label_x1,loc) or HAS(label_x2,loc))
	{
		res.x = HAS(label_x1,loc)? get<Scalar>(label_x1,loc) : get<Scalar>(label_x2,loc);
		set_a();
		
		if (HAS(label_x1,loc))
		{
			ss << label_x1 << "=" << res.x;
		}
	}
	else if (HAS(label_a,loc))
	{
		res.a = get<Eigen::Array<Scalar,Eigen::Dynamic,Eigen::Dynamic> >(label_a,loc);
		ss << label_a << "=" << res.a.format(arrayFormat);
	}
	
	if (HAS(label_x1,loc) or HAS(label_x2,loc) or HAS(label_a,loc))
	{
		res.label = ss.str();
	}
	
	return res;
}

// It makes little sense, to print out std::any, but here is a possibility if you know all possible types:

//std::string ParamHandler::
//info() const
//{
//	std::stringstream ss;
//	
//	unordered_std::map<type_index,std::string> type_names;
//	type_names[type_index(typeid(int))]             = "int";
//	type_names[type_index(typeid(size_t))]          = "size_t";
//	type_names[type_index(typeid(double))]          = "double";
//	type_names[type_index(typeid(complex<double>))] = "complex<double>";
//	type_names[type_index(typeid(std::vector<double>))]  = "std::vector<double>";
//	type_names[type_index(typeid(bool))]            = "bool";
//	
//	for (auto p:params)
//	{
//		ss << p.first << "\t";
//		
//		if (type_names[std::type_index(p.second.type())] == "int")
//		{
//			ss << any_cast<int>(p.second);
//		}
//		else if (type_names[std::type_index(p.second.type())] == "size_t")
//		{
//			ss << any_cast<size_t>(p.second);
//		}
//		else if (type_names[std::type_index(p.second.type())] == "double")
//		{
//			ss << any_cast<double>(p.second);
//		}
//		else if (type_names[std::type_index(p.second.type())] == "complex<double>")
//		{
//			ss << any_cast<complex<double> >(p.second);
//		}
//		else if (type_names[std::type_index(p.second.type())] == "std::vector<double>")
//		{
//			std::vector<double> pval = any_cast<std::vector<double> >(p.second);
//			for (int i=0; i<pval.size(); ++i)
//			{
//				ss << any_cast<double>(pval[i]);
//				if (i!=pval.size()-1)
//				{
//					ss << ",";
//				}
//			}
//		}
//		else if (type_names[std::type_index(p.second.type())] == "bool")
//		{
//			ss << any_cast<bool>(p.second);
//		}
//		else
//		{
//			throw ("unknown parameter type!");
//		}
//		ss << std::endl;
//	}
//	
//	return ss.str();
//}

#endif
