/* ADLMETA.cpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 17:29:00
 * Last edited:
 *   05/01/2021, 13:38:02
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLMeta class, which can be used to represent
 *   the near namespace where the user can set global values and define
 *   custom variables to re-use elsewhere.
**/

#include "ADLMeta.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLMETA CLASS *****/

/* Constructor for the ADLMeta class, which takes debug information linking this node back to the source file, an identifier to store the 'meta' in and optionally properties to store in the meta namespace. */
ADLMeta::ADLMeta(const DebugInfo& debug, ADLIdentifier* id, ADLConfigs* configs) :
    ADLDefinition(NodeType::meta, debug, 2, NodeType::identifier | NodeType::configs, id, configs)
{}

/* Prints all the toplevel properties, reflecting the AST structure. */
std::ostream& ADLMeta::print(std::ostream& os) const {
    this->children[0]->print(os);
    os << " {";

    // Print each of the configuration parameters, if any, preceded by a space
    if (this->children.size() == 2) {
        os << endl;
        this->children[1]->print(os);
    }
    
    // Close with a bracket and newline, then we're done
    return os << '}' << endl;
}

/* Allows the ADLMeta to be copied polymorphically. */
ADLMeta* ADLMeta::copy() const { return new ADLMeta(*this); }
