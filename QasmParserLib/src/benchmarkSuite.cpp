//
// Created by Cedric Gaberle on 09.06.23.
//

#include "benchmarkSuite.h"
#include "fmt/core.h"

#include <omp.h>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <execution>
#include <chrono>
#include <atomic>


// Declare static data members
long double bparser::BenchmarkParser::opParallelTime = 0;
long double bparser::BenchmarkParser::opParallelPartTime = 0;
long double bparser::BenchmarkParser::strToIntParallelPartTime = 0;
long double bparser::BenchmarkParser::toQasmParallelPartTime = 0;
long double bparser::BenchmarkParser::bothParallelPartTime = 0;
long double bparser::BenchmarkParser::strToIntParallelTime = 0;
long double bparser::BenchmarkParser::toQasmParallelTime = 0;
long double bparser::BenchmarkParser::bothParallelTime = 0;

// Helper function to get character index
template <typename T, typename Func>
void enumerate(const T& container, Func&& func) {
    size_t index = 0;
    for (const auto& element : container) {
        func(element, index);
        ++index;
    }
}

void bparser::BenchmarkParser::errorCheck(std::string& str, float& coef, long long& param) const {
    if (str.empty())
        throw std::invalid_argument("No operator provided!");
    else if (str.length() != numberQubits)
        throw std::invalid_argument("Non-matching length of string representation!");
    else if (coef == 0)
        throw std::invalid_argument("Zero coefficient!");
    else if (param < 0)
        throw std::invalid_argument("Negative parameter!");
    else if (param > std::numeric_limits<unsigned long>::max())
        throw std::invalid_argument("Parameter out of bound!");
}

void bparser::BenchmarkParser::printError(const std::string &errMessage, unsigned long &idx) {
    std::cerr << "Error! At line " << idx << std::endl;
    std::cerr << errMessage << std::endl;
    std::exit(EXIT_FAILURE);
}

void bparser::BenchmarkParser::readLines(const std::string& filename) {
    std::ifstream inFile(filename);

    if (inFile.is_open()){
        std::string line;
        unsigned long lineIdx = 0;

        while (getline(inFile, line)){
            lineIdx += 1;
            BenchmarkParser::QuantumOperator qop;
            std::string strRep; float coef; unsigned long param;  // Operator parameters
            long long checkParam;

            std::istringstream is (line);
            if (!(is >> strRep >> coef >> checkParam)){
                inFile.close();
                printError(std::string("Wrong format!"), lineIdx);
            }

            // Set number of qubits corresponding to qubits in first operator (must be equal for all operators)
            if (lineIdx == 1)
                numberQubits = strRep.length();

            // Error checking on input line operator
            try {errorCheck(strRep, coef, checkParam);}
            catch (const std::invalid_argument& strException) {
                inFile.close();
                printError(strException.what(), lineIdx);
            }
            catch (...) {
                inFile.close();
                printError(std::string("Unknown Error!"), lineIdx);
            }

            param = checkParam;

            // Set independent parameter if provided is 0
            if (param == 0)
                param = lineIdx;

            // Store each line in QuantumOperator struct and push into operators vector
            qop.index = lineIdx; qop.strRep = strRep; qop.coef = coef; qop.param = param;
            operators.emplace_back(qop);
        }
    }
    inFile.close();
}

std::vector< std::vector<unsigned long> > bparser::BenchmarkParser::parseStrIntSeq(std::string &in) {
    std::vector<unsigned long> xVec, yVec, zVec;
    std::vector< std::vector<unsigned long> > intRep;
    intRep.reserve(3);

    // Check for Pauli characters in string representation
    for (auto ch = in.begin(); ch != in.end(); ch++) {
        switch (*ch) {
            case 'I':
                break;
            case 'X': {
                unsigned long i = std::distance(in.begin(), ch) + 1;  // Index of X character
                xVec.emplace_back(i);
                break;
            }
            case 'Y': {
                unsigned long i = std::distance(in.begin(), ch) + 1;  // Index of Y character
                yVec.emplace_back(i);
                break;
            }
            case 'Z': {
                unsigned long i = std::distance(in.begin(), ch) + 1;  // Index of Z character
                zVec.emplace_back(i);
                break;
            }
            default:
                throw std::invalid_argument("Unsupported character instruction!");
        }
    }

    intRep.emplace_back(xVec); intRep.emplace_back(yVec); intRep.emplace_back(zVec);
    return intRep;
}

