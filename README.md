# OpenQASM Parsing Library
![Static Badge](https://img.shields.io/badge/Quantum%20Computing-purple)
![Static Badge](https://img.shields.io/badge/OpenQASM-blue)
![Static Badge](https://img.shields.io/badge/OpenMP-4.x-green)
![Static Badge](https://img.shields.io/badge/OpenMP-5.x-green)
![Static Badge](https://img.shields.io/badge/C%2B%2B17-Execution%20Policies-red)



This project implements a library to simplify the generation of OpenQASM files, the de facto standard when it comes to 
instructing quantum computers. Additionally, the project contains benchmarking files to evaluate performance.

## Description

OpenQASM is considered the de facto standard as an intermediate representation, a step in between the problem formulation and the physical implementation on actual quantum hardware. Almost every current quantum computer compiler supports it. The problem lies in its generation, since it is mostly done by hand as for today, which is tedious and will be unapplicable as systems and problems to solve increase in their complexity in the future.

Some advancements in the direction of simplyfing the process have already been done, e.g. the Qiskit library to generate OpenQASM files from the problem's python code implementation.

Here we present an algorithm, implemented as library in C++, who's input is as simple as an text file consisting of strings and numbers. Further, we make use of available resources by utilizing parallel computing frameworks.


### Input

The text file to feed into the algorithm consists of a string where each character represents a single qubit of the target 
hardware. Using the pauli basis as a complete basis set, able to represent any arbitrary qubit state operation, 
the possible characters are `I` (Identity matrix), `X`(Pauli-X), `Y`(Pauli-Y), and `Z`(Pauli-Z). Further, coefficients 
and parameters for each operator are possible to implement ansätze used in the variational quantum context. Coefficients 
can be optimized in the optimization loop of expectation minimization of the energy eigenstates. Parameters can be used 
to group individual operators, reducing the computational cost in the optimization loop. Groupted operators, sharing the 
same parameter, are optimized in relation to one another.

The overall input file looks something similar to this:
```
IIIXYII 1.5 1

IYXZIII 0.8 2

IIIIIYZ 1.1 1
```

## Requirements

The sequential version implementation of the algorithm may work with any compiler on any given system supporting
C++. To be able to use the parallel version of the algorithm, different frameworks and compilers may be needed.
Following are tested and working combinations, but others may also be applicable. 

- GCC 9 or higher
- cmake 3.23 or higher

To use parallelism, following frameworks must be installed:
- `TBB`
- `Threads`
- `OpenMP`

## How To Use

Since this repository contains the whole project, including the library as well as benchmarking specifics, we can simply
clone and have everything set up, including a main to work with already linked against the library. The standalone 
library will be provided in a different repository as well.

```
# Check out the project
$ git clone 
# Go to project root directory
$ cd QASM-Parser
# Install Google Benchmark here
$ <Follow instructions from https://github.com/google/benchmark>
# Build directory in project directory; may be placed anywhere but data paths should be 
# in the same directory as the build to run benchmarks, otherwise change benchmark files
$ cmake -S . -B build/ -DCMAKE_CXX_COMPILER=<gcc; if not default>
# Build the project
$ cmake --build build/
```

This builds the whole project. Placing the data in the projects root directory, we can run benchmarks
or simply use the library in the main. 

### Google Benchmarks

After building the project we can simply run the Google benchmark executables to investigate the runtime
of different inputs using different strategies, i.e. sequential, parallel execution policy, OpenMP. The executables can
be found under `build/Benchmarks/Benchmark<specific data>/<executable>`.

### Parser Library
To try out the library itself, one can use the `main.cpp` which is already linked against it. The actual library
provides a single function, which is overloaded to change between sequential and parallel execution on operator level.
```
# Sequential execution
qasmparser::parseCircuit(<path to input file>, <version>, <output file name>, <multiplier>);

# Parallel execution
qasmparser::parseCircuit(<path to input file>, <bool to specify OpenMP usage>, <version>, <output file name>, <multiplier>);
```

- Path to input file
  - String
- Bool value to specify OpenMP usage
  - If set, algorithm is used in parallel, otherwise sequential
  - True: Use OpenMP parallelism
  - False: Use execution policy parallelism
- Version
  - Integer value, by default `2` for OpenQASM version 2
  - Can be set to `3` to use version 3
  - Other values are not supported and fall back to version 2
- Output file name
  - Optional value specifying path to store OpenQASM representation
- Multiplier
  - Optional value to multiply all operators with

### Benchmark Suite
To use some benchmarking tools, we are using the `benchmarkSuite`, part of the library. There we have the option to use the sequential or 
parallel version of the algorithm, as well as some different parallelization code fractions tried out during the creation 
of this project. These include the `strToInt` parallelization, where the process from the input string to the then used 
lighter integer representation of vectors holding the indices of the Pauli operations per operator is parallelized. The 
`toQasm` parallelism parallelizes the process from extracting these indices and performing necessary rotations to form 
actual exponentiated unitary in OpenQASM representation. As well as the parallelization also done in the main library ,
performing best, on the operator level where operators are parsed to their OpenQASM representation in parallel, implying 
the other two parallelization strategies intrinsically.

The functions callable form the `benchmarkSuite` are:
```
# Operator parallelism + others; OpenMP or execution policy
bparser::benchmarkPar(<input path>, <useOpenMP>, <strToInt>, <toQasm>, <version>, <multiplier>);

# Only operator parallelism; OpenMP or execution policy
bparser::benchmarkParOnly(<input path>, <useOpenMP>, <version>, <multiplier>);

# No operator parallelism, but possible others; OpenMP or execution policy
bparser::benchmarkSeq(<input path>, <useOpenMP>, <strToInt>, <toQasm>, <version>, <multiplier>);

# Sequential execution, no parallelism 
bparser::benchmarkSeqOnly(<input path>, <version>, <multiplier>);
```

### Helper Functions
We also implemented some functions to ensure correctness and perform benchmarking. These fall into two categories, 
`compare` functions, comparing the output of different approaches to ensure correctness, and `speedup` functions providing
the possibility to generate a .json file containing different interesting benchmarking statistics, e.g. speedupu of parallel
vs. sequential, the parallelizable fraction, Amdahl's speedup etc.

```
# Generate benchmarking values for provided parallelism compared to sequential version
speedup::executionTimes(<input file>, <useOpenMP>, <opParallel>, <strToInt>, <toQasm>);

# Generate .json file containing benchmarking values for easy accesibility and further research storable in output file
# Multiple files at once possible
speedup::generateJson(<file names>, <path to file directory>, <useOpenMP>, <opParallel>, <strToInt>, <toQasm>, <output file>); 
```

