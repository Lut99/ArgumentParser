/* ADLVALUES.cpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 18:11:08
 * Last edited:
 *   26/11/2020, 12:25:31
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

/* Constructor for the ADLValues class, which takes a trail of breadcrumbs for files we're parsing, the line number where the symbol originated and a matching column number.*/
ADLValues::ADLValues(const std::vector<std::string>& filenames, size_t line, size_t col) :
    ADLBranch(NodeType::values, filenames, line, col, nt_values)
{}



/* Allows the ADLValues to be copied polymorphically. */
ADLValues* ADLValues::copy() const { return new ADLValues(*this); }
