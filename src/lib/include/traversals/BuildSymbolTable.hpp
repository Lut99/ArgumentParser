/* BUILD SYMBOL TABLE.hpp
 *   by Lut99
 *
 * Created:
 *   01/01/2021, 16:05:02
 * Last edited:
 *   01/01/2021, 16:24:12
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The first traversal of the compiler, which will use the parsed AST to
 *   construct a symbol table. At this stage, the compiler checks for
 *   duplicate identifiers in the correct namespaces.
**/

#ifndef BUILD_SYMBOL_TABLE_HPP
#define BUILD_SYMBOL_TABLE_HPP

#include "ADLTree.hpp"
#include "ADLSymbolTable.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The identifier for this traversal. */
    static constexpr const char* build_symbol_table_id = "build_symbol_table";
    /* The node types to which this traversal applies. */
    static constexpr NodeType build_symbol_table_types = NodeType::meta | NodeType::positional | NodeType::option | NodeType::type_def | NodeType::config;

    /* The traversal function for the first traversal, that builds a main symbol table (and those in definition nodes) from the parsed AST. */
    SymbolTable traversal_build_symbol_table(ADLTree* tree);
}

#endif
