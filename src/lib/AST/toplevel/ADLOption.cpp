/* ADLOPTION.cpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 16:15:55
 * Last edited:
 *   03/12/2020, 17:05:09
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "ADLOption.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLOPTION CLASS *****/

/* Constructor for the ADLOption class, which takes a breadcrumb trail of files, a DebugInfo struct linking this node to the source code, the shortlabel for this Option (empty to disable), the longlabel of this Option (empty to disable), an optional list of types that make up this Option as ADLTypes, optionally a list of configuration parameters for this Positional as ADLConfigs, whether or not this Option is optional, whether or not this Option's type may be omitted and whether or not this Positional is variadic. */
ADLOption::ADLOption(const std::vector<std::string>& filenames, const DebugInfo& debug, const std::string& shortlabel, const std::string& longlabel, ADLTypes* types, ADLConfigs* configs, bool is_optional, bool type_is_optional, bool is_variadic) :
    ADLBranch(NodeType::option, filenames, debug, 2, NodeType::types | NodeType::configs),
    shortlabel(shortlabel),
    longlabel(longlabel),
    optional(is_optional),
    type_optional(type_is_optional),
    variadic(is_variadic)
{
    // Only add  the types if they are given
    if (types != nullptr) { this->add_node((ADLNode*) types); }
    // Only add the values if they are given
    if (configs != nullptr) { this->add_node((ADLNode*) configs); }
}

/* Prints the Option definition and all its configuration parameters to the given output stream. */
std::ostream& ADLOption::print(std::ostream& os) const {
    // First, print the ids
    if (this->optional) { os << '['; }
    if (!this->shortlabel.empty() && !this->longlabel.empty()) {
        os << '-' << this->shortlabel << ", --" << this->longlabel;
    } else if (!this->shortlabel.empty()) {
        os << '-' << this->shortlabel;
    } else {
        os << "--" << this->longlabel;
    }
    if (this->optional) { os << ']'; }

    // Then, print the types (if any)
    if (this->children.size() >= 1 && this->children[0]->type == NodeType::types) {
        os << ' ';
        if (this->type_optional) { os << '['; }
        this->children[0]->print(os);
        if (this->variadic) { os << "..."; }
        if (this->type_optional) { os << ']'; }
    }

    // Print the starting bracket
    os << " {";

    // Print each of the configuration parameters, if any, preceded by a space
    if (this->children.size() == 2) {
        os << endl;
        this->children[1]->print(os);
    }
    
    // Close with a bracket and newline, then we're done
    return os << '}' << endl;
}

/* Allows the ADLOption to be copied polymorphically. */
ADLOption* ADLOption::copy() const { return new ADLOption(*this); }
