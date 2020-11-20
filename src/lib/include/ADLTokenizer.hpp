/* ADLTOKENIZER.hpp
 *   by Anonymous
 *
 * Created:
 *   05/11/2020, 16:17:58
 * Last edited:
 *   11/20/2020, 2:25:32 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Class that is used to Tokenize a given ADL file. Is used to read one
 *   token at a time from the stream, with the option to put tokens back.
**/

#ifndef ADL_TOKENIZER_HPP
#define ADL_TOKENIZER_HPP

#include <ostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <exception>
#include <unordered_map>

#include "ADLExceptions.hpp"

namespace ArgumentParser {
    namespace Exceptions {
        /* Baseclass exception for all Parser-related errors. */
        class TokenizeError : public ADLError {
        public:
            /* Constructor for the TokenizeError class, which takes a list of files we tried to parse (breadcrumb-style), the line number where the error occurred, the column number, the actual, optionally a message and optionally a pretty message. */
            TokenizeError(const std::vector<std::string>& filenames, const std::string& message = "") :
                ADLError(filenames, message)
            {}

        };

        /* Baseclass exception for all file related exceptions. */
        class IOError : public TokenizeError {
        public:
            /* Error number that occurred. */
            const int err_no;

            /* Constructor for the IOError class, which takes the name of the file that we tried to open, the errno that occured and optionally a message. */
            IOError(const std::vector<std::string>& filenames, const int err_no, const std::string& message = "") :
                TokenizeError(filenames, message),
                err_no(err_no)
            {}

        };
        /* Exception for when a file could not be opened. */
        class FileOpenError : public IOError {
        public:
            /* Constructor for the FileOpenError class, which takes the name of the file that we tried to open and the errno that occured. */
            FileOpenError(const std::vector<std::string>& filenames, const int err_no) :
                IOError(filenames, err_no, "Could not open file: " + std::string(std::strerror(err_no)))
            {}

        };
        /* Exception for when a read error occurred. */
        class FileReadError : public IOError {
        public:
            /* Constructor for the FileReadError class, which takes the name of the file that we tried to open and the errno that occured. */
            FileReadError(const std::vector<std::string>& filenames, const int err_no) :
                IOError(filenames, err_no, "Could not read from file: " + std::string(std::strerror(err_no)))
            {}

        };

        /* Baseclass exception for all syntax errors. */
        class SyntaxError: public ADLCompileError {
        public:
            /* Constructor for the SyntaxError class, which takes:
             *   - a list of files we tried to parse (breadcrumb-style)
             *   - the line number where the error occurred
             *   - the column number where the error occurred
             *   - the line where the error occurred itself
             *   - the start of the part of the raw_line that is erronous
             *   - the size of the part of the raw_line that is erronous
             *   - [optional] a message
             */
            SyntaxError(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line, const std::string& message = "") :
                ADLCompileError(filenames, line, col, raw_line, message)
            {}

        };
        /* Exception for when an illegal character occurred. */
        class UnexpectedCharException : public SyntaxError {
        public:
            /* The character that was illegal. */
            const char c;

            /* Constructor for the UnexpectedCharException, which takes the file where the illegal character occurred, the line number of its occurrence, the column number, the actual line where the error occurred and the illegal character itself. */
            UnexpectedCharException(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line, const char c) :
                SyntaxError(filenames, line, col, raw_line, (std::string("Unexpected character '") += c) + "'."),
                c(c)
            {}

        };
        /* Exception for when a single dash isn't followed by an expected character. */
        class EmptyShortlabelException: public SyntaxError {
        public:
            /* Constructor for the EmptyShortlabelException class, which takes the name of the file where the empty dash occurred, the line number where it did, the column number and the actual line where the error occurred. */
            EmptyShortlabelException(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line) :
                SyntaxError(filenames, line, col, raw_line, "Encountered empty shortlabel.")
            {}

        };
        /* Exception for when a dash isn't empty, but isn't followed by an expected character either. */
        class IllegalShortlabelException: public SyntaxError {
        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalShortlabelException class, which takes the name of the file where the illegal dash occurred, the line number where it did, the column number, the actual line where the error occurred and the illegal character itself. */
            IllegalShortlabelException(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line, const char c) :
                SyntaxError(filenames, line, col, raw_line, (std::string("Encountered illegal character '") += c) + "' for shortlabel."),
                c(c)
            {}

        };
        /* Exception for when a double dash isn't followed by an expected character. */
        class EmptyLonglabelException: public SyntaxError {
        public:
            /* Constructor for the EmptyLonglabelException class, which takes the name of the file where the empty dash occurred, the line number where it did, the column number and the actual line where the error occurred. */
            EmptyLonglabelException(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line) :
                SyntaxError(filenames, line, col, raw_line,  "Encountered empty longlabel.")
            {}

        };
        /* Exception for when a dash isn't empty, but isn't followed by an expected character either. */
        class IllegalLonglabelException: public SyntaxError {
        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalLonglabelException class, which takes the name of the file where the illegal dash occurred, the line number where it did, the column number, the actual line where the error occurred and the illegal character itself. */
            IllegalLonglabelException(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line, const char c) :
                SyntaxError(filenames, line, col, raw_line, (std::string("Encountered illegal character '") += c) + "' for longlabel."),
                c(c)
            {}

        };
        /* Exception for when a triple dash isn't followed by an expected character. */
        class EmptyNegativeException: public SyntaxError {
        public:
            /* Constructor for the EmptyNegativeException class, which takes the name of the file where the empty dash occurred, the line number where it did, the column number and the actual line where the error occurred. */
            EmptyNegativeException(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line) :
                SyntaxError(filenames, line, col, raw_line, "Encountered negative sign without value.")
            {}

        };
        /* Exception for when a dash isn't empty, but isn't followed by an expected character either. */
        class IllegalNegativeException: public SyntaxError {
        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalNegativeException class, which takes the name of the file where the illegal dash occurred, the line number where it did, the column number, the actual line where the error occurred and the illegal character itself. */
            IllegalNegativeException(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line, const char c) :
                SyntaxError(filenames, line, col, raw_line, (std::string("Encountered non-numeric character '") += c) + "' while parsing a numeric value."),
                c(c)
            {}

        };
        /* Exception for when a triple dash isn't followed by an expected character. */
        class EmptyTypeException: public SyntaxError {
        public:
            /* Constructor for the EmptyTypeException class, which takes the name of the file where the empty dash occurred, the line number where it did, the column number and the actual line where the error occurred. */
            EmptyTypeException(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line) :
                SyntaxError(filenames, line, col, raw_line, "Encountered empty type identifier.")
            {}

        };
        /* Exception for when a type contains illegal characters. */
        class IllegalTypeException: public SyntaxError {
        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalTypeException class, which takes the name of the file where the illegal dash occurred, the line number where it did, the column number, the actual line where the error occurred and the illegal character itself. */
            IllegalTypeException(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line, const char c) :
                SyntaxError(filenames, line, col, raw_line, (std::string("Encountered illegal character '") += c) + "' for a type identifier."),
                c(c)
            {}

        };
        /* Exception for when an illegal character is escaped. */
        class IllegalEscapeException: public SyntaxError {
        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalEscapeException class, which takes the name of the file where the illegal dash occurred, the line number where it did, the column number, the actual line where the error occurred and the illegal character itself. */
            IllegalEscapeException(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line, const char c) :
                SyntaxError(filenames, line, col, raw_line, (std::string("Cannot escape character '") += c)),
                c(c)
            {}

        };
        /* Exception for when a newline is encountered in a regex-expression. */
        class IllegalRegexException: public SyntaxError {
        public:
            
        }
    }



