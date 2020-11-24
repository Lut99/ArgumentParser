/* ADLSTRING.cpp
 *   by Lut99
 *
 * Created:
 *   24/11/2020, 23:19:24
 * Last edited:
 *   24/11/2020, 23:24:17
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLString class, which is used to represent raw
 *   string values in the Abstract Syntax Tree (AST) of the ADL. Derived from
 *   the ADLValue class.
**/

#include "ADLString.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLSTRING CLASS *****/

/* Constructor for the ADLString class, which takes a breadcrumb trail of filenames where this node originated from, the line number where this node originated from, the column number where it originated from, this node's parent node and the raw string value. */
ADLString::ADLString(const std::vector<std::string>& filenames, size_t line, size_t col, ADLNode* parent, const std::string& value) :
    ADLValue(NodeType::string, filenames, line, col, parent),
    value(value)
{}



/* Allows the ADLString to be copied polymorphically. */
ADLString* ADLString::copy() const { return new ADLString(*this); }
