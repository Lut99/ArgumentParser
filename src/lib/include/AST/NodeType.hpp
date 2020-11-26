/* NODE TYPE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:33:25
 * Last edited:
 *   26/11/2020, 16:38:16
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
#include <limits>

namespace ArgumentParser {
    /* The type to which cast NodeType if we want to perform numeric operations on it. */
    using nodetype_t = uint16_t;

    /* The NodeType enum, defining all types. */
    enum class NodeType {
        root = 0x1,
        file = 0x2,

        directive = 0x4,
        type_def = 0x8,
        positional = 0x10,
        option = 0x20,

        values = 0x40,
        string = 0x80,
        regex = 0x100,
        number = 0x200,
        decimal = 0x400
    };



    /* Map from each NodeType to a capitalized name. */
    const static std::unordered_map<NodeType, std::string> nodetype_name({
        { NodeType::root, "Root" },
        { NodeType::file, "File" },

        { NodeType::directive, "CompilerDirective" },
        { NodeType::type_def, "TypeDefinition" },
        { NodeType::positional, "Positional" },
        { NodeType::option, "Option" },

        { NodeType::values, "Values" },
        { NodeType::string, "String" },
        { NodeType::regex, "Regex" },
        { NodeType::number, "Number" },
        { NodeType::decimal, "Decimal" }
    });



    /* Given a NodeType that may consist of multiple ones, tries to extract all the possible NodeTypes and pretty prints them in a string. */
    std::string extract_type_names(const NodeType nodes, const std::string& concat_word = "and");

    /* Overloads the &-operator for the NodeTypes. */
    inline constexpr nodetype_t operator&(NodeType n1, NodeType n2) {
        return (nodetype_t) n1 & (nodetype_t) n2;
    }
    /* Overloads the |-operator for the NodeTypes. */
    inline constexpr NodeType operator|(NodeType n1, NodeType n2) {
        return (NodeType) ((nodetype_t) n1 | (nodetype_t) n2);
    }



    /* Constant type denoting all value types. */
    static constexpr NodeType nt_values = NodeType::string | NodeType::regex | NodeType::number | NodeType::decimal;
    /* Constant type denoting all toplevel types. */
    static constexpr NodeType nt_toplevel = NodeType::directive;
    /* Constant type denoting all types. */
    static constexpr NodeType nt_all = (NodeType) ~((nodetype_t) 0);

}

#endif
