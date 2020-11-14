/* ADLPARSER.hpp
 *   by Lut99
 *
 * Created:
 *   11/12/2020, 5:37:52 PM
 * Last edited:
 *   14/11/2020, 17:20:16
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

#include "ADLExceptions.hpp"
#include "ADLTree.hpp"

namespace ArgumentParser {
    namespace Exceptions {
        /* Baseclass exception for all Parser-related errors. */
        class ParseError : public ADLError {
        protected:
            /* The actual line, as string, where the error occurred - used for pretty printing. */
            std::string raw_line;

        public:
            /* The line number where the exception occurred in the current file. */
            const size_t line;
            /* The column number where the exception occurred in the current file. */
            const size_t col;

            /* Constructor for the ParseError class, which takes a list of files we tried to parse (breadcrumb-style), the line number where the error occurred, the column number, the actual, optionally a message and optionally a pretty message. */
            ParseError(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line, const std::string& message = "") :
                ADLError(filenames, message),
                raw_line(raw_line),
                line(line),
                col(col)
            {}

            /* Pretty-prints this Exception as warning. */
            virtual std::ostream& write(std::ostream& os) const {
                // First, print the include breadcrumbs
                for (size_t i = 0; i < this->filenames.size() - 1; i++) {
                    os << "\033[1m" << this->filenames[i] << ":\033[0m" << std::endl << "--> ";
                }

                // Then, print the location & the message
                os << "\033[1m" << this->filenames[this->filenames.size() - 1] << ":" << this->line << ":" << this->col << ": \033[31merror:\033[0m " << this->message << std::endl;

                // Print the raw line
                std::string strline = std::to_string(this->line);
                for (size_t i = strline.size(); i < 5; i++) { os << ' '; }
                os << strline << " | " << this->raw_line;

                // Determine the coloured part (it's an error, so red and bold)
                size_t w_start = this->raw_line.find_first_of("\033[31;1m") + 1;
                size_t w_size = this->raw_line.find_first_of("\033[0m") + 1 - w_start + 1;

                // Write empty lines until we reach w_start
                os << "      | ";
                for (size_t i = 1; i < w_start; i++) { os << ' '; }

                // Then, write '~' with the exception of col
                os << "\033[31;1m";
                for (size_t i = 0; i < w_size; i++) {
                    if (w_start + i == col) { os << '^'; }
                    else { os << '~'; }
                }
                return os << "\033[0m" << std::endl;
            }

        };

        /* Baseclass exception for all Parser-related warnings. */
        class ParseWarning : public ADLWarning {
        protected:
            /* The actual line, as string, where the error occurred - used for pretty printing. */
            std::string raw_line;

        public:
            /* The line number where the exception occurred in the current file. */
            const size_t line;
            /* The column number where the exception occurred in the current file. */
            const size_t col;

            /* Constructor for the ParseError class, which takes a list of files we tried to parse (breadcrumb-style), the line number where the error occurred, the column number, the actual, optionally a message and optionally a pretty message. */
            ParseWarning(const std::string& type, const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line, const std::string& message = "") :
                ADLWarning("parse-" + type, filenames, message),
                raw_line(raw_line),
                line(line),
                col(col)
            {}

            /* Pretty-prints this Exception as warning. */
            virtual std::ostream& write(std::ostream& os) const {
                // First, print the include breadcrumbs
                for (size_t i = 0; i < this->filenames.size() - 1; i++) {
                    os << "\033[1m" << this->filenames[i] << ":\033[0m" << std::endl << "--> ";
                }

                // Then, print the location & the message
                os << "\033[1m" << this->filenames[this->filenames.size() - 1] << ":" << this->line << ":" << this->col << ": \033[35warning:\033[0m " << this->message << " [\033[35;1m-W" << this->type << "\033[0m]" << std::endl;

                // Print the raw line
                std::string strline = std::to_string(this->line);
                for (size_t i = strline.size(); i < 5; i++) { os << ' '; }
                os << strline << " | " << this->raw_line;

                // Determine the coloured part (it's a warning, so purple and bold)
                size_t w_start = this->raw_line.find_first_of("\033[35;1m") + 1;
                size_t w_size = this->raw_line.find_first_of("\033[0m") + 1 - w_start + 1;

                // Write empty lines until we reach w_start
                os << "      | ";
                for (size_t i = 1; i < w_start; i++) { os << ' '; }

                // Then, write '~' with the exception of col
                os << "\033[35;1m";
                for (size_t i = 0; i < w_size; i++) {
                    if (w_start + i == col) { os << '^'; }
                    else { os << '~'; }
                }
                return os << "\033[0m" << std::endl;
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
