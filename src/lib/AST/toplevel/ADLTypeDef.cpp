/* ADLTYPE DEF.cpp
 *   by Lut99
 *
 * Created:
 *   27/11/2020, 16:03:01
 * Last edited:
 *   08/12/2020, 20:48:28
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the TypeDef class, which represents a custom type
 *   definition in the Abstract Syntax Tree (AST) of the ADL. It contains a
 *   list of configuration keywords (with values) as a single Configs-node.
 *   Value-wise, it carries not much more that it's typename.
**/

#include "ADLTypeDef.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLTYPEDEF CLASS *****/

/* Constructor for the ADLTypeDef class, which takes debug information about the node's origin, the type definition's identifier and optionally an ADLConfigs node. */
ADLTypeDef::ADLTypeDef(const DebugInfo& debug, const std::string& id, ADLConfigs* configs) :
    ADLBranch(NodeType::type_def, debug, 1, NodeType::configs),
    id(id)
{
    // Add the given configs node if it isn't empty
    if (configs != nullptr) { this->add_node((ADLNode*) configs); }
}



/* Prints the type definition and all its configuration parameters to the given output stream. */
std::ostream& ADLTypeDef::print(std::ostream& os) const {
    // Print the type definition header
    os << '<' << this->id << "> {";
    // Print each of the configuration parameters, if any, preceded by a space
    if (this->children.size() == 1) {
        os << endl;
        this->children[0]->print(os);
    }
    // Close with a bracket and newline, then we're done
    return os << '}' << endl;
}



/* Allows the ADLTypeDef to be copied polymorphically. */
ADLTypeDef* ADLTypeDef::copy() const { return new ADLTypeDef(*this); }
