//
// Created by Cedric Gaberle on 09.06.23.
// Implement functions of QASM Parser Library in sequential and parallel manner to use in
// benchmarks.
// TODO: Remove whole header file for final version of library
//

#ifndef QASM_PARSER_BENCHMARKSUITE_H
#define QASM_PARSER_BENCHMARKSUITE_H

#include <sstream>
#include <vector>
#include <map>
#include <optional>
#include <chrono>


namespace bparser {
    /**
     * Implementation of a Quantum Parser Class. Holds operators, coefficients and parameters of the input file in instance
     * of subclass QuantumOperator. Implements parsing functionality to parse into OpenQASM Standard, version specified in
     * variable.
     */
    class BenchmarkParser {
    private:
        /**
         * Quantum operator struct holding information about operator, namely the index of occurrence in the input file,
         * gates working on which qubits, the coefficients, and the parameter. The parameter is used to indicate dependent
         * and independent operators by having the same (dependent) or different parameters (independent).
         */
        struct QuantumOperator {
            unsigned long index;                             // Index of operator in input (order of operators)
            std::string strRep;                              // String representation of operator
            std::vector<std::vector<unsigned long> > intOp;  // Operator in integer representation stored in vector of vectors
            float coef;                                      // Coefficient of operator
            unsigned long param;                             // Parameter indicating dependencies
        };

        unsigned long numberQubits;                          // Must equal length of operators in string representation
        std::string mup = "0.5*";                            // Float value to multiply operations
        std::vector<QuantumOperator> operators;              // Vector holding all operators as Quantum Operator struct
        bool openmp;                                         // Specify to use OpenMP parallelism

        /**
         * Parse string representation of input into shorter integer representation describing operator. Integer
         * representation of the form: vectors for each basis holding indices of corresponding character, meaning operation
         * in this particular basis takes place on qubit number index.
         * @param in : Representation of input operator in string format, e.g. IIIIXIY
         * @return Vector of three vectors, each Pauli operation, containing indices of rotations
         */
        std::vector<std::vector<unsigned long> > parseStrIntSeq(std::string &in);

        std::vector<std::vector<unsigned long> > parseStrIntPar(std::string &in);   // Parallel version

        /**
         * Check input for errors. Throw error if string representations have different lengths, the coefficient has a value
         * of zero, or the parameter has a value of less than one. Different string length correspond to different numbers
         * of qubits, which is not possible in a single circuit. A coefficient of zero would let the matrix collaps to zero
         * and therefore would be useless. The parameter, indicating dependencies of operators here, must be in range
         * (1, number of operators) by definition.
         * @param str String representation of the operator, e.g. IIIIXIY.
         * @param coef Coefficient of operator.
         * @param param Parameter of operator.
         */
        void errorCheck(std::string &str, float &coef, long long &param) const;

        /**
         * Print error message and line of occurrence.
         * @param errMessage Error message to print.
         * @param idx Line in input file of error occurrence.
         */
        static void printError(const std::string &errMessage, unsigned long &idx);

        /**
         * Read lines of the input file, perform error checking, convert representation of operators and store in quantum
         * operator instances. Push all operator instances in operators member.
         * @param filename Path to the input file.
         */
        void readLines(const std::string &filename);

        /**
         * Parse QuantumOperator instance operator into OpenQASM 2.0 representation. Pauli-X and -Y operations are performed
         * using rotations along the corresponding basis.
         * @param qop QuantumOperator instance holding index, parameter, coefficient, and integer representation
         * @return String representation of operator in QASM version 2
         */
        std::string parseOpToQasm2Seq(QuantumOperator &qop);

        std::string parseOpToQasm2Par(QuantumOperator &qop);    // Parallel version

    public:
        static long double opParallelPartTime;          // Runtime of parallelizable fraction sequential
        static long double strToIntParallelPartTime;    // Runtime of strToInt fraction sequential
        static long double toQasmParallelPartTime;      // Runtime of opToQasm fraction sequential
        static long double bothParallelPartTime;        // Runtime of strToInt and opToQasm in sequential

