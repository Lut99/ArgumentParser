/* ADLEXCEPTIONS.cpp
 *   by Lut99
 *
 * Created:
 *   14/11/2020, 18:05:10
 * Last edited:
 *   06/12/2020, 14:02:48
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
using namespace ArgumentParser::Exceptions;


/***** ADLNOTE CLASS *****/

/* Copies the ADLNote polymorphically. */
ADLNote* ADLNote::copy() const { return new ADLNote(*this); }





/***** EXCEPTIONHANDLER CLASS *****/

/* Default constructor for the ExceptionHandler class, which optionally takes the initial size of the internal array. */
ExceptionHandler::ExceptionHandler(size_t initial_capacity) :
    length(0),
    max_length(initial_capacity)
{
    // Reserve space for the initial capacity
    this->exceptions = new ADLException*[this->max_length];
}

/* Copy constructor for the ExceptionHandler class. */
ExceptionHandler::ExceptionHandler(const ExceptionHandler& other) :
    length(other.length),
    max_length(other.max_length)
{
    // Allocate a new exceptions list
    this->exceptions = new ADLException*[this->max_length];
    // Iterate through all the existing exceptions to copy them
    for (size_t i = 0; i < this->length; i++) {
        this->exceptions[i] = other.exceptions[i]->copy();
    }
}

/* Move constructor for the ExceptionHandler class. */
ExceptionHandler::ExceptionHandler(ExceptionHandler&& other) :
    exceptions(other.exceptions),
    length(other.length),
    max_length(other.max_length)
{
    // Set the other's value to nullptr to prevent it from deallocating the exceptions
    other.exceptions = nullptr;
}

/* Destructor for the ExceptionHandler class. */
ExceptionHandler::~ExceptionHandler() {
    // If our list isn't a nullptr, deallocate it and all exceptions inside it
    if (this->exceptions != nullptr) {
        for (size_t i = 0; i < this->length; i++) {
            delete this->exceptions[i];
        }
        delete this->exceptions;
    }
}



/* Resizes the internal array, by doubling its size. */
void ExceptionHandler::resize() {
    // First, allocate space for the new array
    size_t new_max_length = this->max_length * 2;
    ADLException** new_exceptions = new ADLException*[new_max_length];

    /* TBD */
}



/* Creates a pretty string for a derived ADLError class. */
std::ostream& ExceptionHandler::print_error(std::ostream& os, const Exceptions::ADLError& e) {
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
        os << ce.debug.line1 << ":" << ce.debug.col1  << ":";
    }
    // Continue with the error & the message
    os << " \033[31merror: \033[0m" << e.message << endl;

    // Next, if we're derived, append the raw line and the indicator where it went wrong
    if (is_derived) {
        // Cast to the derived class
        const ADLCompileError& ce = (const ADLCompileError&) e;
        const DebugInfo& debug = ce.debug;

        // Print the line number + spacing
        std::string strline = std::to_string(debug.line1);
        for (size_t i = strline.size(); i < 5; i++) { os << ' '; }
        os << strline;
        os << " | ";

        // Print the raw_line, with the correct characters in red
        const std::string& sraw = debug.raw_line.snippet;
        size_t line_i = debug.line1;
        bool red_mode = false;
        for (size_t i = 0; i < sraw.size(); i++) {
            // If we're at the correct pos in the string, write the red-marker
            if (line_i == debug.line1 && i + 1 == debug.col1) {
                os << "\033[31;1m";
                red_mode = true;
            }

            // Write the character, possibly doing neat newline padding
            os << sraw[i];
            if (i < sraw.size() - 1 && sraw[i] == '\n') {
                if (red_mode) { os << "\033[0m"; }
                os << "      | ";
                if (red_mode) { os << "\033[31;1m"; }
            }

            // If we're at the correct pos in the string, go back to white text
            if (line_i == debug.line2 && i + 1 == debug.col2) {
                os << "\033[0m";
                red_mode = false;
            }
        }
        // Add a newline if the last character wasn't
        if (sraw[sraw.size() - 1] != '\n') { os << '\n'; }

        // Next, write the same but with spaces and wiggly bits
        os << "      | ";
        for (size_t i = 1; i < debug.col1; i++) { os << ' '; }
        // Write enough '^'s
        os << "\033[31;1m";
        for (size_t i = 0; i < debug.col2 - debug.col1 + 1; i++) { os << '^'; }
        os << "\033[0m" << endl;
    }

    // Done
    return os;
}

