/* WARNING TYPES.cpp
 *   by Lut99
 *
 * Created:
 *   13/12/2020, 14:13:05
 * Last edited:
 *   13/12/2020, 14:15:30
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains all the warning types that have been defined in the
 *   ADL compiler.
**/

#include <vector>
#include <sstream>

#include "WarningTypes.hpp"

using namespace std;
using namespace ArgumentParser;
using namespace ArgumentParser::Exceptions;


/* Given a WarningType that may consist of multiple ones, tries to extract all the possible WarningTypes and pretty prints them in a string. */
std::string Exceptions::extract_type_names(const WarningType warnings, const std::string& concat_word) {
    // Try all of values of the enum to see if it's present
    std::vector<std::string> present;
    for (size_t i = 0; i < warningtype_names.size(); i++) {
        // Create the value-to-check
        warningtype_t wt = 0x1 << i;

        // Check if it occurs
        if ((warningtype_t) warnings & wt) {
            present.push_back(warningtype_names.at((WarningType) wt));
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
