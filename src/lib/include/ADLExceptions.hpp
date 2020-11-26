/* ADLEXCEPTIONS.hpp
 *   by Lut99
 *
 * Created:
 *   14/11/2020, 16:14:52
 * Last edited:
 *   26/11/2020, 15:47:15
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Easily includable file that defines the ArgumentParser's basic
 *   exceptions, including a way to pretty print them.
**/

#ifndef ADL_EXCEPTIONS_HPP
#define ADL_EXCEPTIONS_HPP

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

    };



    /* Baseclass exception for all non-recoverable errors in the parser. */
    class ADLError : public ADLException {
    public:
        /* Constructor for the ADLError class, which takes the relevant include path of files and optionally a message. */
        ADLError(const std::vector<std::string>& filenames, const std::string& message = "") :
            ADLException(filenames, message)
        {}

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
         *   - a list of files we tried to parse (breadcrumb-style)
         *   - a DebugInfo struct containing the location in the source file
         *   - [optional] a message
         */
        ADLCompileError(const std::vector<std::string>& filenames, const DebugInfo& debug, const std::string& message = "") :
            ADLError(filenames, message),
            debug(debug)
        {}

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

        /* Pretty-prints this Exception as warning. */
        virtual std::ostream& write(std::ostream& os) const {
            // Just print the file, 'warning' the message and the warning type, since we don't have any more information
            for (size_t i = 0; i < this->filenames.size() - 1; i++) {
                os << "\033[1m" << this->filenames[i] << ":\033[0m" << std::endl << "--> ";
            }
            return os << "\033[1m" << this->filenames[this->filenames.size() - 1] << ": \033[35mwarning:\033[0m " << this->message << " [\033[35;1m-W" << this->type << "\033[0m]" << std::endl;
        }

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
         *   - a list of files we tried to parse (breadcrumb-style)
         *   - a DebugInfo struct containing the location in the source file
         *   - [optional] a message
         */
        ADLCompileWarning(const std::string& type, const std::vector<std::string>& filenames, const DebugInfo& debug, const std::string& message = "") :
            ADLWarning(type, filenames, message),
            debug(debug)
        {}

    };



    /* Creates a pretty string for a derived ADLError class. */
    std::ostream& print_error(std::ostream& os, const ADLError& e);
    /* Creates a pretty string for a derived ADLWarning class. */
    std::ostream& print_warning(std::ostream& os, const ADLWarning& e);

}

#endif
