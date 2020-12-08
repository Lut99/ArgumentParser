/* ADLBRANCH.cpp
 *   by Lut99
 *
 * Created:
 *   26/11/2020, 11:39:35
 * Last edited:
 *   08/12/2020, 20:44:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLBranch class, which is used as a base for
 *   all nodes that have children themselves. This way, memory management
 *   and traversal is centralized, where the nodes can define access to the
 *   children array using inline functions or references.
**/

#include "ASTExceptions.hpp"
#include "ADLBranch.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLBRANCH CLASS *****/

/* Constructor for the ADLBranch class, which takes the type of the derived node, a DebugInfo struct linking this node to a location in the source file and optionally a NodeType denoting all accepted children types. */
ADLBranch::ADLBranch(NodeType type, const DebugInfo& debug, NodeType whitelist) :
    ADLNode(type, debug),
    has_max(false),
    whitelist(whitelist)
{}

/* Constructor for the ADLBranch class, which takes the type of the derived node, a DebugInfo struct linking this node to a location in the source file, the maximum number of children to allow and optionally a NodeType denoting all accepted children types. */
ADLBranch::ADLBranch(NodeType type, const DebugInfo& debug, size_t max, NodeType whitelist) :
    ADLNode(type, debug),
    has_max(true),
    max(max),
    whitelist(whitelist)
{}

/* Copy constructor for the ADLBranch class. */
ADLBranch::ADLBranch(const ADLBranch& other) :
    ADLNode(other),
    has_max(other.has_max),
    max(other.max),
    whitelist(other.whitelist)
{
    // Copy all the children nodes
    this->children.reserve(other.children.size());
    for (size_t i = 0; i < other.children.size(); i++) {
        this->add_node(other.children[i]->copy());
    }
}

/* Move constructor for the ADLBranch class. */
ADLBranch::ADLBranch(ADLBranch&& other) :
    ADLNode(other),
    has_max(other.has_max),
    max(other.max),
    whitelist(other.whitelist)
{
    // Move all the children nodes to us
    this->children.reserve(other.children.size());
    for (size_t i = 0; i < other.children.size(); i++) {
        this->add_node(other.children[i]);
    }

    // Clear the other's list, to avoid it deallocating them
    other.children.clear();
}

/* Virtual destructor for the ADLBranch class. */
ADLBranch::~ADLBranch() {
    // Simply delete all children we have
    for (size_t i = 0; i < this->children.size(); i++) {
        delete this->children[i];
    }
}



/* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLBranch::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*)) {
    std::string context = "ADL" + nodetype_name.at(this->type) + "::_traverse_recurse(stateless)";

    // Simply loop through all children, possibly replacing them
    for (size_t i = 0; i < this->children.size(); i++) {
        ADLNode* new_node = this->children[i]->traverse(trav_id, node_types, trav_func);

        // Replace it if it changed
        if (new_node != this->children[i]) {
            // If it's replaced, be sure that it's a whitelisted node
            if (!(new_node->type & this->whitelist)) {
                throw Exceptions::IllegalChildException(context, trav_id, this->whitelist, new_node->type);
            }

            // If it was valid, continue replacing it
            delete this->children[i];
            this->children[i] = new_node;
            new_node->parent = this;
        }
    }
}

/* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLBranch::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state) {
    std::string context = "ADL" + nodetype_name.at(this->type) + "::_traverse_recurse()";

    // Simply loop through all children, possibly replacing them
    for (size_t i = 0; i < this->children.size(); i++) {
        ADLNode* new_node = this->children[i]->traverse(trav_id, node_types, trav_func, state);

        // Replace it if it changed
        if (new_node != this->children[i]) {
            // If it's replaced, be sure that it's a whitelisted node
            if (!(new_node->type & this->whitelist)) {
                throw Exceptions::IllegalChildException(context, trav_id, this->whitelist, new_node->type);
            }

            // If it was valid, continue replacing it
            delete this->children[i];
            this->children[i] = new_node;
            new_node->parent = this;
        }
    }
}



/* Adds a node as child of this node. */
void ADLBranch::add_node(ADLNode* node) {
    std::string context = "ADL" + nodetype_name.at(this->type) + "::add_node()";

    // Check if we have a maximum and, if so, if we're at it
    if (this->has_max && this->children.size() == this->max) {
        throw Exceptions::MaximumChildrenException(context, "???", this->max);
    }
    // Make sure the node is of a valid type
    if (!(node->type & this->whitelist)) {
        throw Exceptions::IllegalChildException(context, "???", this->whitelist, node->type);
    }

    // Finally, just add the node and set its parent
    this->children.push_back(node);
    node->parent = this;
}
