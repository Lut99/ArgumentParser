/* ADLFILE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:36:22
 * Last edited:
 *   24/11/2020, 22:45:09
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Class describing a File node in the ADL Abstract Syntax Tree (AST).
 *   This node describes the top-level node of a single file, but can occur
 *   multiple times in a tree (on the same level) if other nodes are
 *   included.
**/

#ifndef ADL_FILE_HPP
#define ADL_FILE_HPP

#include <any>
#include <string>
#include <vector>

#include "ADLNode.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* Class for the second-to-topmost node in the ADL AST. In most cases, topmost node. */
    class ADLFile : public ADLNode {
    protected:
        /* Stores the top-level nodes in the file that this ADLFile represents. */
        std::vector<ADLNode*> toplevel;

        /* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*));
        /* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state);

    public:
        /* Constructor for the ADLFile class, which takes a trail of filenames where it originated, a column number where it originated, a matching column number and the parent of this node. */
        ADLFile(const std::vector<std::string>& filenames, size_t line, size_t col, ADLNode* parent);
        /* Copy constructor for the ADLFile class. */
        ADLFile(const ADLFile& other);
        /* Move constructor for the ADLFile class. */
        ADLFile(ADLFile&& other);
        /* Destructor for the ADLFile class. */
        virtual ~ADLFile();

        /* Adds a new node as direct child of this ADLFile. */
        inline void add_node(ADLNode* node) { this->toplevel.push_back(node); }
        /* Removes a given node as direct child of this ADLFile. */
        void remove_node(ADLNode* node);

        /* Allows access to one of the children in this node. */
        inline ADLNode* operator[](size_t index) const { return this->toplevel[index]; }
        /* Returns the number of top-level nodes in this file. */
        inline size_t size() const { return this->toplevel.size(); }

        /* Returns a iterator pointing to the start of the internal vector of top-level nodes, so that it may be iterator over. */
        inline std::vector<ADLNode*>::const_iterator begin() const { return this->toplevel.begin(); }
        /* Returns a iterator pointing to the end of the internal vector of top-level nodes, so that it may be iterator over. */
        inline std::vector<ADLNode*>::const_iterator end() const { return this->toplevel.end(); }
        /* Returns a reverse iterator pointing to the start of the internal vector of top-level nodes, so that it may be iterator over. */
        inline std::vector<ADLNode*>::const_reverse_iterator rbegin() const { return this->toplevel.rbegin(); }
        /* Returns a reverse iterator pointing to the end of the internal vector of top-level nodes, so that it may be iterator over. */
        inline std::vector<ADLNode*>::const_reverse_iterator rend() const { return this->toplevel.rend(); }

        /* Allows the ADLFile to be copied polymorphically. */
        virtual ADLFile* copy() const;

    };
    
}

#endif