    /* Enum for the Tokens, which is used to identify each separate token. */
    enum class TokenType {
        identifier = 0,
        shortlabel = 1,
        longlabel = 2,
        type = 3,
        string = 4,
        number = 5,
        decimal = 6,
        l_square = 7,
        r_square = 8,
        l_curly = 9,
        r_curly = 10,
        equals = 11,
        semicolon = 12,
        triple_dot = 13,
        directive = 14,
        regex = 15,
        empty = 16
    };

    /* Dictionary that maps a tokentype to a capitalized string. */
    const static std::string tokentype_names[] = {
        "Identifier",
        "Shortlabel",
        "Longlabel",
        "Type",
        "String",
        "Number",
        "Decimal",
        "LSquare",
        "RSquare",
        "LCurly",
        "RCurly",
        "Equals",
        "Semicolon",
        "TripleDot",
        "Directive",
        "Regex",
        "Empty"
    };

    /* The Token struct, which is used to convey information to the higher-level parser. */
    struct Token {
        /* The type of this Token. */
        TokenType type;
        /* The line number where this token started. */
        size_t line;
        /* The column number where this token started. */
        size_t col;
        /* The raw value of this token, if applicable. */
        std::string value;

        /* Allows a token to be written to an outstream. */
        friend std::ostream& operator<<(std::ostream& os, const Token& token);

    };
    /* Allows a token to be written to an outstream. */
    std::ostream& operator<<(std::ostream& os, const Token& token);




    /* The Tokenizer class can be used to open a file and read it token-by-token. Might throw any of the abovely-defined exceptions if syntax errors occur. */
    class Tokenizer {
    private:
        /* The breadcrumbs of included files. */
        std::vector<std::string> filenames;
        
        /* The file from which we will read the tokens. */
        FILE* file;
        /* Counter used internally over the line numbers. */
        size_t line;
        /* Counter used internall over the column numbers. */
        size_t col;
        /* The last position in the internal file where a newline was encountered. */
        long last_newline;

        /* Used to temporarily store tokens that were put back. */
        std::vector<Token> temp;

        /* Used internally to read the first token off the stream. */
        Token read_head();
        /* Reads the entire, current line from the internal file. */
        std::string get_line();

    public:
        /* The path we are currently parsing. */
        const std::string path;

        /* Constructor for the Tokenizer class, which takes the path to the file we should read. */
        Tokenizer(const std::vector<std::string>& filenames);
        /* The copy constructor for the Tokenizer class has been deleted, as it makes no sense to copy a stream (which the Tokenizer pretends to be). */
        Tokenizer(const Tokenizer& other) = delete;
        /* Move constructor for the Tokenizer class. */
        Tokenizer(Tokenizer&& other);
        /* Destructor for the Tokenizer class. */
        ~Tokenizer();

        /* Looks at the top token of the stream without removing it. */
        Token peek();
        /* Removes the top token of the stream and returns it. */
        Token pop();
        /* Puts a token back on the stream. Note that it will not be parsed again, so may retrieving it will be much faster than the first time. */
        void push(const Token& token);

        /* Returns true if an end-of-file has been reached. */
        inline bool eof() const { return feof(this->file); }

    };

}

#endif
