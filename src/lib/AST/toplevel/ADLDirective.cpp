/* ADLDIRECTIVE.cpp
 *   by Lut99
 *
 * Created:
 *   19/11/2020, 14:40:37
 * Last edited:
 *   27/11/2020, 14:58:04
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ADLDirective class, which is a special type of node that
 *   represents compiler directives. Will therefore likely be removed in
 *   one of the first AST traversals.
**/

#include "ADLDirective.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLDIRECTIVE CLASS *****/

/* Constructor for the ADLDirective class, which takes a breadcrumb trail of filenames where this node originated from, a DebugInfo struct linking this node to a location in the source file, the directive itself and optionally a list of values following it. */
ADLDirective::ADLDirective(const std::vector<std::string>& filenames, const DebugInfo& debug, const std::string& directive, ADLValues* values) :
    ADLBranch(NodeType::directive, filenames, debug, 1, NodeType::values),
    directive(directive)
{
    // Add it if the values are given
    if (values != nullptr) { this->add_node(values); }
}



/* Prints the ADLDirective to the given output stream, reflecting the AST structure. */
std::ostream& ADLDirective::print(std::ostream& os) const {
    // Print our directive, preceded by a dot
    os << '.' << this->directive;
    // Add any values
    if (this->children.size() == 1) {
        os << ' ';
        this->children[0]->print(os);
    }
    // Close off with a newline and we're done
    return os << endl;
}



/* Allows the ADLDirective to be copied polymorphically. */
ADLDirective* ADLDirective::copy() const { return new ADLDirective(*this); }
