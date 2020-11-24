/* ADLTREE.hpp
 *   by Anonymous
 *
 * Created:
 *   11/12/2020, 5:42:12 PM
 * Last edited:
 *   24/11/2020, 22:45:18
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
        /* The files in this tree. */
        std::vector<ADLFile*> files;

        /* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*));
        /* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state);

    public:
        /* Constructor for the ADLTree, which doesn't take anything! */
        ADLTree();
        /* Copy constructor for the ADLTree. */
        ADLTree(const ADLTree& other);
        /* Move constructor for the ADLTree. */
        ADLTree(ADLTree&& other);
        /* Destructor for the ADLTree. */
        ~ADLTree();

        /* Add a new file child to the tree. */
        inline void add_file(ADLFile* file) { this->files.push_back(file); }
        /* Removes a given node as direct child of this ADLTree. */
        void remove_node(ADLFile* node);

        /* Allows access to the i'th file stored in this tree. */
        inline ADLFile* operator[](size_t i) const { return this->files[i]; }
        /* Returns the number of files currently stored in the ADLTree. */
        inline size_t size() const { return this->files.size(); }
        
        /* Merge this ADLTree with another one. */
        ADLTree& operator+=(const ADLTree& other);

        /* Returns a iterator pointing to the start of the internal vector of top-level nodes, so that it may be iterator over. */
        inline std::vector<ADLFile*>::const_iterator begin() const { return this->files.begin(); }
        /* Returns a iterator pointing to the end of the internal vector of top-level nodes, so that it may be iterator over. */
        inline std::vector<ADLFile*>::const_iterator end() const { return this->files.end(); }
        /* Returns a reverse iterator pointing to the start of the internal vector of top-level nodes, so that it may be iterator over. */
        inline std::vector<ADLFile*>::const_reverse_iterator rbegin() const { return this->files.rbegin(); }
        /* Returns a reverse iterator pointing to the end of the internal vector of top-level nodes, so that it may be iterator over. */
        inline std::vector<ADLFile*>::const_reverse_iterator rend() const { return this->files.rend(); }

    };
    
}

#endif
