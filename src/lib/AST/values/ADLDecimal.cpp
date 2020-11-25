/* ADLDECIMAL.cpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 16:14:03
 * Last edited:
 *   25/11/2020, 17:21:58
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLDecimal class is used to represent floating-point numbers in the
 *   Abstract Syntax Tree (AST) of the ADL. Note that it doesn't actually
 *   parse the value itself; this is left for the Tokenizer.
 * 
 *   The ADLDecimal extends from the ADLValue class.
**/

#include "ADLDecimal.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLDECIMAL CLASS *****/

/* Constructor for the ADLDecimal class, which takes a breadcrumb trail of filenames where this node originated from, the line number where this node originated from, the column number where it originated from and the value this node represents. */
ADLDecimal::ADLDecimal(const std::vector<std::string>& filenames, size_t line, size_t col, long value) :
    ADLValue(NodeType::number, filenames, line, col),
    value(value)
{}



/* Allows the ADLDecimal to be copied polymorphically. */
ADLDecimal* ADLDecimal::copy() const { return new ADLDecimal(*this); }
