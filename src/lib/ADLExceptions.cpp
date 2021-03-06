/* ADLEXCEPTIONS.cpp
 *   by Lut99
 *
 * Created:
 *   14/11/2020, 18:05:10
 * Last edited:
 *   05/01/2021, 14:23:36
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


/***** ADLERROR CLASS *****/

/* Function that prints the error to the given output stream. */
std::ostream& ADLError::print(std::ostream& os) const {
    // Always print the possible list of breadcrumbs first
    for (size_t i = 0; i < this->filenames.size() - 1; i++) {
        os << "\033[1m" << this->filenames[i] << ":\033[0m" << endl << "--> ";
    }

    // Then, we can also always print the filename itself
    os << "\033[1m" << this->filenames[this->filenames.size() - 1] << ":";
    
    // Continue with the error & the message
    os << " \033[31merror: \033[0m" << this->message << endl;

    // Done
    return os;
}





/***** ADLCOMPILEERROR CLASS *****/

/* Function that prints the compilation error to the given output stream. */
std::ostream& ADLCompileError::print(std::ostream& os) const {
    const DebugInfo& debug = this->debug;

    // Always print the possible list of breadcrumbs first
    for (size_t i = 0; i < this->filenames.size() - 1; i++) {
        os << "\033[1m" << this->filenames[i] << ":\033[0m" << endl << "--> ";
    }

    // Then, we can also always print the filename itself
    os << "\033[1m" << this->filenames[this->filenames.size() - 1] << ":" << debug.line1 << ":" << debug.col1  << ":" << " \033[31merror: \033[0m" << this->message << endl;

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

    // Done
    return os;
}





/***** ADLWARNING CLASS *****/

/* Function that prints the warning to the given output stream. */
std::ostream& ADLWarning::print(std::ostream& os) const {
    // Always print the possible list of breadcrumbs first
    for (size_t i = 0; i < this->filenames.size() - 1; i++) {
        os << "\033[1m" << this->filenames[i] << ":\033[0m" << endl << "--> ";
    }

    // Then, we can also always print the filename itself
    os << "\033[1m" << this->filenames[this->filenames.size() - 1] << ":" << " \033[35mwarning: \033[0m" << this->message << " [\033[35;1m" << warningtype_names.at(this->type) << "\033[0m]" << endl;

    // Done
    return os;
}





/***** ADLCOMPILEWARNING CLASS *****/

/* Function that prints the compilation warning to the given output stream. */
std::ostream& ADLCompileWarning::print(std::ostream& os) const {
    const DebugInfo& debug = this->debug;

    // Always print the possible list of breadcrumbs first
    for (size_t i = 0; i < this->filenames.size() - 1; i++) {
        os << "\033[1m" << this->filenames[i] << ":\033[0m" << endl << "--> ";
    }

    // Then, we can also always print the filename itself
    os << "\033[1m" << this->filenames[this->filenames.size() - 1] << ":" << debug.line1 << ":" << debug.col1  << ":" << " \033[35mwarning: \033[0m" << this->message << " [\033[35;1m" << warningtype_names.at(this->type) << "\033[0m]" << endl;

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

    // Done
    return os;
}





/***** ADLNOTE CLASS *****/

/* Copies the ADLNote polymorphically. */
ADLNote* ADLNote::copy() const { return new ADLNote(*this); }

