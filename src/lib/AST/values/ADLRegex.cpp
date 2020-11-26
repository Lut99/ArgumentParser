/* ADLREGEX.cpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 16:09:41
 * Last edited:
 *   26/11/2020, 12:27:16
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLRegex class is used to represent a Regex-expression in the AST.
 *   In particular, it represents a value-regex, not a compile-result regex
 *   (although it does wrap them).
 * 
 *   Note: for now, it stores the Regex as a string, but this should change to
 *   an already parsed regex as soon as the Tokenizer supports this.
**/

#include "ADLRegex.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLREGEX CLASS *****/

/* Constructor for the ADLRegex class, which takes a breadcrumb trail of filenames where this node originated from, the line number where this node originated from, the column number where it originated from and the raw regex value. */
ADLRegex::ADLRegex(const std::vector<std::string>& filenames, size_t line, size_t col, const std::string& value) :
    ADLLeaf(NodeType::regex, filenames, line, col),
    value(value)
{}



/* Allows the ADLRegex to be copied polymorphically. */
ADLRegex* ADLRegex::copy() const { return new ADLRegex(*this); }
