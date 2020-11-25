/* ADLEXCEPTIONS.cpp
 *   by Lut99
 *
 * Created:
 *   14/11/2020, 18:05:10
 * Last edited:
 *   25/11/2020, 16:02:20
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the root of the ArgumentParser exception system,
 *   and the functionality to print each exception in a very pretty,
 *   GCC-like style.
**/

#include "ADLExceptions.hpp"

using namespace std;
using namespace ArgumentParser;


/* Creates a pretty string for a derived ADLError class. */
std::ostream& Exceptions::print_error(std::ostream& os, const Exceptions::ADLError& e) {
    bool is_derived = dynamic_cast<ADLCompileError const*>(&e);

    // Always print the possible list of breadcrumbs first
    for (size_t i = 0; i < e.filenames.size() - 1; i++) {
        os << "\033[1m" << e.filenames[i] << ":\033[0m" << endl << "--> ";
    }

    // Then, we can also always print the filename itself
    os << "\033[1m" << e.filenames[e.filenames.size() - 1] << ":";
    // If we are actually a Compile error, append the line & column number
    if (is_derived) {
        const ADLCompileError& ce = (const ADLCompileError&) e;
        os << ce.line << ":" << ce.col  << ":";
    }
    // Continue with the error & the message
    os << " \033[31merror: \033[0m" << e.message << endl;

    // Next, if we're derived, append the raw line and the indicator where it went wrong
    if (is_derived) {
        // Cast to the derived class
        const ADLCompileError& ce = (const ADLCompileError&) e;

        // Print the line number + spacing
        std::string strline = std::to_string(ce.line);
        for (size_t i = strline.size(); i < 5; i++) { os << ' '; }
        os << strline;
        os << " | ";

        // Print the raw_line, with the correct char in red
        for (size_t i = 0; i < ce.raw_line.size(); i++) {
            if (i + 1 == ce.col) { os << "\033[31;1m" << ce.raw_line[i] << "\033[0m"; }
            else { os << ce.raw_line[i]; }
        }

        // Next, write the same but with spaces and wiggly bits
        os << "      | ";
        for (size_t i = 1; i < ce.col; i++) { os << ' '; }
        os << "\033[31;1m" << '^' << "\033[0m" << endl;
    }

    // Done
    return os;
}

/* Creates a pretty string for a derived ADLWarning class. */
std::ostream& Exceptions::print_warning(std::ostream& os, const Exceptions::ADLWarning& e) {
    bool is_derived = dynamic_cast<ADLCompileWarning const*>(&e);

    // Always print the possible list of breadcrumbs first
    for (size_t i = 0; i < e.filenames.size() - 1; i++) {
        os << "\033[1m" << e.filenames[i] << ":\033[0m" << endl << "--> ";
    }

    // Then, we can also always print the filename itself
    os << "\033[1m" << e.filenames[e.filenames.size() - 1] << ":";
    // If we are actually a Compile error, append the line & column number
    if (is_derived) {
        const ADLCompileWarning& ce = (const ADLCompileWarning&) e;
        os << ce.line << ":" << ce.col  << ":";
    }
    // Continue with the error & the message
    os << " \033[35mwarning: \033[0m" << e.message << " [\033[35;1m-W" << e.type << "\033[0m]" << endl;

    // Next, if we're derived, append the raw line and the indicator where it went wrong
    if (is_derived) {
        // Cast to the derived class
        const ADLCompileWarning& ce = (const ADLCompileWarning&) e;

        // Print the line number + spacing
        std::string strline = std::to_string(ce.line);
        for (size_t i = strline.size(); i < 5; i++) { os << ' '; }
        os << strline;
        os << " | ";

        // Print the raw_line, with the correct char in red
        for (size_t i = 0; i < ce.raw_line.size(); i++) {
            if (i + 1 == ce.col) { os << "\033[35;1m" << ce.raw_line[i] << "\033[0m"; }
            else { os << ce.raw_line[i]; }
        }

        // Next, write the same but with spaces and wiggly bits
        os << "      | ";
        for (size_t i = 1; i < ce.col; i++) { os << ' '; }
        os << "\033[35;1m" << '^' << "\033[0m" << endl;
    }

    // Done
    return os;
}
