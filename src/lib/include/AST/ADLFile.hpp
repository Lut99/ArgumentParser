/* ADLFILE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:36:22
 * Last edited:
 *   18/11/2020, 20:49:23
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

#include "ADLNode.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* Class for the second-to-topmost node in the ADL AST. In most cases, topmost node. */
    class ADLFile : public ADLNode {
    public:

        /* Allows the ADLFile to be copied polymorphically. */
        virtual ADLFile* copy() const;

    };
    
}

#endif
