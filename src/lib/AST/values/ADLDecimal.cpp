/* ADLDECIMAL.cpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 16:14:03
 * Last edited:
 *   26/11/2020, 15:40:36
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

/* Constructor for the ADLDecimal class, which takes a breadcrumb trail of filenames where this node originated from, a DebugInfo struct linking this node to a location in the source file and the value this node represents. */
ADLDecimal::ADLDecimal(const std::vector<std::string>& filenames, const DebugInfo& debug, long value) :
    ADLLeaf(NodeType::number, filenames, debug),
    value(value)
{}



/* Allows the ADLDecimal to be copied polymorphically. */
ADLDecimal* ADLDecimal::copy() const { return new ADLDecimal(*this); }
