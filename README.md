# OpenQASM Parsing Library
![Static Badge](https://img.shields.io/badge/Quantum%20Computing-purple)
![Static Badge](https://img.shields.io/badge/OpenQASM-blue)
![Static Badge](https://img.shields.io/badge/OpenMP-4.x-green)
![Static Badge](https://img.shields.io/badge/OpenMP-5.x-green)
![Static Badge](https://img.shields.io/badge/C%2B%2B17-Execution%20Policies-red)



This project implements a library to simplify the generation of OpenQASM files, the de facto standard when it comes to 
instructing quantum computers. Additionally, the project contains python bindings to use the library as an extension in 
python.

## Description

OpenQASM is considered the de facto standard as an intermediate representation, a step in between the problem formulation and the physical implementation on actual quantum hardware. Almost every current quantum computer compiler supports it. The problem lies in its generation, since it is mostly done by hand as for today, which is tedious and will be unapplicable as systems and problems to solve increase in their complexity in the future.

Some advancements in the direction of simplyfing the process have already been done, e.g. the Qiskit library to generate OpenQASM files from the problem's python code implementation.

Here we present an algorithm, implemented as library in C++, who's input is as simple as a text file consisting of strings and numbers. Further, we make use of available resources by utilizing parallel computing frameworks.


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
- `fmt`

## How To Use

A tutorial on how to install the library as python extension and use it can be found in the accompanying [jupyter 
notebook](https://github.com/msqc-goethe/QasmParserLibrary).

As a quick setup guide clone the repository and change into the directory. Perform the following commands with
your appropriate paths.

```
mkdir build && cd build
cmake .. -DCMAKE_C_COMPILER=/path_or_alias/to/your/gcc-binary -DCMAKE_CXX_COMPILER=/path_or_alias/to/your/g++-binary -DPYTHON_LIBRARY_DIR="<your_path_to_your_site-packages>" -DPYTHON_EXECUTABLE="<your_path_to_your_python_binary>"
cmake --build .
cmake --install .
```
(The compiler must only be provided if the GCC is not yet the default compiler. Quotation marks are necessary!)

### Parser Library
We can specify six different variables. The only variable that must be set is the path to the input file. This variable is positional, meaning we need to specify this variable first when calling the function. All other variables are key-word only. In order to specify them you have to set the variable at call in the standard pythonic way, e.g. *use_omp=True*.

- *version*
  The version can be set to be OpenQASM v2 or v3. The differences include the header specific to that version in the output OpenQASM file and the capability to deal with unspecified parameters. If not provided or set to a value other than 2 or 3, the default version used is version 3, which supports parameterization.

- *use_omp*
  If parallelization is supported by your system, it is enabled by default. One can, for scientific/benchmarking purposes for example, decide which parallelization framework to use. If set to *True* OpenMP is the underlying parallelization framework, otherwise and by default the C++17 execution policies are used. These have the added advantage of automatic compiler optimization, leaving less room for unnecassary occupancy of resources or errors in general. If one wishes, he can tailor the OpenMP implementation in the C++ library source code to his specific needs.

- *parameterize*
  Set this variable to parameterize the given ansatz. To indicate commuting operators we use the grouping/parameter index in the input file. These commuting operators share the same parameter. Non-parameterized circuits can also be generated by setting this variable to false. 

- *output_fn*
  If specified, the OpenQASM representation is written to the file in that location. This value is optional. Nevertheless, the output of the function is still accessible in the current session, e.g. stored in a variable bound to that call.

- *multiplier*
  Value to be multiplied to each operator. Again, this is an optional parameter which has no default.

