/* ADLSTRING.cpp
 *   by Lut99
 *
 * Created:
 *   24/11/2020, 23:19:24
 * Last edited:
 *   27/11/2020, 14:56:36
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLString class, which is used to represent raw
 *   string values in the Abstract Syntax Tree (AST) of the ADL. Derived from
 *   the ADLValue class.
**/

#include "ADLString.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLSTRING CLASS *****/

/* Constructor for the ADLString class, which takes a breadcrumb trail of filenames where this node originated from, a DebugInfo struct linking this node to a location in the source file and the raw string value. */
ADLString::ADLString(const std::vector<std::string>& filenames, const DebugInfo& debug, const std::string& value) :
    ADLLeaf(NodeType::string, filenames, debug),
    value(value)
{}



/* Prints the string to the given output stream. */
std::ostream& ADLString::print(std::ostream& os) const {
    // Just output the value in between brackets
    return os << '"' << this->value << '"';
}



/* Allows the ADLString to be copied polymorphically. */
ADLString* ADLString::copy() const { return new ADLString(*this); }
