/* ADLMETA.cpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 17:29:00
 * Last edited:
 *   03/12/2020, 17:31:17
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

/* Constructor for the ADLMeta class, which takes a breadcrumb trail of filenames, debug information linking this node back to the source file and optionally properties to store in the meta namespace. */
ADLMeta::ADLMeta(const std::vector<std::string>& filenames, const DebugInfo& debug, ADLConfigs* configs) :
    ADLBranch(NodeType::meta, filenames, debug, 1, NodeType::configs)
{
    // Only add the configs if they're not nullptrs
    if (configs != nullptr) { this->add_node((ADLNode*) configs); }
}

/* Prints all the toplevel properties, reflecting the AST structure. */
std::ostream& ADLMeta::print(std::ostream& os) const {
    os << "META {";

    // Print each of the configuration parameters, if any, preceded by a space
    if (this->children.size() == 1) {
        os << endl;
        this->children[0]->print(os);
    }
    
    // Close with a bracket and newline, then we're done
    return os << '}' << endl;
}

/* Allows the ADLMeta to be copied polymorphically. */
ADLMeta* ADLMeta::copy() const { return new ADLMeta(*this); }
