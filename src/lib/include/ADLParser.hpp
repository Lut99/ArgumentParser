/* ADLPARSER.hpp
 *   by Lut99
 *
 * Created:
 *   11/12/2020, 5:37:52 PM
 * Last edited:
 *   13/11/2020, 14:46:54
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADL Parser takes care of parsing the tokens that are returned from
 *   the ADL Tokenizer. See the Argument Definition Language specification
 *   for more information on the grammar that the parser implements. The
 *   output is a tree as found in ADLTree.hpp.
**/

#ifndef ADL_PARSER_HPP
#define ADL_PARSER_HPP

#include <vector>
#include <string>
#include <exception>

#include "ADLTree.hpp"

namespace ArgumentParser {
    namespace Exceptions {
        /* Baseclass exception for all Parser-related exceptions. */
        class ParseException : public std::exception {
        protected:
            /* The trail of filenames we are parsing. */
            std::vector<std::string> filenames;
            /* The line number where the exception occurred in the current file. */
            size_t line;
            /* The column number where the exception occurred in the current file. */
            size_t col;
            /* The actual, raw line where the exception occurred. */
            std::string raw_line;
            /* The message that any derived class wants us to print. */
            std::string message;

        public:
            /* Constructor for the ParseException class, which takes a list of files we tried to parse (breadcrumb-style), the line number where the error occurred, the column number, the actual, raw line where the error happened and optionally a message. */
            ParseException(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line, const std::string& message = "") :
                filenames(filenames),
                line(line),
                col(col),
                raw_line(raw_line),
                message(message)
            {}

            /* Returns the file where the exception occurred. */
            inline std::string get_filename() const { return this->filenames.size() > 0 ? this->filenames[0] : "<none>"; }
            /* Returns the breadcrumb of files where the exception occurred. */
            inline std::vector<std::string> get_filenames() const { return this->filenames; }
            /* Returns the line number where the exception occurred. */
            inline size_t get_line() const { return this->line; }
            /* Returns the column number where the exception occurred. */
            inline size_t get_col() const { return this->col; }
            /* Returns the raw line where the exception occurred. */
            inline std::string get_raw_line() const { return this->get_raw_line(); }
            /* Returns the internal message. */
            inline std::string get_message() const { return this->message; }

            /* Allows the ParseException to be copied polymorphically. */
            virtual ParseException* copy() const {
                return new ParseException(*this);
            }

        };

    }

    /* Static "class" that is used to parse a file - and recursively all included files. */
    namespace Parser {
        /* Parses a given file, including any included files. Returns a single root node, from which the entire parsed tree is build. Does not immediately throw exceptions, but instead collects them as much as possible and therefore throws a vector of ParseExceptions. Use Exceptions::print_exceptions() to print them neatly. */
        ADLTree* parse(const std::string& filename);
    };
    
}

#endif
