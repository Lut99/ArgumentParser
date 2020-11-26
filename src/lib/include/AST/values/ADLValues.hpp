/* ADLVALUES.hpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 18:11:33
 * Last edited:
 *   26/11/2020, 12:27:48
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains a class that is used to group multiple, consecutive
 *   ADLValue symbols into one list of them.
**/

#ifndef ADL_VALUES_HPP
#define ADL_VALUES_HPP

#include <string>
#include <vector>

#include "ADLBranch.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLValues class is used to group ADLValue symbols. */
    class ADLValues: public ADLBranch {
    public:
        /* Constructor for the ADLValues class, which takes a trail of breadcrumbs for files we're parsing, the line number where the symbol originated and a matching column number. */
        ADLValues(const std::vector<std::string>& filenames, size_t line, size_t col);

        /* Allows the ADLValues to be copied polymorphically. */
        virtual ADLValues* copy() const;

    };
};

#endif
