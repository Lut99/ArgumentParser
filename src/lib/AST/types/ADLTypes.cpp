/* ADLTYPES.cpp
 *   by Lut99
 *
 * Created:
 *   12/1/2020, 6:12:33 PM
 * Last edited:
 *   11/02/2021, 17:36:50
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
ADLTypes::ADLTypes(const DebugInfo& debug, ADLIdentifier* type_id) :
    ADLBranch(NodeType::types, debug, NodeType::identifier)
{
    // Only add the identifier if it isn't empty
    if (type_id != nullptr) { this->add_node(type_id); }
}

/* Prints the list of identifiers to the given output stream. */
std::ostream& ADLTypes::print(std::ostream& os) const {
    // Print all internal ids, separated by spaces and wrapped in '<>'
    for (size_t i = 0; i < this->children.size(); i++) {
        if (i > 0) { os << ' '; }
        this->children[i]->print(os);
    }
    return os;
}

/* Allows the ADLTypes to be copied polymorphically. */
ADLTypes* ADLTypes::copy() const { return new ADLTypes(*this); }
