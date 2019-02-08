#ifndef EIGENFILES
#define EIGENFILES

#include <iostream>
#include <fstream>
//#include <cstdint>
//#include <filesystem>

#include <Eigen/Dense>

#define MAXBUFSIZE ((int) 1e6)

Eigen::MatrixXd readMatrix (const std::string filename)
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
		getline(infile, line);
		
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
	Eigen::MatrixXd result(rows,cols);
	for (int i=0; i<rows; i++)
	for (int j=0; j<cols; j++)
		result(i,j) = buff[cols*i+j];
	
	return result;
};

#endif