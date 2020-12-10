/* ADLEXCEPTIONS.hpp
 *   by Lut99
 *
 * Created:
 *   14/11/2020, 16:14:52
 * Last edited:
 *   10/12/2020, 17:29:55
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Easily includable file that defines the ArgumentParser's basic
 *   exceptions, including a way to pretty print them.
**/

#ifndef ADL_EXCEPTIONS_HPP
#define ADL_EXCEPTIONS_HPP

#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include <ostream>

#include "DebugInfo.hpp"

namespace ArgumentParser::Exceptions {
    /* Exception for when there aren't enough filenames. */
    class BreadcrumbException : public std::exception {
    public:
        /* Constructor for the BreadcrumbException, which takes nothing. */
        BreadcrumbException() {}

        /* Implementation for std::exception's what() function, so that it's compatible with C++'s uncaught-exception handling. */
        virtual const char* what() const noexcept { return "ADLException-derived exceptions need at least one filename."; }

    };



    /* Baseclass for all exceptions used throughout the parser. */
    class ADLException : public std::exception {
    public:
        /* The include path of the filenames we want to parse. */
        const std::vector<std::string> filenames;
        /* The actual message we got. */
        const std::string message;

        /* Constructor for the ADLException class, which takes the relevant include path of files and optionally a message. */
        ADLException(const std::vector<std::string>& filenames, const std::string& message = "") :
            filenames(filenames),
            message(message)
        {
            if (filenames.size() < 1) { throw BreadcrumbException(); }
        }

        /* Implementation for std::exception's what() function, so that it's compatible with C++'s uncaught-exception handling. */
        virtual const char* what() const noexcept { return this->message.c_str(); }

        /* Function that prints the error to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const = 0;

        /* Copies the exception polymorphically. */
        virtual ADLException* copy() const = 0;

    };



    /* Baseclass exception for all non-recoverable errors in the parser. */
    class ADLError : public ADLException {
    public:
        /* Constructor for the ADLError class, which takes the relevant include path of files and optionally a message. */
        ADLError(const std::vector<std::string>& filenames, const std::string& message = "") :
            ADLException(filenames, message)
        {}

        /* Function that prints the error to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

    };

    /* Baseclass exception for all non-recoverable errors that are specific to a certain position within a file. */
    class ADLCompileError: public ADLError {
    private:
        /* Declare the print function as friend. */
        friend std::ostream& print_error(std::ostream& os, const ADLError& e);

    public:
        /* The debug information of the token or node that caused this error. */
        const DebugInfo debug;

        /* Constructor for the ADLCompileError class, which takes:
         *   - a DebugInfo struct containing the location in the source file
         *   - [optional] a message
         */
        ADLCompileError(const DebugInfo& debug, const std::string& message = "") :
            ADLError(debug.filenames, message),
            debug(debug)
        {}

        /* Function that prints the compilation error to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

    };



    /* Baseclass exception for all warnings (i.e., extra information about bad practises). */
    class ADLWarning : public ADLException {
    public:
        /* The type of this warning. */
        const std::string type;

        /* Constructor for the ADLWarning class, which takes the type of warning, the relevant include path of files and optionally a message. */
        ADLWarning(const std::string& type, const std::vector<std::string>& filenames, const std::string& message = "") :
            ADLException(filenames, message),
            type(type)
        {}

        /* Function that prints the warning to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

    };

    /* Baseclass exception for all non-recoverable errors that are specific to a certain position within a file. */
    class ADLCompileWarning: public ADLWarning {
    private:
        /* Declare the print function as friend. */
        friend std::ostream& print_warning(std::ostream& os, const ADLWarning& e);

    public:
        /* The debug information of the token or node that caused this error. */
        const DebugInfo debug;

        /* Constructor for the ADLCompileWarning class, which takes:
         *   - the type of this warning
         *   - a DebugInfo struct containing the location in the source file
         *   - [optional] a message
         */
        ADLCompileWarning(const std::string& type, const DebugInfo& debug, const std::string& message = "") :
            ADLWarning(type, debug.filenames, message),
            debug(debug)
        {}

        /* Function that prints the compilation warning to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

    };



    /* Exception that stores additional information to accompany an error or warning. Always references a particular place in a source file. */
    class ADLNote: public ADLException {
    public:
        /* The debug information of this warning. */
        const DebugInfo debug;

