#ifndef NESTEDLOOPITERATOR
#define NESTEDLOOPITERATOR

//#include <Eigen/Dense>
#include <initializer_list>
#include <algorithm>
#include <functional>
#include <assert.h>
#include <numeric>

class NestedLoopIterator
{
public:
	
	NestedLoopIterator(){};
	NestedLoopIterator (size_t dim_input, initializer_list<size_t> ranges_input);
//	NestedLoopIterator (size_t dim_input, Eigen::VectorXi ranges_input);
	NestedLoopIterator (size_t dim_input, vector<size_t> ranges_input);
	NestedLoopIterator (size_t dim_input, size_t const_range);
	
	void operator++()
	{
		++curr_index;
		make_tensorIndex();
	};
	
	size_t operator*() {return index();}
	
	NestedLoopIterator& operator= (const size_t &cmp) {curr_index=cmp; make_tensorIndex();}
	bool operator<  (const size_t &cmp) {return curr_index< cmp;}
	bool operator<= (const size_t &cmp) {return curr_index<=cmp;}
	bool operator!= (const size_t &cmp) {return curr_index!=cmp;}
	bool operator== (const size_t &cmp) {return curr_index==cmp;}
	
	size_t begin();
	size_t end();
	
	inline size_t index() {return curr_index;}
//	inline size_t index_sum() {return tensor_index.sum();}
	inline size_t index_sum() {return accumulate(tensor_index.begin(), tensor_index.end(), 0);}
	inline size_t operator() (size_t index) const {return tensor_index[index];}
	
//private:

	size_t dim;
	size_t curr_index;
	size_t total_range;
	
	void make_tensorIndex();
	void make_reverseTensorIndex();
	
//	Eigen::VectorXi tensor_index;
//	Eigen::VectorXi ranges;
	vector<size_t> tensor_index;
	vector<size_t> ranges;
};

NestedLoopIterator::
NestedLoopIterator (size_t dim_input, initializer_list<size_t> ranges_input)
:dim(dim_input)
{
	tensor_index.resize(dim);
	ranges.resize(dim);
	assert(ranges_input.size()==dim);
	
	size_t i=0;
	for (auto range=ranges_input.begin(); range!=ranges_input.end(); ++range)
	{
		assert(*range>=0);
		ranges[i] = *range;
		++i;
	}
//	total_range = ranges.prod();
	total_range = accumulate(ranges.begin(), ranges.end(), 1, multiplies<size_t>());
	
//	tensor_index.setZero();
	std::fill(tensor_index.begin(), tensor_index.end(), 0);
	curr_index = 0;
}

//NestedLoopIterator::
//NestedLoopIterator (int dim_input, Eigen::VectorXi ranges_input)
//:dim(dim_input)
//{
//	tensor_index.resize(dim);
//	ranges.resize(dim);
//	assert(ranges_input.rows()==dim);
//	
//	ranges = ranges_input;
//	total_range = ranges.prod();
//	
//	tensor_index.setZero();
//	curr_index = 0;
//}

NestedLoopIterator::
NestedLoopIterator (size_t dim_input, vector<size_t> ranges_input)
:dim(dim_input)
{
	assert(ranges_input.size() == dim);
	tensor_index.resize(dim);
	ranges.resize(dim);
	
	ranges = ranges_input;
//	total_range = ranges.prod();
	total_range = accumulate(ranges.begin(), ranges.end(), 1, multiplies<size_t>());
	
//	tensor_index.setZero();
	std::fill(tensor_index.begin(), tensor_index.end(), 0);
	curr_index = 0;
}

NestedLoopIterator::
NestedLoopIterator (size_t dim_input, size_t const_range)
:dim(dim_input)
{
	tensor_index.resize(dim);
	ranges.resize(dim);
	
	std::fill(ranges.begin(), ranges.end(), const_range);
//	std::fill(tensor_index.begin(), tensor_index.end(), const_range);
	total_range = accumulate(ranges.begin(), ranges.end(), 1, multiplies<size_t>());
	
	std::fill(tensor_index.begin(), tensor_index.end(), 0);
	curr_index = 0;
}

size_t NestedLoopIterator::
begin()
{
	return 0;
}

size_t NestedLoopIterator::
end()
{
	return total_range;
}

void NestedLoopIterator::
make_tensorIndex()
{
	size_t r = curr_index;
	for (int s=dim-1; s>=0; --s)
//	for (size_t s=dim-1; s-->0;) // wrong
	{
		tensor_index[s] = r%ranges[s];
		r /= ranges[s];
	}
}

void NestedLoopIterator::
make_reverseTensorIndex()
{
	size_t r = curr_index;
	for (int s=0; s<dim; s++)
//	for (size_t s=dim-1; s-->0;) // wrong
	{
		tensor_index[s] = r%ranges[s];
		r /= ranges[s];
	}
}

#endif