/* Creates a pretty string for a derived ADLWarning class. */
std::ostream& ExceptionHandler::print_warning(std::ostream& os, const Exceptions::ADLWarning& e) {
    bool is_derived = dynamic_cast<ADLCompileWarning const*>(&e);

    // Always print the possible list of breadcrumbs first
    for (size_t i = 0; i < e.filenames.size() - 1; i++) {
        os << "\033[1m" << e.filenames[i] << ":\033[0m" << endl << "--> ";
    }

    // Then, we can also always print the filename itself
    os << "\033[1m" << e.filenames[e.filenames.size() - 1] << ":";
    // If we are actually a Compile warning, append the line & column number
    if (is_derived) {
        const ADLCompileWarning& ce = (const ADLCompileWarning&) e;
        os << ce.debug.line1 << ":" << ce.debug.col1  << ":";
    }
    // Continue with the warning & the message
    os << " \033[35mwarning: \033[0m" << e.message << " [\033[35;1m-W" << e.type << "\033[0m]" << endl;

    // Next, if we're derived, append the raw line and the indicator where it went wrong
    if (is_derived) {
        // Cast to the derived class
        const ADLCompileWarning& ce = (const ADLCompileWarning&) e;
        const DebugInfo& debug = ce.debug;

        // Print the line number + spacing
        std::string strline = std::to_string(debug.line1);
        for (size_t i = strline.size(); i < 5; i++) { os << ' '; }
        os << strline;
        os << " | ";

        // Print the raw_line, with the correct characters in red
        const std::string& sraw = debug.raw_line.snippet;
        size_t line_i = debug.line1;
        bool red_mode = false;
        for (size_t i = 0; i < sraw.size(); i++) {
            // If we're at the correct pos in the string, write the red-marker
            if (line_i == debug.line1 && i + 1 == debug.col1) {
                os << "\033[35;1m";
                red_mode = true;
            }

            // Write the character, possibly doing neat newline padding
            os << sraw[i];
            if (i < sraw.size() - 1 && sraw[i] == '\n') {
                if (red_mode) { os << "\033[0m"; }
                os << "      | ";
                if (red_mode) { os << "\033[35;1m"; }
            }

            // If we're at the correct pos in the string, go back to white text
            if (line_i == debug.line2 && i + 1 == debug.col2) {
                os << "\033[0m";
                red_mode = false;
            }
        }
        // Add a newline if the last character wasn't
        if (sraw[sraw.size() - 1] != '\n') { os << '\n'; }

        // Next, write the same but with spaces and wiggly bits
        os << "      | ";
        for (size_t i = 1; i < debug.col1; i++) { os << ' '; }
        // Write enough '^'s
        os << "\033[35;1m";
        for (size_t i = 0; i < debug.col2 - debug.col1 + 1; i++) { os << '^'; }
        os << "\033[0m" << endl;
    }

    // Done
    return os;
}

/* Creates a pretty string for a derived ADLNote class. */
std::ostream& ExceptionHandler::print_note(std::ostream& os, const ADLNote& e) {
    const DebugInfo& debug = e.debug;

    // Always print the possible list of breadcrumbs first
    for (size_t i = 0; i < e.filenames.size() - 1; i++) {
        os << "\033[1m" << e.filenames[i] << ":\033[0m" << endl << "--> ";
    }

    // Then, we can also always print the filename itself
    os << "\033[1m" << e.filenames[e.filenames.size() - 1] << ":";
    // If we are actually a Compile warning, append the line & column number
    os << debug.line1 << ":" << debug.col1  << ":";
    // Continue with the note & the message
    os << " \033[36mnote: \033[0m" << e.message << endl;

    // Print the line number + spacing
    std::string strline = std::to_string(debug.line1);
    for (size_t i = strline.size(); i < 5; i++) { os << ' '; }
    os << strline;
    os << " | ";

    // Print the raw_line, with the correct characters in cyan
    const std::string& sraw = debug.raw_line.snippet;
    size_t line_i = debug.line1;
    bool red_mode = false;
    for (size_t i = 0; i < sraw.size(); i++) {
        // If we're at the correct pos in the string, write the red-marker
        if (line_i == debug.line1 && i + 1 == debug.col1) {
            os << "\033[36;1m";
            red_mode = true;
        }

        // Write the character, possibly doing neat newline padding
        os << sraw[i];
        if (i < sraw.size() - 1 && sraw[i] == '\n') {
            if (red_mode) { os << "\033[0m"; }
            os << "      | ";
            if (red_mode) { os << "\033[36;1m"; }
        }

        // If we're at the correct pos in the string, go back to white text
        if (line_i == debug.line2 && i + 1 == debug.col2) {
            os << "\033[0m";
            red_mode = false;
        }
    }
    // Add a newline if the last character wasn't
    if (sraw[sraw.size() - 1] != '\n') { os << '\n'; }

    // Next, write the same but with spaces and wiggly bits
    os << "      | ";
    for (size_t i = 1; i < debug.col1; i++) { os << ' '; }
    // Write enough '^'s
    os << "\033[36;1m";
    for (size_t i = 0; i < debug.col2 - debug.col1 + 1; i++) { os << '^'; }
    os << "\033[0m" << endl;

    // We're done!
    return os;
}
