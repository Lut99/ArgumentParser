/* ADLDECIMAL.cpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 16:14:03
 * Last edited:
 *   08/12/2020, 20:50:14
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLDecimal class is used to represent floating-point numbers in the
 *   Abstract Syntax Tree (AST) of the ADL. Note that it doesn't actually
 *   parse the value itself; this is left for the Tokenizer.
 * 
 *   The ADLDecimal extends from the ADLValue class.
**/

#include "ADLDecimal.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLDECIMAL CLASS *****/

/* Constructor for the ADLDecimal class, which takes a DebugInfo struct linking this node to a location in the source file and the value this node represents. */
ADLDecimal::ADLDecimal(const DebugInfo& debug, double value) :
    ADLLeaf(NodeType::number, debug),
    value(value)
{}



/* Prints the decimal to the given output stream. */
std::ostream& ADLDecimal::print(std::ostream& os) const {
    // Just output the value
    return os << this->value;
}



/* Allows the ADLDecimal to be copied polymorphically. */
ADLDecimal* ADLDecimal::copy() const { return new ADLDecimal(*this); }
