/* ADLTREE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:36:22
 * Last edited:
 *   11/02/2021, 17:59:44
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
        
        /* Lets the Tree-node have a go at trying to store the given value/node for the given property key. Returns 'true' if it was successfull, or 'false' if no such property is relevant for this class. */
        virtual bool set_property(const std::string& key, const std::string& value);

        /* Prints all nodes in this file, reflecting the AST structure. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLTree to be copied polymorphically. */
        virtual ADLTree* copy() const;

    };
    
}

#endif
