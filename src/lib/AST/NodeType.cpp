/* NODE TYPE.cpp
 *   by Lut99
 *
 * Created:
 *   26/11/2020, 16:37:23
 * Last edited:
 *   26/11/2020, 16:38:42
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Baseclass for all nodes in the ADL Abstract Syntax Tree (AST).
**/

#include "NodeType.hpp"

using namespace std;
using namespace ArgumentParser;


/* Given a NodeType that may consist of multiple ones, tries to extract all the possible NodeTypes and pretty prints them in a string. */
std::string ArgumentParser::extract_type_names(const NodeType nodes, const std::string& concat_word) {
    // Try all of values of the enum to see if it's present
    std::stringstream sstr;
    for (size_t i = 0; i < nodetype_name.size(); i++) {
        // Create the value-to-check
        nodetype_t nt = 0x1 << i;

        // Check if it occurs
        if ((nodetype_t) nodes & nt) {
            // It does; so print the appropriate inter-node first
            if (i < nodetype_name.size() - 1) { sstr << ", "; }
            else { sstr << ' ' << concat_word << ' '; }

            // Add the name of the NodeType itself
            sstr << nodetype_name.at((NodeType) nt);
        }
    }

    // Done, return the string
    return sstr.str();
}
