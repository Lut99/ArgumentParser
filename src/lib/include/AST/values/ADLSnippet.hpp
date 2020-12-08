/* ADLSNIPPET.hpp
 *   by Lut99
 *
 * Created:
 *   01/12/2020, 12:54:00
 * Last edited:
 *   08/12/2020, 20:51:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLSnippet class, which is used to represent a
 *   bit of in-file C++ code. The code itself isn't parsed, but instead
 *   kept until the C++-ArgumentParser code is generated. Then, it will
 *   simply be pasted at the correct location (after substitution of the
 *   ++<X>++ references).
**/

#ifndef ADL_SNIPPET_HPP
#define ADL_SNIPPET_HPP

#include <string>
#include <ostream>

#include "ADLLeaf.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLSnippet class is used to represent a bit of inline C++-code. */
    class ADLSnippet: public ADLLeaf {
    public:
        /* The raw C++-sourcecode that this node wraps. */
        std::string code;

        /* Constructor for the ADLSnippet class, which takes a DebugInfo struct linking the node back to the source file and the C++-code that this node wraps. */
        ADLSnippet(const DebugInfo& debug, const std::string& code);

        /* Prints the code snippet to the given output stream. For readability, the code itself isn't pasted (as that's less relevant for parsing anyway). */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLSnippet to be copied polymorphically. */
        virtual ADLSnippet* copy() const;

    };
}

#endif