std::vector< std::vector<unsigned long> > bparser::BenchmarkParser::parseStrIntPar(std::string &in) {
    std::vector<unsigned long> xVec, yVec, zVec;
    std::vector< std::vector<unsigned long> > intRep;
    intRep.reserve(3);

    // Check for Pauli characters in string representation
    if (openmp) {
        #pragma omp parallel for default(none) shared(in, xVec, yVec, zVec)
        for (unsigned long i = 0; i < in.size(); i++) {
            if (in[i] == 'X') {
                #pragma omp critical (xVec)
                {
                    xVec.emplace_back(i + 1);
                }
            } else if (in[i] == 'Y') {
                #pragma omp critical (yVec)
                {
                    yVec.emplace_back(i + 1);
                }
            } else if (in[i] == 'Z') {
                #pragma omp critical (zVec)
                {
                    zVec.emplace_back(i + 1);
                }
            }
        }
    }

    else {
        std::mutex mtx;
        std::for_each(std::execution::par, in.begin(), in.end(), [&](const char &c) {
            if (c == 'X') {
                enumerate(in, [&](char, size_t index) {
                    if (&c == &in[index]) {
                        {
                            std::lock_guard<std::mutex> guard(mtx);
                            xVec.emplace_back(static_cast<unsigned long>(index + 1));
                        }
                    }
                });
            } else if (c == 'Y') {
                enumerate(in, [&](char, size_t index) {
                    if (&c == &in[index]) {
                        {
                            std::lock_guard<std::mutex> guard(mtx);
                            yVec.emplace_back(static_cast<unsigned long>(index + 1));
                        }
                    }
                });
            } else if (c == 'Z') {
                enumerate(in, [&](char, size_t index) {
                    if (&c == &in[index]) {
                        {
                            std::lock_guard<std::mutex> guard(mtx);
                            zVec.emplace_back(static_cast<unsigned long>(index + 1));
                        }
                    }
                });
            }
        });
    }

    intRep.emplace_back(xVec); intRep.emplace_back(yVec); intRep.emplace_back(zVec);
    return intRep;
}

std::string bparser::BenchmarkParser::parseOpToQasm2Seq(QuantumOperator& qop) {
    // Parameterised rotation in Z basis. By definition this rotation is done on the last used qubit by the operator
    const auto lastUsed = std::max(
            {qop.intOp[0].empty() ? 0 : *std::max_element(qop.intOp[0].begin(), qop.intOp[0].end()),
             qop.intOp[1].empty() ? 0 : *std::max_element(qop.intOp[1].begin(), qop.intOp[1].end()),
             qop.intOp[2].empty() ? 0 : *std::max_element(qop.intOp[2].begin(), qop.intOp[2].end())}
             );

    std::string qasmOp = fmt::format("rz({}{}*$[{}]) q[{}];\n", mup, qop.coef, qop.param, lastUsed - 1);
    std::string beforeLast, afterLast;

    // Go through vector entries indicating pauli matrix on qubits at these indices and therefore rotations in
    // corresponding basis. First vector corresponds to Pauli-X, second to Pauli-Y, third and last to Pauli-Z.
    for (const auto& vec : qop.intOp) {
        switch (std::distance(qop.intOp.begin(), std::find(qop.intOp.begin(), qop.intOp.end(), vec))) {
            case 0:
                for (auto qubitIdx : vec) {
                    std::string before = fmt::format("ry(pi/2) q[{}];\n", qubitIdx - 1);
                    std::string after = fmt::format("ry(-pi/2) q[{}];\n", qubitIdx - 1);

                    if (qubitIdx == lastUsed) {
                        beforeLast = before; afterLast = after;
                        continue;
                    }

                    before += fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1);
                    after.insert(0, fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1));

                    qasmOp.insert(0, before);
                    qasmOp += after;
                }
                break;

            case 1:
                for (auto qubitIdx : vec) {
                    // Rotation in x basis by -0.5 pi before and +0.5 pi afterwards
                    std::string before = fmt::format("rx(-pi/2) q[{}];\n", qubitIdx - 1);
                    std::string after = fmt::format("rx(pi/2) q[{}];\n", qubitIdx - 1);

                    if (qubitIdx == lastUsed) {
                        afterLast = after; beforeLast = before;
                        continue;
                    }
                    // If operation is performed on last qubit used by operator, no CNOTs are needed
                    before += fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1);
                    after.insert(0, fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1));

                    qasmOp.insert(0, before);
                    qasmOp += after;
                }
                break;

            case 2:
                for (auto qubitIdx : vec) {
                    // Z vector, indicating Pauli Z operations. No rotation since we are in Z basis already
                    if (qubitIdx != lastUsed) {
                        std::string before = fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1);
                        std::string after = fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1);

                        qasmOp.insert(0, before);
                        qasmOp += after;
                    }
                }
                break;

            default:
                // To many vectors, therefore to many pauli operations, only 3 possible (Pauli-X, Pauli-Y, Pauli-Z)
                std::exit(EXIT_FAILURE);
        }
    }

    qasmOp.insert(0, beforeLast);
    qasmOp += afterLast;

    return qasmOp.insert(0, fmt::format("\n// New operator from line {}\n", qop.index));
}