        /* Constructor for the ADLNote class, which takes a DebugInfo struct to link the location of this error in a source file and a message making the note. */
        ADLNote(const DebugInfo& debug, const std::string& message) :
            ADLException(debug.filenames, message),
            debug(debug)
        {}
        
        /* Copies the ADLNote polymorphically. */
        virtual ADLNote* copy() const;

        /* Function that prints the note to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

    };



    /* Class that is used to group and then neatly print all exceptions and warnings in the ADL compiler. */
    class ExceptionHandler {
    private:
        /* Pointer to the list of all stored exceptions. */
        ADLException** exceptions;
        /* Number of exceptions stored in this handler. */
        size_t length;
        /* Maximum number of exception we allocated space for. */
        size_t max_length;
        
        /* Number of errors that have been logged in the handler. */
        size_t n_errors;

        /* Resizes the internal array, by doubling its size. */
        void resize();

        /* Private function that prints a note to the parser and then throws the given main error. */
        void add_note(const ADLNote& note);
        /* Private function that recursively adds extra notes to the parser. */
        template <class... NOTES>
        void add_note(const ADLNote& note, NOTES... rest) {
            // Note the note in our internal list, resizing as needed
            if (this->length >= this->max_length) { this->resize(); }
            this->exceptions[this->length++] = note.copy();
    
            // Don't forget to print it
            if (this->print_on_add) { note.print(std::cerr); }

            // Add the rest with the rest of the recursion
            this->add_note(rest...);
        }

    public:
        /* Stores whether or not we should print each exception when we add it. */
        bool print_on_add;
        
        /* Default constructor for the ExceptionHandler class, which optionally takes whether or not exceptions should be printed immediately and the initial size of the internal array. */
        ExceptionHandler(bool print_on_add = true, size_t initial_capacity=4);
        /* Copy constructor for the ExceptionHandler class. */
        ExceptionHandler(const ExceptionHandler& other);
        /* Move constructor for the ExceptionHandler class. */
        ExceptionHandler(ExceptionHandler&& other);
        /* Destructor for the ExceptionHandler class. */
        ~ExceptionHandler();

        /* Adds a new exception to the handler. */
        ExceptionHandler& log(const ADLException& except);
        /* Adds a new exception to the handler, which accompanying notes to add to this exception. */
        template <class... NOTES>
        ExceptionHandler& log(const ADLException& except, NOTES... notes) {
            // Log the exception in our internal list
            if (this->length >= this->max_length) { this->resize(); }
            this->exceptions[this->length++] = except.copy();
            if (dynamic_cast<const ADLError*>(&except)) { ++this->n_errors; }
    
            // Don't forget to print it
            if (this->print_on_add) { except.print(std::cerr); }

            // Then, use the notes recursion to add each note
            this->add_note(notes...);

            // When done, return ourselves so we may potentially be thrown
            return *this;
        }
        
        /* Returns a constant reference to the i'th exception in this handler. */
        inline const ADLException& operator[](size_t i) const { return *(this->exceptions[i]); }
        /* Returns the number of exceptions currently stored in this handler. */
        inline size_t size() const { return this->length; }
        /* Returns the number of errors (not warning) logged in the handler. */
        inline size_t errors() const { return this->n_errors; }
        
        /* Neatly prints all exceptions in this ExceptionHandler. */
        friend std::ostream& operator<<(std::ostream& os, const ExceptionHandler& except);

        /* Copy assignment operator for the ExceptionHandler class. */
        inline ExceptionHandler& operator=(const ExceptionHandler& other) { return *this = ExceptionHandler(other); }
        /* Move assignment operator for the ExceptionHandler class. */
        ExceptionHandler& operator=(ExceptionHandler&& other);
        /* Swap operator for the ExceptionHandler class. */
        friend void swap(ExceptionHandler& eh1, ExceptionHandler& eh2);

    };
    /* Neatly prints all exceptions in this ExceptionHandler. */
    std::ostream& operator<<(std::ostream& os, const ExceptionHandler& except);
    /* Swap operator for the ExceptionHandler class. */
    void swap(ExceptionHandler& eh1, ExceptionHandler& eh2);



    /* Static error handler, which can be used to write errors to from the entire parser. */
    static ExceptionHandler error_handler;


    
    /* Shortcut for the handler's log function. */
    inline ExceptionHandler& log(const ADLException& except) { return error_handler.log(except); }
    /* Shortcut for the handler's log function (with note support). */
    template <class... NOTES>
    inline ExceptionHandler& log(const ADLException& except, NOTES... notes) { return error_handler.log(except, notes...); }

}

#endif
