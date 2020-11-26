/* ADLVALUES.cpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 18:11:08
 * Last edited:
 *   26/11/2020, 15:37:58
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains a class that is used to group multiple, consecutive
 *   ADLValue symbols into one list of them.
**/

#include "ADLValues.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLVALUES CLASS *****/

/* Constructor for the ADLValues class, which takes a trail of breadcrumbs for files we're parsing, a DebugInfo struct linking this node to a location in the source file and at least one value. */
ADLValues::ADLValues(const std::vector<std::string>& filenames, const DebugInfo& debug, ADLNode* value) :
    ADLBranch(NodeType::values, filenames, debug, nt_values)
{
    // Immediately add the value
    this->add_node(value);
}



/* Allows the ADLValues to be copied polymorphically. */
ADLValues* ADLValues::copy() const { return new ADLValues(*this); }
