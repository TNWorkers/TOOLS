# Numerical Tools Library

This library contains basic numerical tools that can be combined with the VMPS code (https://github.com/TNWorkers/VMPS) or with other codes.

## Features

- parsing command-line arguments (akin to Python's argparse)
- interface to save C++ Eigen matrices into files, esp. in the HDF5 format
- interface to save generic numerical data to files
- convenient iterator to iterate over an interval and save data
- logger to log the program output
- calculate to compute the memory requirement of Eigen objects
- iterator to perform an arbitrary number of nested loops
- convenience typedefs to work with nuclear data
- Ooura integration for oscillating integrals
- parameter handler to parse generic parameters given to Hamiltonian
- polychromatic console output
- functions to create electromagnetic pulses
- stopwatch to measure time requirement of functions
- convenience functions to handle strings, e.g. convert arbitrary numerical data to strings
- plotter to plot results directly in the terminal

## Dependencies

- C++, Eigen, Boost, GSL, HDF5

## Documentation

Browse doxygen [documentation](https://tnworkers.github.io/TOOLS) to explore the interface and general documentation.

## Contributers

Roman Rausch, Matthias Peschke, Cassian Plorin
