/* NODE TYPE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:33:25
 * Last edited:
 *   30/11/2020, 17:51:30
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file simply defines all node types that nodes can become.
**/

#ifndef NODETYPE_HPP
#define NODETYPE_HPP

#include <string>
#include <unordered_map>
#include <limits>

namespace ArgumentParser {
    /* The type to which cast NodeType if we want to perform numeric operations on it. */
    using nodetype_t = uint16_t;

    /* The NodeType enum, defining all types. */
    enum class NodeType {
        root = 0x1,
        file = 0x2,

        type_def = 0x4,
        positional = 0x8,
        option = 0x10,

        configs = 0x20,
        config = 0x40,

        values = 0x80,
        string = 0x100,
        regex = 0x200,
        number = 0x400,
        decimal = 0x800,
        reference = 0x1000
    };



    /* Map from each NodeType to a capitalized name. */
    const static std::unordered_map<NodeType, std::string> nodetype_name({
        { NodeType::root, "Root" },
        { NodeType::file, "File" },

        { NodeType::type_def, "TypeDef" },
        { NodeType::positional, "Positional" },
        { NodeType::option, "Option" },

        { NodeType::configs, "Configs" },
        { NodeType::config, "Config" },

        { NodeType::values, "Values" },
        { NodeType::string, "String" },
        { NodeType::regex, "Regex" },
        { NodeType::number, "Number" },
        { NodeType::decimal, "Decimal" },
        { NodeType::reference, "Reference" }
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
    static constexpr NodeType nt_values = NodeType::string | NodeType::regex | NodeType::number | NodeType::decimal | NodeType::reference;
    /* Constant type denoting all toplevel types. */
    static constexpr NodeType nt_toplevel = NodeType::type_def | NodeType::positional | NodeType::option;
    /* Constant type denoting all types. */
    static constexpr NodeType nt_all = (NodeType) ~((nodetype_t) 0);

}

#endif
