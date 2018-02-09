#ifndef NESTEDLOOPITERATOR
#define NESTEDLOOPITERATOR

//#include <Eigen/Dense>
#include <vector>
#include <initializer_list>
#include <algorithm>
#include <functional>
#include <assert.h>
#include <numeric>

class NestedLoopIterator
{
public:
	
	NestedLoopIterator(){};
	NestedLoopIterator (std::size_t dim_input, std::initializer_list<std::size_t> ranges_input);
	NestedLoopIterator (std::size_t dim_input, std::vector<std::size_t> ranges_input);
	NestedLoopIterator (std::size_t dim_input, std::size_t const_range);
	
	void operator++() {++curr_index; make_tensorIndex();};
	
	std::size_t operator*() {return index();}
	
	NestedLoopIterator& operator= (const std::size_t &cmp) {curr_index=cmp; make_tensorIndex(); return *this;}
	bool operator<  (const std::size_t &cmp) {return curr_index< cmp;}
	bool operator<= (const std::size_t &cmp) {return curr_index<=cmp;}
	bool operator!= (const std::size_t &cmp) {return curr_index!=cmp;}
	bool operator== (const std::size_t &cmp) {return curr_index==cmp;}
	
	std::size_t begin();
	std::size_t end();
	
	inline std::size_t index() {return curr_index;}
	inline std::size_t index_sum() {return accumulate(tensor_index.begin(), tensor_index.end(), 0);}
	inline std::size_t operator() (std::size_t index) const {return tensor_index[index];}
	
private:

	std::size_t dim;
	std::size_t curr_index;
	std::size_t total_range;
	
	void make_tensorIndex();
	void make_reverseTensorIndex();
	
	std::vector<std::size_t> tensor_index;
	std::vector<std::size_t> ranges;
};

NestedLoopIterator::
NestedLoopIterator (std::size_t dim_input, std::initializer_list<std::size_t> ranges_input)
:dim(dim_input)
{
	tensor_index.resize(dim);
	ranges.resize(dim);
	assert(ranges_input.size()==dim);
	
	std::size_t i=0;
	for (auto range=ranges_input.begin(); range!=ranges_input.end(); ++range)
	{
		assert(*range>=0);
		ranges[i] = *range;
		++i;
	}
	total_range = accumulate(ranges.begin(), ranges.end(), 1, std::multiplies<std::size_t>());
	
	std::fill(tensor_index.begin(), tensor_index.end(), 0);
	curr_index = 0;
}

NestedLoopIterator::
NestedLoopIterator (std::size_t dim_input, std::vector<std::size_t> ranges_input)
:dim(dim_input)
{
	assert(ranges_input.size() == dim);
	tensor_index.resize(dim);
	ranges.resize(dim);
	
	ranges = ranges_input;
	total_range = accumulate(ranges.begin(), ranges.end(), 1, std::multiplies<std::size_t>());
	
	std::fill(tensor_index.begin(), tensor_index.end(), 0);
	curr_index = 0;
}

NestedLoopIterator::
NestedLoopIterator (std::size_t dim_input, std::size_t const_range)
:dim(dim_input)
{
	tensor_index.resize(dim);
	ranges.resize(dim);
	
	std::fill(ranges.begin(), ranges.end(), const_range);
	total_range = accumulate(ranges.begin(), ranges.end(), 1, std::multiplies<std::size_t>());
	
	std::fill(tensor_index.begin(), tensor_index.end(), 0);
	curr_index = 0;
}

std::size_t NestedLoopIterator::
begin()
{
	return 0;
}

std::size_t NestedLoopIterator::
end()
{
	return total_range;
}

void NestedLoopIterator::
make_tensorIndex()
{
	std::size_t r = curr_index;
	for (int s=dim-1; s>=0; --s) // must be int!
	{
		tensor_index[s] = r%ranges[s];
		r /= ranges[s];
	}
}

void NestedLoopIterator::
make_reverseTensorIndex()
{
	std::size_t r = curr_index;
	for (int s=0; s<dim; s++)
	{
		tensor_index[s] = r%ranges[s];
		r /= ranges[s];
	}
}

#endif
