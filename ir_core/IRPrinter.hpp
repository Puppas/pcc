#ifndef PCC_IR_CORE_IRPRINTER_H
#define PCC_IR_CORE_IRPRINTER_H

#include <iostream>
#include <string>
#include <unordered_map>

enum class ValueKind;
class Type;
class Value;
class Inst;
class BB;
class Module;
class Function;


/**
 * \class IRPrinter
 * \brief Utility class for printing Intermediate Representation (IR).
 *
 * The \c IRPrinter class provides functionality to print Basic Blocks, functions, and modules
 * to a given output stream.
 */
class IRPrinter 
{
public:
    IRPrinter() = default;
    IRPrinter(const IRPrinter&) = delete;
    IRPrinter& operator=(const IRPrinter&) = delete;

    /**
     * \brief Prints the given Basic Block to the output stream.
     * \param bb The Basic Block to be printed.
     * \param os The output stream.
     * \param debug The print mode.
     */
    void print(const BB *bb, std::ostream &os, bool debug);

    /**
     * \brief Prints the given function to the output stream.
     * \param func The function to be printed.
     * \param os The output stream.
     * \param debug The print mode.
     */
    void print(const Function *func, std::ostream &os, bool debug);

    /**
     * \brief Prints the given module to the output stream.
     * \param m The module to be printed.
     * \param os The output stream.
     * \param debug The print mode.
     */
    void print(const Module *m, std::ostream &os, bool debug);

    /**
     * @brief Generate the dot text of \p func to stream \p os, which can be used by graphviz.
     * 
     * @param func The function to be printed.
     * @param os The output stream.
     * @param debug The print mode.
     */
    void gen_dot_cfg(const Function* func, std::ostream &os, bool debug);

    /**
     * @brief Generate dot text of \p func to file.
     *  
     * Note that this method will create a new file with \p name , if the file does not exist.
     * 
     * @param func The function to be printed.
     * @param name The name of target file.
     * @param debug The print mode.
     */
    void gen_dot_cfg(const Function* func, const std::string& name, bool debug);

private:
    std::unordered_map<const Value*, int> val_to_num;

    std::string op_to_str(ValueKind kind);
    std::string ty_to_str(Type *ty);
    std::string val_to_str(const Value *v);
    std::string inst_to_str(const Inst* inst);
};




#endif /* PCC_IR_CORE_IRPRINTER_H */
