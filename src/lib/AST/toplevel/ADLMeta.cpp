/* ADLMETA.cpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 17:29:00
 * Last edited:
 *   11/02/2021, 17:55:01
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



/* Lets the Meta-node have a go at trying to store the given value/node for the given property key. Returns 'true' if it was successfull, or 'false' if no such property is relevant for this class. */
bool ADLMeta::set_property(const std::string& key, const std::string& value) {
    // For now, the Meta class accepts no keys
    return false;
}



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