/* Function that prints the note to the given output stream. */
std::ostream& ADLNote::print(std::ostream& os) const {
    const DebugInfo& debug = this->debug;

    // Always print the possible list of breadcrumbs first
    for (size_t i = 0; i < this->filenames.size() - 1; i++) {
        os << "\033[1m" << this->filenames[i] << ":\033[0m" << endl << "--> ";
    }

    // Then, we can also always print the filename itself
    os << "\033[1m" << this->filenames[this->filenames.size() - 1] << ":" << debug.line1 << ":" << debug.col1  << ":" << " \033[36mnote: \033[0m" << this->message << endl;

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





/***** EXCEPTIONHANDLER CLASS *****/

/* Default constructor for the ExceptionHandler class, which optionally takes whether or not exceptions should be printed immediately and the initial size of the internal array. */
ExceptionHandler::ExceptionHandler(bool print_on_add, size_t initial_capacity) :
    length(0),
    max_length(initial_capacity),
    print_on_add(print_on_add),
    toplevel_suppressed((WarningType) 0),
    config_suppressed((WarningType) 0)
{
    // Reserve space for the initial capacity
    this->exceptions = new ADLException*[this->max_length];
}

/* Copy constructor for the ExceptionHandler class. */
ExceptionHandler::ExceptionHandler(const ExceptionHandler& other) :
    length(other.length),
    max_length(other.max_length),
    print_on_add(other.print_on_add),
    toplevel_suppressed(other.toplevel_suppressed),
    config_suppressed(other.config_suppressed)
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
    max_length(other.max_length),
    print_on_add(other.print_on_add),
    toplevel_suppressed(other.toplevel_suppressed),
    config_suppressed(other.config_suppressed)
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
        delete[] this->exceptions;
    }
}



/* Resizes the internal array, by doubling its size. */
void ExceptionHandler::resize() {
    // First, allocate space for the new array
    size_t new_max_length = this->max_length * 2;
    ADLException** new_exceptions = new ADLException*[new_max_length];

    // Move all the elements over
    for (size_t i = 0; i < this->length; i++) {
        new_exceptions[i] = this->exceptions[i];
    }

    // Replace the existing lists
    delete this->exceptions;
    this->max_length = new_max_length;
    this->exceptions = new_exceptions;
}



/* Private function that adds a single note to the parser. */
void ExceptionHandler::add_note(const ADLNote& note) {
    // Note the note in our internal list, resizing as needed
    if (this->length >= this->max_length) { this->resize(); }
    this->exceptions[this->length++] = note.copy();
    
    // Don't forget to print it
    if (this->print_on_add) { note.print(cerr); }
}



/* Adds a new exception to the handler. */
ExceptionHandler& ExceptionHandler::log(const ADLException& except) {
    // If the exception is actually a warning, possibly ignore it
    if (dynamic_cast<const ADLWarning*>(&except)) {
        const ADLWarning& warning = (const ADLWarning&) except;
        if ((this->toplevel_suppressed | this->config_suppressed) & warning.type) {
            // It's one of the suppressed types; just return, without doing anything
            #ifdef DEBUG
            cout << "[   Exception   ] Suppressed warning of type '" << warningtype_names.at(warning.type) << "'" << endl;
            #endif
            return *this;
        }
    }

    // Log the exception in our internal list
    if (this->length >= this->max_length) { this->resize(); }
    this->exceptions[this->length++] = except.copy();
    if (dynamic_cast<const ADLError*>(&except)) { ++this->n_errors; }

    // Don't forget to print it
    if (this->print_on_add) { except.print(cerr); }

    // When done, return ourselves so we may potentially be thrown
    return *this;
}



/* Neatly prints all exceptions in this ExceptionHandler. */
std::ostream& Exceptions::operator<<(std::ostream& os, const ExceptionHandler& handler) {
    // Simply print each of the exceptions
    for (size_t i = 0; i < handler.length; i++) {
        handler.exceptions[i]->print(os);
    }

    // When all is done, return the stream
    return os;
}



/* Move assignment operator for the ExceptionHandler class. */
ExceptionHandler& ExceptionHandler::operator=(ExceptionHandler&& other) {
    if (this != &other) { swap(*this, other); }
    return *this;
}

/* Swap operator for the ExceptionHandler class. */
void Exceptions::swap(ExceptionHandler& eh1, ExceptionHandler& eh2) {
    using std::swap;

    swap(eh1.exceptions, eh2.exceptions);
    swap(eh1.length, eh2.length);
    swap(eh1.max_length, eh2.max_length);
    swap(eh1.print_on_add, eh2.print_on_add);
    swap(eh1.toplevel_suppressed, eh2.toplevel_suppressed);
    swap(eh1.config_suppressed, eh2.config_suppressed);
}
