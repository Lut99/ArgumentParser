/* ADLBAKED.hpp
 *   by Lut99
 *
 * Created:
 *   12/5/2020, 1:04:17 PM
 * Last edited:
 *   12/5/2020, 4:23:58 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains a const static C++-map, which will contain the
 *   pre-baked ADL system files, ready to be included from a user file. For
 *   now, the files are included in plain tekst and parsed with the user
 *   file, but later we might want to pre-compile this and then include it
 *   if the ArgumentParser will support compiling individual files and then
 *   linking them together.
 * 
 *   The contents of this file will be inserted by a script, hence the weird
 *   comment at the place where one would expect dictionary values.
**/

#ifndef ADL_BAKED_HPP
#define ADL_BAKED_HPP

#include <cstddef>

namespace ArgumentParser {
    namespace System {
        static const size_t n_files = /*****COUNTPASTE*****/;

        static const char* names[] = {
            /*****NAMEPASTE*****/
        };
        static const char* files[] = {
            /*****FILEPASTE*****/
        };
    }
}

#endif