std::string bparser::BenchmarkParser::parseOpToQasm2Par(QuantumOperator& qop) {
    // Parameterised rotation in Z basis. By definition this rotation is done on the last used qubit by the operator
    const auto lastUsed = std::max(
            {qop.intOp[0].empty() ? 0 : *std::max_element(qop.intOp[0].begin(), qop.intOp[0].end()),
             qop.intOp[1].empty() ? 0 : *std::max_element(qop.intOp[1].begin(), qop.intOp[1].end()),
             qop.intOp[2].empty() ? 0 : *std::max_element(qop.intOp[2].begin(), qop.intOp[2].end())}
             );

    std::string qasmOp = fmt::format("rz({}{}*$[{}]) q[{}];\n", mup, qop.coef, qop.param, lastUsed - 1);
    std::string beforeLast, afterLast;

    if (!openmp) {
        std::mutex mtxQasmOp;
        std::mutex mtxBeforeAfter;
        // Go through vector entries indicating pauli matrix on qubits at these indices and therefore rotations in
        // corresponding basis. First vector corresponds to Pauli-X, second to Pauli-Y, third and last to Pauli-Z.
        std::for_each(std::execution::par, qop.intOp.begin(), qop.intOp.end(),
                      [&](const std::vector<unsigned long> &vec) {
                          auto it = std::find(qop.intOp.begin(), qop.intOp.end(), vec);
                          switch (it - qop.intOp.begin()) {
                              case 0:
                                  for (auto qubitIdx: vec) {
                                      // Rotation in y basis by +0.5 pi before and -0.5 pi afterward
                                      std::string before = fmt::format("ry(pi/2) q[{}];\n", qubitIdx - 1);
                                      std::string after = fmt::format("ry(-pi/2) q[{}];\n", qubitIdx - 1);

                                      if (qubitIdx == lastUsed) {
                                          {
                                              std::lock_guard<std::mutex> guard(mtxBeforeAfter);
                                              beforeLast = before;
                                              afterLast = after;
                                          }
                                          continue;
                                      }

                                      before += fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1);
                                      after.insert(0, fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1));
                                      {
                                          std::lock_guard<std::mutex> guard(mtxQasmOp);
                                          qasmOp.insert(0, before);
                                          qasmOp += after;
                                      }
                                  }
                                  break;

                              case 1:
                                  for (auto qubitIdx: vec) {
                                      // Rotation in x basis by -0.5 pi before and +0.5 pi afterward
                                      std::string before = fmt::format("rx(-pi/2) q[{}];\n", qubitIdx - 1);
                                      std::string after = fmt::format("rx(pi/2) q[{}];\n", qubitIdx - 1);

                                      if (qubitIdx == lastUsed) {
                                          {
                                              std::lock_guard<std::mutex> guard(mtxBeforeAfter);
                                              beforeLast = before;
                                              afterLast = after;
                                          }
                                          continue;
                                      }

                                      before += fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1);
                                      after.insert(0, fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1));
                                      {
                                          std::lock_guard<std::mutex> guard(mtxQasmOp);
                                          qasmOp.insert(0, before);
                                          qasmOp += after;
                                      }
                                  }
                                  break;

                              case 2:
                                  for (auto qubitIdx: vec) {
                                      // Z vector, indicating Pauli Z operations. No rotation since we are in Z basis already
                                      if (qubitIdx != lastUsed) {
                                          std::string before = fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1,
                                                                           lastUsed - 1);
                                          std::string after = fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1,
                                                                          lastUsed - 1);
                                          {
                                              std::lock_guard<std::mutex> guard(mtxQasmOp);
                                              qasmOp.insert(0, before);
                                              qasmOp += after;
                                          }
                                      }
                                  }
                                  break;

                              default:
                                  // To many vectors, therefore to many pauli operations, only 3 possible (Pauli-X, Pauli-Y, Pauli-Z)
                                  std::exit(EXIT_FAILURE);
                          }
                      });
    }

    else {
        // OpenMP implementation
        #pragma omp parallel for default(none) shared(qop, lastUsed, beforeLast, afterLast, qasmOp) num_threads(3)
        for (const auto &vec: qop.intOp) {
            switch (std::distance(qop.intOp.begin(), std::find(qop.intOp.begin(), qop.intOp.end(), vec))) {
                case 0:
                    for (auto qubitIdx: vec) {
                        // Rotation in y basis by +0.5 pi before and -0.5 pi afterward
                        std::string before = fmt::format("ry(pi/2) q[{}];\n", qubitIdx - 1);
                        std::string after = fmt::format("ry(-pi/2) q[{}];\n", qubitIdx - 1);

                        if (qubitIdx == lastUsed) {
                            #pragma omp critical (beforeAfterLast)
                            {
                                beforeLast = before;
                                afterLast = after;
                            }
                            continue;
                        }

                        before += fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1);
                        after.insert(0, fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1));

                        #pragma omp critical (beforeAfter)
                        {
                            qasmOp.insert(0, before);
                            qasmOp += after + " // From thread: ";
                        }
                    }
                    break;

                case 1:
                    for (auto qubitIdx: vec) {
                        // Rotation in x basis by -0.5 pi before and +0.5 pi afterward
                        std::string before = fmt::format("rx(-pi/2) q[{}];\n", qubitIdx - 1);
                        std::string after = fmt::format("rx(pi/2) q[{}];\n", qubitIdx - 1);

                        if (qubitIdx == lastUsed) {
                            #pragma omp critical (beforeAfterLast)
                            {
                                beforeLast = before;
                                afterLast = after;
                            }
                            continue;
                        }

                        before += fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1);
                        after.insert(0, fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1));

                        #pragma omp critical (beforeAfter)
                        {
                            qasmOp.insert(0, before);
                            qasmOp += after;
                        }
                    }
                    break;

                case 2:
                    for (auto qubitIdx: vec) {
                        // Z vector, indicating Pauli Z operations. No rotation since we are in Z basis already
                        if (qubitIdx != lastUsed) {
                            std::string before = fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1);
                            std::string after = fmt::format("cx q[{}], q[{}];\n", qubitIdx - 1, lastUsed - 1);

                            #pragma omp critical (beforeAfter)
                            {
                                qasmOp.insert(0, before);
                                qasmOp += after;
                            }
                        }
                    }
                    break;

                default:
                    // To many vectors, therefore to many pauli operations, only 3 possible (Pauli-X, Pauli-Y, Pauli-Z)
                    std::exit(EXIT_FAILURE);
            }
        }
    }

    qasmOp.insert(0, beforeLast);
    qasmOp += afterLast;

    return qasmOp.insert(0, fmt::format("\n// New operator from line {}\n", qop.index));
}

