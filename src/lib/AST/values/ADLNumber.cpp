/* ADLNUMBER.cpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 16:14:03
 * Last edited:
 *   08/12/2020, 20:50:38
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLNumber class is used to represent integral numbers in the
 *   Abstract Syntax Tree (AST) of the ADL. Note that it doesn't actually
 *   parse the value itself; this is left for the Tokenizer.
 * 
 *   The ADLNumber extends from the ADLValue class.
**/

#include "ADLNumber.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLNUMBER CLASS *****/

/* Constructor for the ADLNumber class, which takes a DebugInfo struct linking this node to a location in the source file and the value this node represents. */
ADLNumber::ADLNumber(const DebugInfo& debug, long value) :
    ADLLeaf(NodeType::number, debug),
    value(value)
{}



/* Prints the number to the given output stream. */
std::ostream& ADLNumber::print(std::ostream& os) const {
    // Just output the value
    return os << this->value;
}



/* Allows the ADLNumber to be copied polymorphically. */
ADLNumber* ADLNumber::copy() const { return new ADLNumber(*this); }
