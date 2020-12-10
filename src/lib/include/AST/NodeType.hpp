/* NODE TYPE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:33:25
 * Last edited:
 *   10/12/2020, 16:43:22
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
    using nodetype_t = uint32_t;

    /* The NodeType enum, defining all types. */
    enum class NodeType {
        root = 0x1,
        file = 0x2,

        meta = 0x4,
        type_def = 0x8,
        positional = 0x10,
        option = 0x20,

        identifier = 0x40,
        types = 0x80,

        configs = 0x100,
        config = 0x200,

        values = 0x400,
        string = 0x800,
        regex = 0x1000,
        number = 0x2000,
        decimal = 0x4000,
        reference = 0x8000,
        snippet = 0x10000
    };



    /* Map from each NodeType to a capitalized name. */
    const static std::unordered_map<NodeType, std::string> nodetype_name({
        { NodeType::root, "Root" },
        { NodeType::file, "File" },

        { NodeType::meta, "Meta" },
        { NodeType::type_def, "TypeDef" },
        { NodeType::positional, "Positional" },
        { NodeType::option, "Option" },

        { NodeType::identifier, "Identifier" },
        { NodeType::types, "Types" },

        { NodeType::configs, "Configs" },
        { NodeType::config, "Config" },

        { NodeType::values, "Values" },
        { NodeType::string, "String" },
        { NodeType::regex, "Regex" },
        { NodeType::number, "Number" },
        { NodeType::decimal, "Decimal" },
        { NodeType::reference, "Reference" },
        { NodeType::snippet, "Snippet" }
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
    static constexpr NodeType nt_values = NodeType::string | NodeType::regex | NodeType::number | NodeType::decimal | NodeType::reference | NodeType::snippet;
    /* Constant type denoting all toplevel types. */
    static constexpr NodeType nt_toplevel = NodeType::meta | NodeType::type_def | NodeType::positional | NodeType::option;
    /* Constant type denoting all types. */
    static constexpr NodeType nt_all = (NodeType) ~((nodetype_t) 0);

}

#endif
