/* ADLNODE.cpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:39:22
 * Last edited:
 *   18/11/2020, 20:41:37
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Baseclass for all nodes in the ADL Abstract Syntax Tree (AST).
**/

#include "ADLNode.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLNODE CLASS *****/

/* Constructor for the ADLNode, which takes a type and a parent node. */
ADLNode::ADLNode(NodeType type, ADLNode* parent) :
    type(type),
    parent(parent)
{}



/* Define the swap operator for the ADLNode so children may use it. */
void ArgumentParser::swap(ADLNode& n1, ADLNode& n2) {
    using std::swap;

    swap(n1.type, n2.type);
    swap(n1.parent, n2.parent);
}
