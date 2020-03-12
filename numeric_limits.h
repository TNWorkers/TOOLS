#ifndef NUMERIC_LIMITS
#define NUMERIC_LIMITS

template<typename Scalar>
struct mynumeric_limits
{
public:
	mynumeric_limits() {};
	static inline double epsilon() {return static_cast<double>(1.e-8);};
	static inline Scalar infinity() {return static_cast<Scalar>(1.e20);};
};
#endif