        static long double opParallelTime;              // Runtime of parallelizable fraction parallel
        static long double strToIntParallelTime;        // Runtime of strToInt fraction parallel
        static long double toQasmParallelTime;          // Runtime of toQasm fraction parallel
        static long double bothParallelTime;            // Runtime of both fractions parallel

        // Friend functions
        friend std::string benchmarkPar(const std::string &inFilename,
                                        bool useOpenMP,
                                        bool parallelParseStrInt,
                                        bool parallelParseOpToQasm,
                                        bool version,
                                        const std::optional<float> &multiplier);

        friend std::string benchmarkParOnly(const std::string &inFilename,
                                            bool useOpenMP,
                                            bool version,
                                            const std::optional<float> &multiplier);

        friend std::string benchmarkSeq(const std::string &inFilename,
                                        bool useOpenMP,
                                        bool parallelParseStrInt,
                                        bool parallelParseOpToQasm,
                                        bool version,
                                        const std::optional<float> &multiplier);

        friend std::string benchmarkSeqOnly(const std::string &inFilename,
                                            bool version,
                                            const std::optional<float> &multiplier);

    };

    /**
     * Parallel version of parsing algorithm.
     * @param inFilename Path to input file containing circuit in string representation
     * @param useOpenMP Specify to use OpenMP parallelism
     * @param parallelParseStrInt Specify to use string to integer parallelism additionally to operator parallelism
     * @param parallelParseOpToQasm Specify to use integer to OpenQASM string parallelism additionally
     * @param version If set true, equals OpenQASM version 3; Version 2 for false (default)
     * @param multiplier Optional; Multiplier to multiply all operations with
     * @return Qasm version of input file as string.
     */
    std::string benchmarkPar(const std::string &inFilename,
                             bool useOpenMP = false,
                             bool parallelParseStrInt = false,
                             bool parallelParseOpToQasm = false,
                             bool version = false,
                             const std::optional<float> &multiplier = std::nullopt);

    /**
     * Parallel version of parsing algorithm, only using operator parallelism
     * @param inFilename Path to input file containing circuit in string representation
     * @param useOpenMP Specify to use OpenMP parallelism
     * @param version If set true, equals OpenQASM version 3; Version 2 for false (default)
     * @param multiplier Optional; Multiplier to multiply all operations with
     * @return Qasm version of input file as string
     */
    std::string benchmarkParOnly(const std::string &inFilename,
                                 bool useOpenMP = false,
                                 bool version = false,
                                 const std::optional<float> &multiplier = std::nullopt);

    /**
     * Non operator parallelism version of parsing algorithm. Other parallelization strategies can be applied.
     * @param inFilename Path to input file containing circuit in string representation
     * @param useOpenMP Specify to use OpenMP parallelism
     * @param parallelParseStrInt Specify to use string to integer parallelism
     * @param parallelParseOpToQasm Specify to use integer to OpenQASM string parallelism
     * @param version If set true, equals OpenQASM version 3; Version 2 for false (default)
     * @param multiplier Optional; Multiplier to multiply all operations with
     * @return QASM version of input file as string
     */
    std::string benchmarkSeq(const std::string &inFilename,
                             bool useOpenMP = false,
                             bool parallelParseStrInt = false,
                             bool parallelParseOpToQasm = false,
                             bool version = false,
                             const std::optional<float> &multiplier = std::nullopt);

    /**
     * Sequential version of parsing algorithm.
     * @param inFilename Path to input file containing circuit in string representation
     * @param version If set true, equals OpenQASM version 3; Version 2 for false (default)
     * @param multiplier Optional; Multiplier to multiply all operations with
     * @return QASM version of input file as string
     */
    std::string benchmarkSeqOnly(const std::string &inFilename,
                                 bool version = false,
                                 const std::optional<float> &multiplier = std::nullopt);
}

#endif //QASM_PARSER_BENCHMARKSUITE_H
