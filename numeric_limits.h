#ifndef NUMERIC_LIMITS
#define NUMERIC_LIMITS

template<typename Scalar>
struct numeric_limits
{
public:
	numeric_limits() {};
	static inline Scalar epsilon() {return Scalar(1.e-12);};
	static inline Scalar infinity() {return Scalar(1.e20);};
};
#endif
