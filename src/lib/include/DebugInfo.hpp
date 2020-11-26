/* DEBUG INFO.hpp
 *   by Lut99
 *
 * Created:
 *   26/11/2020, 14:38:54
 * Last edited:
 *   26/11/2020, 16:09:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the DebugInfo struct, which is used to keep track
 *   of a node or token's origin in the source file we are compiling. In
 *   particular, it keeps track of the specific line- and column numbers
 *   and carries the (at most) 50 characters of line where the error
 *   occurred.
**/

#ifndef DEBUG_INFO_HPP
#define DEBUG_INFO_HPP

#include <cstdlib>
#include <string>

namespace ArgumentParser {
    /* The LineSnippet struct is used to carry a piece of a line, noted which line and from where to where. */
    struct LineSnippet {
        /* The column number of the first character in the snippet. */
        size_t col1;
        /* The column number of the last character in the snippet. */
        size_t col2;
        /* The line snippet itself. */
        std::string snippet;

        /* Constructor for the LineSnippet which takes the column numbers and the string itself. */
        LineSnippet(size_t col1, size_t col2, const std::string& snippet) :
            col1(col1), col2(col2), snippet(snippet)
        {}
        /* Constructor for the LineSnippet which initializes it with a raw string. */
        LineSnippet(const std::string& s) : col1(0), col2(s.size() - 1), snippet(s) {}

        /* Concatenates a string to this line snippet. */
        inline std::string operator+(const std::string& s) { return this->snippet + s; }
        /* Concatenates this line snippet to a string. */
        friend inline std::string operator+(const std::string& s, const LineSnippet& ls) { return s + ls.snippet; }
    };

    /* The DebugInfo struct keeps track of where a node or token came from. */
    struct DebugInfo {
        /* The line number where this node or token started. */
        size_t line1;
        /* The column number where this node or token started. */
        size_t col1;
        /* The line number where this node or token ended. */
        size_t line2;
        /* The column number where this node or token ended. */
        size_t col2;

        /* The raw string including and around the token. */
        LineSnippet raw_line;

        /* Constructor for the DebugInfo struct, which takes the line information and a raw line snippet. */
        DebugInfo(size_t line1, size_t col1, size_t line2, size_t col2, const LineSnippet& raw_line) :
            line1(line1), col1(col1), line2(line2), col2(col2), raw_line(raw_line)
        {}
        /* Constructor for the DebugInfo struct, which takes only the starting line information and a raw line snippet. */
        DebugInfo(size_t line, size_t col, const LineSnippet& raw_line) :
            line1(line), col1(col), line2(line), col2(col), raw_line(raw_line)
        {}
    };

    /* A static debuginfo for when a node does not have a direct origin. */
    const static DebugInfo di_empty(0, 0, 0, 0, std::string(""));

}

#endif