std::string bparser::benchmarkPar(const std::string& inFilename,
                                          const bool useOpenMP,
                                          const bool parallelParseStrInt,
                                          const bool parallelParseOpToQasm,
                                          const bool version,
                                          const std::optional<float>& multiplier) {
    BenchmarkParser p;
    std::map<unsigned long, std::string> qasmOperators;
    std::string qasm;
    p.openmp = useOpenMP;

    if (multiplier.has_value())
        p.mup = std::to_string(multiplier.value()) + "*";

    std::mutex parseOpMtx;

    // Read lines into `operators` vector
    p.readLines(inFilename);

    // Time parallel execution
    auto parStartTime = std::chrono::system_clock::now();

    if (useOpenMP) {
        #pragma omp parallel for default(none) shared(p, parallelParseStrInt, version, parallelParseOpToQasm, qasmOperators)
        for (auto &op: p.operators) {
            try {
                parallelParseStrInt ? op.intOp = p.parseStrIntPar(op.strRep) : op.intOp =
                                                                                       p.parseStrIntSeq(op.strRep);
            }
            catch (const std::invalid_argument &exception) {
                #pragma omp critical (print)
                p.printError(exception.what(), op.index);
            }

            std::string qasmOp;
            parallelParseOpToQasm ? qasmOp = p.parseOpToQasm2Par(op) : qasmOp = p.parseOpToQasm2Seq(op);
            #pragma omp critical (qasmOperator)
            qasmOperators[op.index] = qasmOp;
        }
    }

    else {
        // For each operator: parse string into integer representation, parse into OpenQASM, and store in `qasmOperators`
        std::for_each(std::execution::par, p.operators.begin(), p.operators.end(),
                      [&](BenchmarkParser::QuantumOperator op) {
                          //for (auto& op: parser.operators) {
                          try {
                              parallelParseStrInt ? op.intOp = p.parseStrIntPar(op.strRep) : op.intOp =
                                                                                                     p.parseStrIntSeq(
                                                                                                             op.strRep);
                          }
                          catch (const std::invalid_argument &exception) {
                              p.printError(exception.what(), op.index);
                          }

                          std::string qasmOp;
                          parallelParseOpToQasm ? qasmOp = p.parseOpToQasm2Par(op) : qasmOp = p.parseOpToQasm2Seq(op);

                          std::lock_guard<std::mutex> guard(parseOpMtx);
                          qasmOperators[op.index] = qasmOp;
                      });
    }

    auto parEndTime = std::chrono::system_clock::now();
    BenchmarkParser::opParallelTime = std::chrono::duration_cast<std::chrono::duration<long double, std::nano> >
            (parEndTime - parStartTime).count();

    // OpenQASM version specific header
    if (version) {
        qasm.insert(0, fmt::format("OPENQASM 3.0;\n"
                                          "include \"stdgates.inc\";\n"
                                          "qubit[{0}] q;\n"  // Qubit register of size `numberQubits`
                                          "bit[{0}] c;\n",   // Classical bit register of same size
                                          p.numberQubits)
        );
    }
    else {
        qasm.insert(0, fmt::format("OPENQASM 2.0;\n"
                                          "include \"qelib1.inc\";\n"
                                          "qreg q[{0}];\n"   // Qubit register of size `numberQubits`
                                          "creg c[{0}];\n",  // Classical bit register of same size
                                          p.numberQubits)
        );
    }

    for (const auto& [idx, op] : qasmOperators) {
        qasm += op;
    }
    return qasm;
}

