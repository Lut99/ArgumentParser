/* ADLBOOLEAN.cpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 18:02:33
 * Last edited:
 *   03/12/2020, 18:03:41
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLBoolean class is used to represent boolean numbers in the
 *   Abstract Syntax Tree (AST) of the ADL. Note that it doesn't actually
 *   parse the value itself; this is left for the Tokenizer.
 * 
 *   The ADLBoolean extends from the ADLValue class.
**/

#include "ADLBoolean.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLBOOLEAN CLASS *****/

/* Constructor for the ADLBoolean class, which takes a breadcrumb trail of filenames where this node originated from, a DebugInfo struct linking this node to a location in the source file and the value this node represents. */
ADLBoolean::ADLBoolean(const std::vector<std::string>& filenames, const DebugInfo& debug, bool value) :
    ADLLeaf(NodeType::number, filenames, debug),
    value(value)
{}



/* Prints the decimal to the given output stream. */
std::ostream& ADLBoolean::print(std::ostream& os) const {
    // Just output the value
    return os << (this->value ? "true" : "false");
}



/* Allows the ADLBoolean to be copied polymorphically. */
ADLBoolean* ADLBoolean::copy() const { return new ADLBoolean(*this); }
