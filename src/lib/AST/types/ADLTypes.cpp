/* ADLTYPES.cpp
 *   by Lut99
 *
 * Created:
 *   12/1/2020, 6:12:33 PM
 * Last edited:
 *   08/12/2020, 20:49:02
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File that contains the ADLTypes class, which is used as an array for
 *   type identifiers in a Positional or Option definition.
**/

#include "ADLTypes.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLTYPES CLASS *****/

/* Constructor for the ADLTypes class, which takes a DebugInfo struct that links this node to the source file and optionally already one type identifier (as raw value). */
ADLTypes::ADLTypes(const DebugInfo& debug, const std::string& type_id) :
    ADLLeaf(NodeType::types, debug)
{
    // Only add the string if it isn't empty
    if (!type_id.empty()) { this->ids.push_back(type_id); }
}

/* Prints the list of identifiers to the given output stream. */
std::ostream& ADLTypes::print(std::ostream& os) const {
    // Print all internal ids, separated by spaces and wrapped in '<>'
    for (size_t i = 0; i < this->ids.size(); i++) {
        if (i > 0) { os << ' '; }
        os << '<' << this->ids[i] << '>';
    }
    return os;
}

/* Allows the ADLTypes to be copied polymorphically. */
ADLTypes* ADLTypes::copy() const { return new ADLTypes(*this); }
