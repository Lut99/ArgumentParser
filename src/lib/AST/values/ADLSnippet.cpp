/* ADLSNIPPET.cpp
 *   by Lut99
 *
 * Created:
 *   01/12/2020, 12:57:46
 * Last edited:
 *   08/12/2020, 20:51:39
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "ADLSnippet.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLSNIPPET CLASS *****/

/* Constructor for the ADLSnippet class, which takes a DebugInfo struct linking the node back to the source file and the C++-code that this node wraps. */
ADLSnippet::ADLSnippet(const DebugInfo& debug, const std::string& code) :
    ADLLeaf(NodeType::snippet, debug),
    code(code)
{}



/* Prints the code snippet to the given output stream. For readability, the code itself isn't pasted (as that's less relevant for parsing anyway). */
std::ostream& ADLSnippet::print(std::ostream& os) const {
    // Just print some placeholder
    return os << "++{ ... }++";
}



/* Allows the ADLSnippet to be copied polymorphically. */
ADLSnippet* ADLSnippet::copy() const { return new ADLSnippet(*this); }
