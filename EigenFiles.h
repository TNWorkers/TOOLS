#ifndef EIGENFILES
#define EIGENFILES

#include <iostream>
#include <fstream>
//#include <cstdint>
//#include <filesystem>
#include <unistd.h>
#include <string>

#include <Eigen/Dense>

#define MAXBUFSIZE ((int) 1e6)

Eigen::Matrix<double,Dynamic,Dynamic> readMatrix (const std::string filename)
{
	int cols = 0;
	int rows = 0;
	double buff[MAXBUFSIZE];
	
//	assert(std::filesystem::exists(filename));
	
	// Read numbers from file into buffer.
	ifstream infile;
	infile.open(filename);
	assert(infile.is_open() and "Does the file exist?");
	while (!infile.eof())
	{
		std::string line;
		getline(infile,line);
		
		if (line.length() > 0 and line.at(0) != '#')
		{
			int temp_cols = 0;
			stringstream stream(line);
			while (!stream.eof()) stream >> buff[cols*rows+temp_cols++];
			if (temp_cols == 0) continue;
			if (cols == 0) cols = temp_cols;
			rows++;
		}
	}
	
	infile.close();
	
	// Populate matrix with numbers.
	Eigen::Matrix<double,Dynamic,Dynamic> result(rows,cols);
	for (int i=0; i<rows; i++)
	for (int j=0; j<cols; j++)
	{
		result(i,j) = buff[cols*i+j];
	}
	
	return result;
};

Eigen::Matrix<double,Dynamic,Dynamic> loadMatrix (const std::string filename)
{
	return readMatrix(filename);
}

void saveMatrix (const Eigen::Matrix<double,Dynamic,Dynamic> &M, const std::string filename, bool PRINT = true)
{
	ofstream fout(filename);
	for (int i=0; i<M.rows(); ++i)
	{
		for (int j=0; j<M.cols(); ++j)
		{
			fout << setprecision(14) << M(i,j);
			if (j != M.cols() - 1)
			{
				fout << "\t";
			}
			else
			{
				fout << endl;
			}
		}
	}
	fout.close();
	if (PRINT) {lout << "saved to: " << filename << endl;}
};

void saveMatrix_cpython (const Eigen::Matrix<complex<double>,Dynamic,Dynamic> &M, const std::string filename, bool PRINT = true)
{
	ofstream fout(filename);
	for (int i=0; i<M.rows(); ++i)
	{
		for (int j=0; j<M.cols(); ++j)
		{
			fout << setprecision(14) << M(i,j).real();
			if (M(i,j).imag() > 0.) fout << "+";
			fout << M(i,j).imag() << "j";
			
//			fout << setprecision(14) << M(i,j);
			
			if (j != M.cols() - 1)
			{
				fout << "\t";
			}
			else
			{
				fout << endl;
			}
		}
	}
	fout.close();
	if (PRINT) {lout << "saved to: " << filename << endl;}
};

void save_xy (const Eigen::Array<double,Dynamic,1> &x, const Eigen::Array<double,Dynamic,1> &y, const std::string filename, bool PRINT = true)
{
	ofstream fout(filename);
	for (int i=0; i<x.rows(); ++i)
	{
		fout << setprecision(14) << x(i) << "\t" << y(i) << endl;
	}
	fout.close();
	if (PRINT) {lout << "saved to: " << filename << endl;}
}

void save_xy (const Eigen::Array<double,Dynamic,1> &x, const Eigen::Array<double,Dynamic,1> &y1, 
              const Eigen::Array<double,Dynamic,1> &y2, const std::string filename, bool PRINT = true)
{
	ofstream fout(filename);
	for (int i=0; i<x.rows(); ++i)
	{
		fout << setprecision(14) << x(i) << "\t" << y1(i) << "\t" << y2(i) << endl;
	}
	fout.close();
	if (PRINT) {lout << "saved to: " << filename << endl;}
}

//inline bool fileExists (const std::string& filename)
//{
//	struct stat buffer;
//	return (stat(filename.c_str(), &buffer) == 0); 
//}

#endif
