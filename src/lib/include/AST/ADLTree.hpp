/* ADLTREE.hpp
 *   by Anonymous
 *
 * Created:
 *   11/12/2020, 5:42:12 PM
 * Last edited:
 *   26/11/2020, 14:11:47
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

#include "ADLBranch.hpp"
#include "ADLFile.hpp"

namespace ArgumentParser{
    /* The root node of any ADLTree, which even spans multiple files. */
    class ADLTree : public ADLBranch {
    public:
        /* Constructor for the ADLTree, which takes an optional file. */
        ADLTree(ADLNode* file = nullptr);

        /* Merge this ADLTree with another one. */
        ADLTree& operator+=(const ADLTree& other);

        /* Allows the ADLTree to be copied polymorphically. */
        virtual ADLTree* copy() const;

    };
    
}

#endif
