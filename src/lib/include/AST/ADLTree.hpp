/* ADLTREE.hpp
 *   by Anonymous
 *
 * Created:
 *   11/12/2020, 5:42:12 PM
 * Last edited:
 *   25/11/2020, 20:41:12
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file defines the tree used to represent a parsed ADL file.
**/

#ifndef ADL_TREE_HPP
#define ADL_TREE_HPP

#include <cstdlib>
#include <vector>
#include <any>

#include "ADLNode.hpp"
#include "ADLFile.hpp"

namespace ArgumentParser{
    /* The root node of any ADLTree, which even spans multiple files. */
    class ADLTree : public ADLNode {
    private:
        /* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*));
        /* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state);

    public:
        /* The files in this tree. */
        std::vector<ADLFile*> files;

        /* Constructor for the ADLTree, which doesn't take anything! */
        ADLTree();
        /* Copy constructor for the ADLTree. */
        ADLTree(const ADLTree& other);
        /* Move constructor for the ADLTree. */
        ADLTree(ADLTree&& other);
        /* Destructor for the ADLTree. */
        ~ADLTree();

        /* Dynamically adds another file to the list of files. */
        void add_file(ADLFile* file);

        /* Merge this ADLTree with another one. */
        ADLTree& operator+=(const ADLTree& other);

        /* Allows the ADLTree to be copied polymorphically. */
        virtual ADLTree* copy() const;

    };
    
}

#endif
