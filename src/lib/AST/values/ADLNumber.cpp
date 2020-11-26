/* ADLNUMBER.cpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 16:14:03
 * Last edited:
 *   26/11/2020, 12:27:03
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLNumber class is used to represent integral numbers in the
 *   Abstract Syntax Tree (AST) of the ADL. Note that it doesn't actually
 *   parse the value itself; this is left for the Tokenizer.
 * 
 *   The ADLNumber extends from the ADLValue class.
**/

#include "ADLNumber.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLNUMBER CLASS *****/

/* Constructor for the ADLNumber class, which takes a breadcrumb trail of filenames where this node originated from, the line number where this node originated from, the column number where it originated from and the value this node represents. */
ADLNumber::ADLNumber(const std::vector<std::string>& filenames, size_t line, size_t col, long value) :
    ADLLeaf(NodeType::number, filenames, line, col),
    value(value)
{}



/* Allows the ADLNumber to be copied polymorphically. */
ADLNumber* ADLNumber::copy() const { return new ADLNumber(*this); }
