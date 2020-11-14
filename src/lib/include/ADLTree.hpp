/* ADLTREE.hpp
 *   by Anonymous
 *
 * Created:
 *   11/12/2020, 5:42:12 PM
 * Last edited:
 *   14/11/2020, 14:35:13
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file defines the tree used to represent a parsed ADL file.
**/

#ifndef ADL_TREE_HPP
#define ADL_TREE_HPP

namespace ArgumentParser{
    namespace Exceptions {

    }

    /* Lists all node types. */
    enum class NodeType {

    };

    /* The root node of any ADLTree, which even spans multiple files. */
    class ADLTree {

    };
    /* Baseclass for all non-root nodes for the tree. */
    class ADLNode {
    public:
        /* Type of this node. */
        const NodeType type;

    };
    
    
    
}

#endif
