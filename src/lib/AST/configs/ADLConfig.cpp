/* ADLCONFIG.cpp
 *   by Lut99
 *
 * Created:
 *   27/11/2020, 16:24:34
 * Last edited:
 *   27/11/2020, 17:13:18
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLConfig class, which stores a single
 *   configuration argument for either type of argument definitions. Always
 *   stores a keyword, and then one or multiple values.
**/

#include "ADLConfig.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLCONFIG CLASS *****/

/* Constructor for the ADLConfig class, which takes a breadcrumb trail of filenames, debug information on the node's origin, the name of the parameter configured and a list of values as ADLValues. */
ADLConfig::ADLConfig(const std::vector<std::string>& filenames, const DebugInfo& debug, const std::string& param, ADLValues* values) :
    ADLBranch(NodeType::config, filenames, debug, 1, NodeType::values),
    param(param)
{
    // Add the values node
    this->add_node(values);
}



/* Prints the configuration as the parameter name, all values and then a semicolon to the given output stream. */
std::ostream& ADLConfig::print(std::ostream& os) const {
    // First, print the parameter name + space
    os << this->param << ' ';
    // Then, print all values
    this->children[0]->print(os);
    // End with semicolon; done
    return os << ';';
}



/* Allows the ADLConfig to be copied polymorphically. */
ADLConfig* ADLConfig::copy() const { return new ADLConfig(*this); }
