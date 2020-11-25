/* ADLFILE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:36:22
 * Last edited:
 *   25/11/2020, 20:35:51
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
        /* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*));
        /* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state);

    public:
        /* Stores the top-level nodes in the file that this ADLFile represents. */
        std::vector<ADLNode*> toplevel;

        
        /* Constructor for the ADLFile class, which only takes a trail of filenames. */
        ADLFile(const std::vector<std::string>& filenames);
        /* Copy constructor for the ADLFile class. */
        ADLFile(const ADLFile& other);
        /* Move constructor for the ADLFile class. */
        ADLFile(ADLFile&& other);
        /* Destructor for the ADLFile class. */
        virtual ~ADLFile();

        /* Dynamically adds another nested node to the list of toplevel-nodes. */
        void add_node(ADLNode* node);

        /* Allows the ADLFile to be copied polymorphically. */
        virtual ADLFile* copy() const;

    };
    
}

#endif