std::string bparser::benchmarkParOnly(const std::string& inFilename,
                                      const bool useOpenMP,
                                      const bool version,
                                      const std::optional<float>& multiplier) {
    BenchmarkParser p;
    std::map<unsigned long, std::string> qasmOperators;
    std::string qasm;
    p.openmp = useOpenMP;

    if (multiplier.has_value())
        p.mup = std::to_string(multiplier.value()) + "*";

    std::mutex parseOpMtx;

    // Read lines into `operators` vector
    p.readLines(inFilename);

    if (useOpenMP) {
        #pragma omp parallel for default(none) shared(p, version, qasmOperators)
        for (auto &op: p.operators) {
            try {
                op.intOp = p.parseStrIntSeq(op.strRep);
            }
            catch (const std::invalid_argument &exception) {
                #pragma omp critical (print)
                p.printError(exception.what(), op.index);
            }

            std::string qasmOp;
            qasmOp = p.parseOpToQasm2Seq(op);
            #pragma omp critical (qasmOperator)
            qasmOperators[op.index] = qasmOp;
        }
    }

    else {
        // For each operator: parse string into integer representation, parse into OpenQASM, and store in `qasmOperators`
        std::for_each(std::execution::par, p.operators.begin(), p.operators.end(),
                      [&](BenchmarkParser::QuantumOperator op) {
                          //for (auto& op: parser.operators) {
                          try {
                              op.intOp = p.parseStrIntSeq(op.strRep);
                          }
                          catch (const std::invalid_argument &exception) {
                              p.printError(exception.what(), op.index);
                          }

                          std::string qasmOp;
                          qasmOp = p.parseOpToQasm2Seq(op);

                          std::lock_guard<std::mutex> guard(parseOpMtx);
                          qasmOperators[op.index] = qasmOp;
                      });
    }

    // OpenQASM version specific header
    if (version) {
        qasm.insert(0, fmt::format("OPENQASM 3.0;\n"
                                   "include \"stdgates.inc\";\n"
                                   "qubit[{0}] q;\n"  // Qubit register of size `numberQubits`
                                   "bit[{0}] c;\n",   // Classical bit register of same size
                                   p.numberQubits)
        );
    }
    else {
        qasm.insert(0, fmt::format("OPENQASM 2.0;\n"
                                   "include \"qelib1.inc\";\n"
                                   "qreg q[{0}];\n"   // Qubit register of size `numberQubits`
                                   "creg c[{0}];\n",  // Classical bit register of same size
                                   p.numberQubits)
        );
    }

    for (const auto& [idx, op] : qasmOperators) {
        qasm += op;
    }
    return qasm;
}

