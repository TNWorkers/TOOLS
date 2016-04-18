#ifndef SIMPLELISTINITIALIZER
#define SIMPLELISTINITIALIZER

#include <Eigen/Dense>

class SimpleListInitializer
{
public:

    SimpleListInitializer (Eigen::MatrixXd * M_input, int row_input, int col_input)
    :M(M_input), row(row_input), col(col_input)
    {}

    SimpleListInitializer operator, (double x) 
    {
//    	M->conservativeResize(M->rows(),M->cols()+1);
        (*M)(row,col) = x;
        return SimpleListInitializer(M,row,col+1);
    }
    
    SimpleListInitializer operator, (complex<double> x) 
    {
//    	M->conservativeResize(M->rows(),M->cols()+2);
		(*M)(row,col)   = x.real();
		(*M)(row,col+1) = x.imag();
        return SimpleListInitializer(M,row,col+2); 
    }

private:

    SimpleListInitializer(); 

protected:

	int row;
	int col;
    Eigen::MatrixXd * M; 
};

//template<typename ScalarType, typename IteratorType> 
//class SimpleListInitializer
//{
//public:

//    SimpleListInitializer (IteratorType Iterator_input)
//    :storedIterator(Iterator_input)
//    {}

//    SimpleListInitializer<ScalarType,IteratorType> operator, (ScalarType x) 
//    { 
//        *storedIterator = x; 
//        return SimpleListInitializer<ScalarType,IteratorType> (storedIterator+1); 
//    }

//private:

//    SimpleListInitializer(); 

//protected:

//    IteratorType storedIterator; 
//};

//class Aarray 
//{
//public:
//    
//    Aarray (std::size_t size = 10) : data_(new double[size])
//    {
//    }

//    ~Aarray()
//    {
//        delete [] data_;
//    }

//    SimpleListInitializer<double,double*> operator<< (double x)
//    {
////    	cout << x << endl;
//        data_[0] = x;
////      cout << data_[0] << endl;
//        return SimpleListInitializer<double,double*>(data_+1);
////      cout << data_[0] << endl;
//    }
//    
//    void print()
//    {
//		cout << "data: " << data_[0] << "\t" << data_[1] << endl;
//    }

//private:

//    double* data_;
//};

#endif
