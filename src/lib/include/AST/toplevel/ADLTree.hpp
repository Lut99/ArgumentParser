/* ADLTREE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:36:22
 * Last edited:
 *   03/12/2020, 23:18:41
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

#include "ADLBranch.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* Class for the topmost node in the ADL AST. */
    class ADLTree : public ADLBranch {
    public:
        /* Constructor for the ADLTree class, which only optionally takes a toplevel node. */
        ADLTree(ADLNode* toplevel = nullptr);

        /* Prints all nodes in this file, reflecting the AST structure. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLTree to be copied polymorphically. */
        virtual ADLTree* copy() const;

    };
    
}

#endif
