/* ADLTREE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:36:22
 * Last edited:
 *   05/01/2021, 13:39:19
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Class rooting the entire Abstract Syntax Tree (AST) of the ADL.
**/

#ifndef ADL_TREE_HPP
#define ADL_TREE_HPP

#include <string>
#include <vector>

#include "ADLDefinition.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* Class for the topmost node in the ADL AST. */
    class ADLTree : public ADLDefinition {
    public:
        /* Constructor for the ADLTree class, which takes the filename of the root file and optionally takes a toplevel node. */
        ADLTree(const std::string& root_file, ADLNode* toplevel = nullptr);

        /* Prints all nodes in this file, reflecting the AST structure. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLTree to be copied polymorphically. */
        virtual ADLTree* copy() const;

    };
    
}

#endif
