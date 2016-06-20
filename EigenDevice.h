#ifndef EigenDeviceQ
#define EigenDeviceQ

#include <unsupported/Eigen/CXX11/ThreadPool>
#include <unsupported/Eigen/CXX11/Tensor>

/**Class for handling the different devices, which can be used in Eigen to evaluate expressions.
\note For now, only the ThreadPoolDevice and the DefaultDevice are supported. The GpuDevice is not. Waiting for Eigen update*/

template<class device_type>
class EigenDevice
{
public:
	EigenDevice<device_type>() {};
	device_type get() const;
	void set(int Cores);
	
private:
	device_type device;
};

template<class device_type>
device_type EigenDevice<device_type>::
get() const
{
	return device;
}

template<class device_type>
void EigenDevice<device_type>::
set( int Cores )
{;}

template<>
void EigenDevice<Eigen::ThreadPoolDevice>::
set( int Cores )
{
	Eigen::ThreadPool tp(Cores); // = Eigen::ThreadPool(4);
	device = Eigen::ThreadPoolDevice(&tp,Cores);	
}


#endif
