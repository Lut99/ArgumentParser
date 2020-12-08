/* ADLCONFIGS.cpp
 *   by Lut99
 *
 * Created:
 *   27/11/2020, 16:16:23
 * Last edited:
 *   08/12/2020, 20:45:51
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLConfigs class, which is used as a wrapper
 *   around ADLConfig nodes to group them in one array.
**/

#include "ADLConfigs.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLCONFIGS CLASS *****/

/* Constructor for the ADLConfigs class, which takes debug information on this node's origin and optionally already an ADLConfig node. */
ADLConfigs::ADLConfigs(const DebugInfo& debug, ADLConfig* config) :
    ADLBranch(NodeType::configs, debug, NodeType::config)
{
    // Also add config to our internal list of nodes
    this->add_node((ADLNode*) config);
}



/* Prints the type definition and all its configuration parameters to the given output stream. */
std::ostream& ADLConfigs::print(std::ostream& os) const {
    // Print each of the configurations, prepended by an indent
    for (size_t i = 0; i < this->children.size(); i++) {
        os << "    ";
        this->children[i]->print(os) << endl;
    }
    // Done, return
    return os;
}



/* Allows the ADLConfigs to be copied polymorphically. */
ADLConfigs* ADLConfigs::copy() const { return new ADLConfigs(*this); }

