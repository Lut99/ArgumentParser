/* NODE TYPE.cpp
 *   by Lut99
 *
 * Created:
 *   26/11/2020, 16:37:23
 * Last edited:
 *   27/11/2020, 17:20:59
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Baseclass for all nodes in the ADL Abstract Syntax Tree (AST).
**/

#include <vector>
#include <sstream>

#include "NodeType.hpp"

using namespace std;
using namespace ArgumentParser;


/* Given a NodeType that may consist of multiple ones, tries to extract all the possible NodeTypes and pretty prints them in a string. */
std::string ArgumentParser::extract_type_names(const NodeType nodes, const std::string& concat_word) {
    // Try all of values of the enum to see if it's present
    std::vector<std::string> present;
    for (size_t i = 0; i < nodetype_name.size(); i++) {
        // Create the value-to-check
        nodetype_t nt = 0x1 << i;

        // Check if it occurs
        if ((nodetype_t) nodes & nt) {
            present.push_back(nodetype_name.at((NodeType) nt));
        }
    }

    // Check if there are only 0 or 1 nodes
    if (present.size() == 0) { return "nothing"; }
    else if (present.size() == 1) { return present[0]; }

    // Otherwise, print them concatenated
    std::stringstream sstr;
    for (size_t i = 0; i < present.size(); i++) {
        if (i > 0 && i < present.size() - 1) { sstr << ", "; }
        else if (i == present.size() - 1) { sstr << ' ' << concat_word << ' '; }
        sstr << present[i];
    }

    // Done, return the string
    return sstr.str();
}
