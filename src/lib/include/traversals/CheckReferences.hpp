/* CHECK REFERENCES.hpp
 *   by Lut99
 *
 * Created:
 *   11/02/2021, 15:57:33
 * Last edited:
 *   11/02/2021, 17:35:10
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File that implements the traversal that links (and thus verifies)
 *   references made throughout an ADL file.
**/

#ifndef CHECK_REFERENCES_HPP
#define CHECK_REFERENCES_HPP

#include "ADLTree.hpp"
#include "ADLSymbolTable.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The identifier for this traversal. */
    static constexpr const char* check_references_id = "check_references";
    /* The node types to which this traversal applies. */
    static constexpr NodeType check_references_types = NodeType::types | NodeType::reference;

    /* The traversal function for the first traversal, that builds a main symbol table (and those in definition nodes) from the parsed AST. */
    void traversal_check_references(ADLTree* tree);
}

#endif
