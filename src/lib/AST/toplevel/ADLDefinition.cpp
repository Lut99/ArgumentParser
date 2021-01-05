/* ADLDEFINITION.cpp
 *   by Lut99
 *
 * Created:
 *   04/01/2021, 18:08:55
 * Last edited:
 *   05/01/2021, 13:58:23
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Baseclass for all classes that derive from the toplevel. In
 *   particular, allows easier access and organisation of all classes that
 *   can have symbol tables and to their identifiers.
**/

#include <vector>

#include "ADLDefinition.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLDEFINITION CLASS *****/
/* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, an identifier of the definition and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
ADLDefinition::ADLDefinition(NodeType type, const DebugInfo& debug, ADLIdentifier* identifier, ADLConfigs* configs):
    ADLBranch(type, debug, nt_all),
    identifier(identifier),
    shortlabel(nullptr),
    longlabel(nullptr),
    configs(configs)
{
    // Add the identifier if it exists
    if (identifier != nullptr) { this->add_node((ADLNode*) identifier); }
    // Add the list of configs if it exists
    if (configs != nullptr) { this->add_node((ADLNode*) configs); }
}

/* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, a whitelist of NodeTypes to accept as children, an identifier of the definition and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
ADLDefinition::ADLDefinition(NodeType type, const DebugInfo& debug, NodeType whitelist, ADLIdentifier* identifier, ADLConfigs* configs) :
    ADLBranch(type, debug, whitelist),
    identifier(identifier),
    shortlabel(nullptr),
    longlabel(nullptr),
    configs(configs)
{
    // Add the identifier if it exists
    if (identifier != nullptr) { this->add_node((ADLNode*) identifier); }
    // Add the list of configs if it exists
    if (configs != nullptr) { this->add_node((ADLNode*) configs); }
}

/* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, the maximum number of children, an identifier of the definition and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
ADLDefinition::ADLDefinition(NodeType type, const DebugInfo& debug, size_t max_children, ADLIdentifier* identifier, ADLConfigs* configs):
    ADLBranch(type, debug, max_children, nt_all),
    identifier(identifier),
    shortlabel(nullptr),
    longlabel(nullptr),
    configs(configs)
{
    // Add the identifier if it exists
    if (identifier != nullptr) { this->add_node((ADLNode*) identifier); }
    // Add the list of configs if it exists
    if (configs != nullptr) { this->add_node((ADLNode*) configs); }
}

/* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, the maximum number of children, a whitelist of NodeTypes to accept as children, an identifier of the definition and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
ADLDefinition::ADLDefinition(NodeType type, const DebugInfo& debug, size_t max_children, NodeType whitelist, ADLIdentifier* identifier, ADLConfigs* configs):
    ADLBranch(type, debug, max_children, whitelist),
    identifier(identifier),
    shortlabel(nullptr),
    longlabel(nullptr),
    configs(configs)
{
    // Add the identifier if it exists
    if (identifier != nullptr) { this->add_node((ADLNode*) identifier); }
    // Add the list of configs if it exists
    if (configs != nullptr) { this->add_node((ADLNode*) configs); }
}

/* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, an optional shortlabel for the node, and optional longlabel and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
ADLDefinition::ADLDefinition(NodeType type, const DebugInfo& debug, ADLIdentifier* shortlabel, ADLIdentifier* longlabel, ADLConfigs* configs) :
    ADLBranch(type, debug, nt_all),
    identifier(nullptr),
    shortlabel(shortlabel),
    longlabel(longlabel),
    configs(configs)
{
    // Add the shortlabel if it exists
    if (shortlabel != nullptr) { this->add_node((ADLNode*) shortlabel); }
    // Add the longlabel if it exists
    if (longlabel != nullptr) { this->add_node((ADLNode*) longlabel); }
    // Add the list of configs if it exists
    if (configs != nullptr) { this->add_node((ADLNode*) configs); }
}

/* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, a whitelist of NodeTypes to accept as children, an optional shortlabel for the node, and optional longlabel and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
ADLDefinition::ADLDefinition(NodeType type, const DebugInfo& debug, NodeType whitelist, ADLIdentifier* shortlabel, ADLIdentifier* longlabel, ADLConfigs* configs) :
    ADLBranch(type, debug, whitelist),
    identifier(nullptr),
    shortlabel(shortlabel),
    longlabel(longlabel),
    configs(configs)
{
    // Add the shortlabel if it exists
    if (shortlabel != nullptr) { this->add_node((ADLNode*) shortlabel); }
    // Add the longlabel if it exists
    if (longlabel != nullptr) { this->add_node((ADLNode*) longlabel); }
    // Add the list of configs if it exists
    if (configs != nullptr) { this->add_node((ADLNode*) configs); }
}

/* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, the maximum number of children, an optional shortlabel for the node, and optional longlabel and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
ADLDefinition::ADLDefinition(NodeType type, const DebugInfo& debug, size_t max_children, ADLIdentifier* shortlabel, ADLIdentifier* longlabel, ADLConfigs* configs) :
    ADLBranch(type, debug, max_children, nt_all),
    identifier(nullptr),
    shortlabel(shortlabel),
    longlabel(longlabel),
    configs(configs)
{
    // Add the shortlabel if it exists
    if (shortlabel != nullptr) { this->add_node((ADLNode*) shortlabel); }
    // Add the longlabel if it exists
    if (longlabel != nullptr) { this->add_node((ADLNode*) longlabel); }
    // Add the list of configs if it exists
    if (configs != nullptr) { this->add_node((ADLNode*) configs); }
}

/* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, the maximum number of children, a whitelist of NodeTypes to accept as children, an optional shortlabel for the node, and optional longlabel and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
ADLDefinition::ADLDefinition(NodeType type, const DebugInfo& debug, size_t max_children, NodeType whitelist, ADLIdentifier* shortlabel, ADLIdentifier* longlabel, ADLConfigs* configs) :
    ADLBranch(type, debug, max_children, whitelist),
    identifier(nullptr),
    shortlabel(shortlabel),
    longlabel(longlabel),
    configs(configs)
{
    // Add the shortlabel if it exists
    if (shortlabel != nullptr) { this->add_node((ADLNode*) shortlabel); }
    // Add the longlabel if it exists
    if (longlabel != nullptr) { this->add_node((ADLNode*) longlabel); }
    // Add the list of configs if it exists
    if (configs != nullptr) { this->add_node((ADLNode*) configs); }
}

/* Copy constructor for the ADLDefinition class. */
ADLDefinition::ADLDefinition(const ADLDefinition& other) :
    ADLBranch(other),
    identifier(nullptr),
    shortlabel(nullptr),
    longlabel(nullptr),
    configs(nullptr)
{
    // Re-write the identifier pointers
    vector<ADLNode*> nodes = this->get_nodes(NodeType::identifier);
    for (size_t i = 0; i < nodes.size(); i++) {
        ADLIdentifier* node = (ADLIdentifier*) nodes[i];
        if (node->identifier[0] == '-') {
            if (node->identifier.size() > 1 && node->identifier[1] == '-') {
                // Longlabel
                this->longlabel = node;
            } else {
                // Shortlabel
                this->shortlabel = node;
            }
        } else {
            // Normal identifier
            this->identifier = node;
        }
    }

    // Re-write the config pointer
    nodes = this->get_nodes(NodeType::configs);
    if (nodes.size() > 0) {
        this->configs = (ADLConfigs*) nodes[0];
    }
}



/* Adds a node as child of this node. */
void ADLDefinition::add_node(ADLNode* node) {
    // Call the parent
    ADLBranch::add_node(node);

    // Possibly link the configs list to it
    if (node->type == NodeType::configs) {
        this->configs = (ADLConfigs*) this->children[this->children.size() - 1];
    }
}
