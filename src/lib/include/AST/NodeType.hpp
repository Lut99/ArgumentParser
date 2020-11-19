/* NODE TYPE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:33:25
 * Last edited:
 *   19/11/2020, 14:44:24
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file simply defines all node types that nodes can become.
**/

#ifndef NODETYPE_HPP
#define NODETYPE_HPP

#include <string>
#include <sstream>
#include <unordered_map>

namespace ArgumentParser {
    /* The type to which cast NodeType if we want to perform numeric operations on it. */
    using nodetype_t = int;

    /* The NodeType enum, defining all types. */
    enum class NodeType {
        root = 0x1,
        file = 0x2,
        directive = 0x4,
        type_def = 0x8,
        positional = 0x10,
        option = 0x20
    };

    /* Map from each NodeType to a capitalized name. */
    const static std::unordered_map<NodeType, std::string> nodetype_name({
        { NodeType::root, "Root" },
        { NodeType::file, "File" },
        { NodeType::directive, "CompilerDirective" },
        { NodeType::type_def, "TypeDefinition" },
        { NodeType::positional, "Positional" },
        { NodeType::option, "Option" }
    });

    /* Given a NodeType that may consist of multiple ones, tries to extract all the possible NodeTypes and pretty prints them in a string. */
    std::string extract_type_names(const NodeType& nodes) {
        // Try all of values of the enum to see if it's present
        std::stringstream sstr;
        for (size_t i = 0; i < nodetype_name.size(); i++) {
            // Create the value-to-check
            nodetype_t nt = 0x1 << i;

            // Check if it occurs
            if ((nodetype_t) nodes & nt) {
                // It does; so print the appropriate inter-node first
                if (i < nodetype_name.size() - 1) { sstr << ", "; }
                else { sstr << " and "; }

                // Add the name of the NodeType itself
                sstr << nodetype_name.at((NodeType) nt);
            }
        }

        // Done, return the string
        return sstr.str();
    }

}

#endif
