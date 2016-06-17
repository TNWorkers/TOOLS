#ifndef EigenDevice
#define EigenDevice

#include <unsupported/Eigen/CXX11/ThreadPool>
#include <unsupported/Eigen/CXX11/Tensor>

/**
Class for handling the different devices, which can be used in Eigen to evaluate expressions.
\note For now only the ThreadPoolDevice and the DefaultDevice are supported. The GpuDevice is not. Waiting for Eigen update...
*/

class EigenDevice
{
public:
	EigenDevice();

	EigenDevice( size_t numCores_in ) { numCores=numCores_in; tp(numcores); thread_pool_device(tp,numCores); }

	Eigen::ThreadPoolDevice getPoolDevice() { return thread_pool_device; }
	Eigen::DefaultDevice getDefaultDevice() { return default_device; }
	
private:
	size_t numCores;
	Eigen::ThreadPool tp;
	Eigen::ThreadPoolDevice thread_pool_device;
	Eigen::DefaultDevice default_device;
}
#endif
