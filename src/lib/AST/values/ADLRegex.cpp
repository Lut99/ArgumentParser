/* ADLREGEX.cpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 16:09:41
 * Last edited:
 *   27/11/2020, 14:56:22
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLRegex class is used to represent a Regex-expression in the AST.
 *   In particular, it represents a value-regex, not a compile-result regex
 *   (although it does wrap them).
 * 
 *   Note: for now, it stores the Regex as a string, but this should change to
 *   an already parsed regex as soon as the Tokenizer supports this.
**/

#include "ADLRegex.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLREGEX CLASS *****/

/* Constructor for the ADLRegex class, which takes a breadcrumb trail of filenames where this node originated from, a DebugInfo struct linking this node to a location in the source file and the raw regex value. */
ADLRegex::ADLRegex(const std::vector<std::string>& filenames, const DebugInfo& debug, const std::string& value) :
    ADLLeaf(NodeType::regex, filenames, debug),
    value(value)
{}



/* Prints the regex-expression to the given output stream. */
std::ostream& ADLRegex::print(std::ostream& os) const {
    // Just output the value in between brackets and with an 'r'
    return os << "r\"" << this->value << '"';
}



/* Allows the ADLRegex to be copied polymorphically. */
ADLRegex* ADLRegex::copy() const { return new ADLRegex(*this); }
