/* ADLVALUE.hpp
 *   by Lut99
 *
 * Created:
 *   24/11/2020, 22:59:43
 * Last edited:
 *   24/11/2020, 23:00:42
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the baseclass ADLValue, which is used to derive all
 *   values types from (strings, regex-expressions, numbers and decimals).
**/

#ifndef ADL_VALUE_HPP
#define ADL_VALUE_HPP

#include "ADLNode.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLValue class is the baseclass for all value types in the tree. */
    class ADLValue : public ADLNode {
        
    };
}

#endif
