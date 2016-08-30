#ifndef TUPLES_Q
#define TUPLES_Q

#include <iostream>

//#include <boost/math/special_functions/binomial.hpp>

/**Class for ordered tuples of size \param N which contains numbers from 0 to \param R -1.
\describe_R
\describe_N*/
template<size_t R, size_t N>
class Tuples
{
	using size_t = std::size_t;
public:
	/**Returns true if the \param j 'th tuple contains the number \param k and otherwise false.
	 \param j : j is the number of the tuple to check
	 \param K : k is searched in the j'th tuple*/
	static bool isPresent(size_t j, size_t k);

	/**Returns the \param j 'th tuple.
	   \param j : j is the number of the tuple*/
	static std::array<size_t,N> getTuple(size_t j);

	/**Returns the number \param j of the tuple \param tuple.
	   \param tuple*/
	static size_t getNumber( const std::array<size_t,N> &tuple );

private:
	static void sort( std::array<size_t,N> &tuple );
	static size_t getFirstNumber ( size_t R_, size_t N_, size_t j );
	static size_t firstN ( size_t R_, size_t N_, size_t j );
	static size_t binomial ( size_t , size_t k );
	static size_t factorial ( size_t n );
};

template<size_t R, size_t N>
size_t Tuples<R,N>::
getNumber( const std::array<size_t,N> &tuple )
{
	auto t=tuple;
	sort(t);
	size_t out=0;
	size_t R_=R;
	for (size_t k=0; k<N; k++)
	{
		if( k==0 )
		{
			(t[k] > 0) ? out += firstN(R_,N-k,t[k]-1) : out += 0;
			R_ -= t[k]+1;
		}
		else
		{
			(t[k]-t[k-1]-1 > 0) ? out += firstN(R_,N-k,(t[k]-t[k-1]-2)) : out += 0;
			R_ -= t[k] - t[k-1];
		}
	}
	return out;
}

template<size_t R, size_t N>
bool Tuples<R,N>::
isPresent(size_t j, size_t k)
{
	auto t=getTuple(j);
	sort(t);
	auto it = std::find(t.begin(),t.end(),k);
	if (it != t.end()) { return true; }
	else { return false; }
}

template<size_t R, size_t N>
std::array<size_t,N> Tuples<R,N>::
getTuple(size_t j)
{
	std::array<size_t,N> out;
	size_t R_=R;
	std::vector<size_t>	j_; j_.push_back(j);
	size_t offset=0;
	for (size_t k=0; k<N; k++)
	{
		out[k] = getFirstNumber(R_,N-k,j_[k]) + offset;
		if (getFirstNumber(R_,N-k,j_[k]) > 0) { j_.push_back( j_[k] - firstN(R_,N-k,getFirstNumber(R_,N-k,j_[k])-1)); }
		else { j_.push_back(j_[k]); }
		offset += getFirstNumber(R_,N-k,j_[k])+1;
		R_ -= getFirstNumber(R_,N-k,j_[k])+1;
	}
	return out;
}

template<size_t R, size_t N>
void Tuples<R,N>::
sort( std::array<size_t,N> &tuple )
{
	std::sort(tuple.begin(),tuple.end());
}

template<size_t R, size_t N>
size_t Tuples<R,N>::
getFirstNumber( size_t R_, size_t N_, size_t j )
{
	for (size_t k=0; k<=R_-N_; k++)
	{
		size_t lower;
		( k == 0 ) ? lower = 0 : lower = firstN(R_,N_,k-1);
		auto upper = firstN(R_,N_,k);
		if ( j >= lower and j < upper ) { return k; }
	}
	assert(false and "error");
}

template<size_t R, size_t N>
size_t Tuples<R,N>::
firstN( size_t R_, size_t N_, size_t j )
{
	assert(j <= (R_-N_) and "invalid number.");
	size_t out=0;
	for (size_t k=0; k<=j; k++)
	{
		out +=  binomial(R_-k-1,N_-1); //static_cast<size_t>(boost::math::binomial_coefficient<double>(R_-k-1, N_-1)); //
	}
	return out;
}

template<size_t R, size_t N>
size_t Tuples<R,N>::
binomial (std::size_t n, std::size_t k)
{
	return factorial(n)/(factorial(k)*factorial(n-k));
}

template<size_t R, size_t N>
size_t Tuples<R,N>::
factorial(size_t n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

#endif
