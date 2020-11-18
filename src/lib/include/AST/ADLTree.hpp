/* ADLTREE.hpp
 *   by Anonymous
 *
 * Created:
 *   11/12/2020, 5:42:12 PM
 * Last edited:
 *   18/11/2020, 21:21:04
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
    class ADLTree {
    private:
        /* The files in this tree. */
        std::vector<ADLFile*> files;

    public:
        /* Constructor for the ADLTree, which doesn't take anything! */
        ADLTree() {}
        /* Copy constructor for the ADLTree. */
        ADLTree(const ADLTree& other);
        /* Move constructor for the ADLTree. */
        ADLTree(ADLTree&& other);
        /* Virtual destructor for the ADLTree. */
        virtual ~ADLTree();

        /* Add a new file child to the tree. */
        inline void add_file(const ADLFile& file) { this->files.push_back(file.copy()); }

        /* Allows access to the i'th file stored in this tree. */
        inline ADLFile* operator[](size_t i) const { return this->files[i]; }
        /* Returns the number of files currently stored in the ADLTree. */
        inline size_t size() const { return this->files.size(); }

        /* Traverses the tree, and calls the given function when any of the given types (OR'ed together) is encountered. The return pointer of your function will replace the current node pointer before traversing deeper.*/
        void traverse(NodeType nodes, ADLNode* (*trav_func)(ADLNode*));
        /* Traverses the tree, and calls the given function when any of the given types (OR'ed together) is encountered. The argument to the function is the current node and the argument that we given at the start and may be used to retain a state. The return pointer of your function will replace the current node pointer before traversing deeper. */
        void traverse(NodeType nodes, ADLNode* (*trav_func)(ADLNode*, std::any&), std::any& traverse_value);
        
        /* Merge this ADLTree with another one. */
        ADLTree& operator+=(const ADLTree& other);

        /* Copy assignment operator for the ADLTree class. */
        inline ADLTree& operator=(const ADLTree& other) { return *this = ADLTree(other); }
        /* Move assignment operator for the ADLTree class. */
        ADLTree& operator=(ADLTree&& other);
        /* Swap operator for the ADLTree class. */
        friend void swap(ADLTree& t1, ADLTree& t2);

    };
    /* Swap operator for the ADLTree class. */
    void swap(ADLTree& t1, ADLTree& t2);
    
}

#endif