std::string bparser::benchmarkSeq(const std::string& inFilename,
                                          const bool useOpenMP,
                                          const bool parallelParseStrInt,
                                          const bool parallelParseOpToQasm,
                                          const bool version,
                                          const std::optional<float>& multiplier) {
    BenchmarkParser p;
    std::map<unsigned long, std::string> qasmOperators;
    std::string qasm;
    p.openmp = useOpenMP;

    //BenchmarkParser parser;
    if (multiplier.has_value())
        p.mup = std::to_string(multiplier.value()) + "*";

    // Read lines into `operators` vector and print execution time
    p.readLines(inFilename);

    // Time operator parallelism fraction execution
    if (!parallelParseStrInt && !parallelParseOpToQasm) {
        long double strToInt = 0;
        long double toQasm = 0;
        long double both = 0;

        auto parStartTime = std::chrono::system_clock::now();
        for (auto& op : p.operators) {
            auto bothStartTime = std::chrono::system_clock::now();
            auto intOpStartTime = std::chrono::system_clock::now();
            try {
                op.intOp = p.parseStrIntSeq(op.strRep);
            }
            catch (const std::invalid_argument& exception) {
                p.printError(exception.what(), op.index);
            }
            auto intOpEndTime = std::chrono::system_clock::now();
            strToInt += std::chrono::duration_cast<std::chrono::duration<long double, std::nano> >(intOpEndTime - intOpStartTime).count();

            std::string qasmOp;
            auto toQasmStartTime = std::chrono::system_clock::now();
            qasmOp = p.parseOpToQasm2Seq(op);
            auto toQasmEndTime = std::chrono::system_clock::now();
            toQasm += std::chrono::duration_cast<std::chrono::duration<long double, std::nano> >(toQasmEndTime - toQasmStartTime).count();

            auto bothEndTime = std::chrono::system_clock::now();
            both += std::chrono::duration_cast<std::chrono::duration<long double, std::nano> >(bothEndTime - bothStartTime).count();
            qasmOperators[op.index] = qasmOp;
        }

        auto parEndTime = std::chrono::system_clock::now();
        BenchmarkParser::opParallelPartTime = std::chrono::duration_cast<std::chrono::duration<long double, std::nano> >
                (parEndTime - parStartTime).count();
        BenchmarkParser::strToIntParallelPartTime = strToInt;
        BenchmarkParser::toQasmParallelPartTime = toQasm;
        BenchmarkParser::bothParallelPartTime = both;
    }

    else if(parallelParseOpToQasm && parallelParseStrInt) {
        auto bothParallelStartTime = std::chrono::system_clock::now();
        for (auto &op: p.operators) {
            try {
                op.intOp = p.parseStrIntPar(op.strRep);
            }
            catch (const std::invalid_argument &exception) {
                p.printError(exception.what(), op.index);
            }

            std::string qasmOp;
            qasmOp = p.parseOpToQasm2Par(op);

            qasmOperators[op.index] = qasmOp;
        }

        auto bothParallelEndTime = std::chrono::system_clock::now();
        BenchmarkParser::bothParallelTime = std::chrono::duration_cast<std::chrono::duration<long double, std::nano> >
                (bothParallelEndTime - bothParallelStartTime).count();
    }

    else if (parallelParseStrInt) {
        long double strToInt = 0;

        for (auto &op: p.operators) {
            auto toIntStartTime = std::chrono::system_clock::now();
            try {
                op.intOp = p.parseStrIntPar(op.strRep);
            }
            catch (const std::invalid_argument &exception) {
                p.printError(exception.what(), op.index);
            }
            auto toIntEndTime = std::chrono::system_clock::now();
            strToInt += std::chrono::duration_cast<std::chrono::duration<long double, std::nano> >(toIntEndTime - toIntStartTime).count();

            std::string qasmOp;
            qasmOp = p.parseOpToQasm2Par(op);

            qasmOperators[op.index] = qasmOp;
        }
        BenchmarkParser::strToIntParallelTime = strToInt;
    }

    else {
        long double opToQasm = 0;

        for (auto &op: p.operators) {
            try {
                op.intOp = p.parseStrIntPar(op.strRep);
            }
            catch (const std::invalid_argument &exception) {
                p.printError(exception.what(), op.index);
            }

            auto toQasmStartTime = std::chrono::system_clock::now();
            std::string qasmOp;
            qasmOp = p.parseOpToQasm2Par(op);

            auto toQasmEndTime = std::chrono::system_clock::now();
            opToQasm += std::chrono::duration_cast<std::chrono::duration<long double, std::nano> >(
                    toQasmEndTime - toQasmStartTime).count();

            qasmOperators[op.index] = qasmOp;
        }
        BenchmarkParser::toQasmParallelTime = opToQasm;
    }

    // OpenQASM version specific header
    if (version) {
        qasm.insert(0, fmt::format("OPENQASM 3.0;\n"
                                          "include \"stdgates.inc\";\n"
                                          "qubit[{0}] q;\n"  // Qubit register of size `numberQubits`
                                          "bit[{0}] c;\n",   // Classical bit register of same size
                                          p.numberQubits)
        );
    }
    else {
        qasm.insert(0, fmt::format("OPENQASM 2.0;\n"
                                          "include \"qelib1.inc\";\n"
                                          "qreg q[{0}];\n"   // Qubit register of size `numberQubits`
                                          "creg c[{0}];\n",  // Classical bit register of same size
                                          p.numberQubits)
        );
    }

    for (const auto& [idx, op] : qasmOperators) {
        qasm += op;
    }
    return qasm;
}

std::string bparser::benchmarkSeqOnly(const std::string& inFilename,
                                      const bool version,
                                      const std::optional<float>& multiplier) {
    BenchmarkParser p;
    std::map<unsigned long, std::string> qasmOperators;
    std::string qasm;

    //BenchmarkParser parser;
    if (multiplier.has_value())
        p.mup = std::to_string(multiplier.value()) + "*";

    // Read lines into `operators` vector and print execution time
    p.readLines(inFilename);

    for (auto& op : p.operators) {
        try {
            op.intOp = p.parseStrIntSeq(op.strRep);
        }
        catch (const std::invalid_argument& exception) {
            p.printError(exception.what(), op.index);
        }

        std::string qasmOp;
        qasmOp = p.parseOpToQasm2Seq(op);

        qasmOperators[op.index] = qasmOp;
    }

    // OpenQASM version specific header
    if (version) {
        qasm.insert(0, fmt::format("OPENQASM 3.0;\n"
                                   "include \"stdgates.inc\";\n"
                                   "qubit[{0}] q;\n"  // Qubit register of size `numberQubits`
                                   "bit[{0}] c;\n",   // Classical bit register of same size
                                   p.numberQubits)
        );
    }
    else {
        qasm.insert(0, fmt::format("OPENQASM 2.0;\n"
                                   "include \"qelib1.inc\";\n"
                                   "qreg q[{0}];\n"   // Qubit register of size `numberQubits`
                                   "creg c[{0}];\n",  // Classical bit register of same size
                                   p.numberQubits)
        );
    }

    for (const auto& [idx, op] : qasmOperators) {
        qasm += op;
    }
    return qasm;
}