/* ADLTOKENIZER.hpp
 *   by Anonymous
 *
 * Created:
 *   05/11/2020, 16:17:58
 * Last edited:
 *   26/11/2020, 16:26:23
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
#include <limits>

#include "DebugInfo.hpp"
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
             *   - a DebugInfo struct linking this exception to a place in the source file
             *   - [optional] a message
             */
            SyntaxError(const std::vector<std::string>& filenames, const DebugInfo& debug, const std::string& message = "") :
                ADLCompileError(filenames, debug, message)
            {}

        };
        /* Exception for when an illegal character occurred. */
        class UnexpectedCharException : public SyntaxError {
        public:
            /* The character that was illegal. */
            const char c;

            /* Constructor for the UnexpectedCharException, which takes the file where the illegal character occurred, a DebugInfo struct linking this exception to a place in the source file and the illegal character itself. */
            UnexpectedCharException(const std::vector<std::string>& filenames, const DebugInfo& debug, const char c) :
                SyntaxError(filenames, debug, (std::string("Unexpected character '") += c) + "'."),
                c(c)
            {}

        };
        /* Exception for when a single dash isn't followed by an expected character. */
        class EmptyShortlabelException: public SyntaxError {
        public:
            /* Constructor for the EmptyShortlabelException class, which takes the name of the file where the empty dash occurred and a DebugInfo struct linking this exception to a place in the source file. */
            EmptyShortlabelException(const std::vector<std::string>& filenames, const DebugInfo& debug) :
                SyntaxError(filenames, debug, "Encountered empty shortlabel.")
            {}

        };
        /* Exception for when a dash isn't empty, but isn't followed by an expected character either. */
        class IllegalShortlabelException: public SyntaxError {
        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalShortlabelException class, which takes the name of the file where the illegal dash occurred, a DebugInfo struct linking this exception to a place in the source file and the illegal character itself. */
            IllegalShortlabelException(const std::vector<std::string>& filenames, const DebugInfo& debug, const char c) :
                SyntaxError(filenames, debug, (std::string("Encountered illegal character '") += c) + "' for shortlabel."),
                c(c)
            {}

        };
        /* Exception for when a double dash isn't followed by an expected character. */
        class EmptyLonglabelException: public SyntaxError {
        public:
            /* Constructor for the EmptyLonglabelException class, which takes the name of the file where the empty dash occurred and a DebugInfo struct linking this exception to a place in the source file. */
            EmptyLonglabelException(const std::vector<std::string>& filenames, const DebugInfo& debug) :
                SyntaxError(filenames, debug,  "Encountered empty longlabel.")
            {}

        };
        /* Exception for when a dash isn't empty, but isn't followed by an expected character either. */
        class IllegalLonglabelException: public SyntaxError {
        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalLonglabelException class, which takes the name of the file where the illegal dash occurred, a DebugInfo struct linking this exception to a place in the source file and the illegal character itself. */
            IllegalLonglabelException(const std::vector<std::string>& filenames, const DebugInfo& debug, const char c) :
                SyntaxError(filenames, debug, (std::string("Encountered illegal character '") += c) + "' for longlabel."),
                c(c)
            {}

        };
        /* Exception for when a triple dash isn't followed by an expected character. */
        class EmptyNegativeException: public SyntaxError {
        public:
            /* Constructor for the EmptyNegativeException class, which takes the name of the file where the empty dash occurred and a DebugInfo struct linking this exception to a place in the source file. */
            EmptyNegativeException(const std::vector<std::string>& filenames, const DebugInfo& debug) :
                SyntaxError(filenames, debug, "Encountered negative sign without value.")
            {}

        };
        /* Exception for when a dash isn't empty, but isn't followed by an expected character either. */
        class IllegalNegativeException: public SyntaxError {
        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalNegativeException class, which takes the name of the file where the illegal dash occurred, a DebugInfo struct linking this exception to a place in the source file and the illegal character itself. */
            IllegalNegativeException(const std::vector<std::string>& filenames, const DebugInfo& debug, const char c) :
                SyntaxError(filenames, debug, (std::string("Encountered non-numeric character '") += c) + "' while parsing a numeric value."),
                c(c)
            {}

        };
        /* Exception for when a triple dash isn't followed by an expected character. */
        class EmptyTypeException: public SyntaxError {
        public:
            /* Constructor for the EmptyTypeException class, which takes the name of the file where the empty dash occurred and a DebugInfo struct linking this exception to a place in the source file. */
            EmptyTypeException(const std::vector<std::string>& filenames, const DebugInfo& debug) :
                SyntaxError(filenames, debug, "Encountered empty type identifier.")
            {}

        };
        /* Exception for when a type contains illegal characters. */
        class IllegalTypeException: public SyntaxError {
        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalTypeException class, which takes the name of the file where the illegal dash occurred, a DebugInfo struct linking this exception to a place in the source file and the illegal character itself. */
            IllegalTypeException(const std::vector<std::string>& filenames, const DebugInfo& debug, const char c) :
                SyntaxError(filenames, debug, (std::string("Encountered illegal character '") += c) + "' for a type identifier."),
                c(c)
            {}

        };
        /* Exception for when an illegal character is parsed while parsing strings. */
        class IllegalStringException: public SyntaxError {
        private:
            /* Possible beautifies the illegal character somewhat. */
            std::string make_readable(char c) const {
                if (c == '\n') { return "\\n"; }
                else if (c == '\r') { return "\\r"; }
                else if (c == '\t') { return "\\t"; }
                else { return std::to_string(c); }
            }

        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalStringException class, which takes the name of the file where the illegal dash occurred, a DebugInfo struct linking this exception to a place in the source file and the illegal character itself. */
            IllegalStringException(const std::vector<std::string>& filenames, const DebugInfo& debug, const char c) :
                SyntaxError(filenames, debug, "Encountered non-readable character '" + this->make_readable(c) + "' while parsing string."),
                c(c)
            {}

        };
    
        /* Baseclass exception for when a value goes out of the allowed range. */
        class OutOfRangeWarning: public ADLCompileWarning {
        public:
            /* Constructor for the OutOfRangeError class, which takes:
             *   - a list of files we tried to parse (breadcrumb-style)
             *   - the line number where the error occurred
             *   - the column number where the error occurred
             *   - the line where the error occurred itself
             *   - [optional] a message
             */
            OutOfRangeWarning(const std::vector<std::string>& filenames, const DebugInfo& debug, const std::string& message = "") :
                ADLCompileWarning("out-of-range", filenames, debug, message)
            {}

        };
        /* Exception that is thrown when a given number overflows (i.e., it's larger than T::max()). */
        class OverflowWarning: public OutOfRangeWarning {
        public:
            /* Constructor for the OverflowWarning class, which takes a breadcrumb path of files we tried to parse, the line number where the overflow occurred, the column number and the raw line itself where the error occurred. */
            OverflowWarning(const std::vector<std::string>& filenames, const DebugInfo& debug) :
                OutOfRangeWarning(filenames, debug, "Overflow of integral constant (larger than " + std::to_string(std::numeric_limits<long>::max()) + ")")
            {}

        };
        /* Exception that is thrown when a given floating-point number overflows. */
        class FloatOverflowWarning: public OutOfRangeWarning {
        public:
            /* Constructor for the FloatOverflowWarning class, which takes a breadcrumb path of files we tried to parse, the line number where the overflow occurred, the column number and the raw line itself where the error occurred. */
            FloatOverflowWarning(const std::vector<std::string>& filenames, const DebugInfo& debug) :
                OutOfRangeWarning(filenames, debug, "Overflow of decimal constant (larger than " + std::to_string(std::numeric_limits<double>::max()) + ")")
            {}

        };
        /* Exception that is thrown when a given number underflows (i.e., it's smaller than T::min()). */
        class UnderflowWarning: public OutOfRangeWarning {
        public:
            /* Constructor for the UnderflowWarning class, which takes a breadcrumb path of files we tried to parse, the line number where the underflow occurred, the column number and the raw line itself where the error occurred. */
            UnderflowWarning(const std::vector<std::string>& filenames, const DebugInfo& debug) :
                OutOfRangeWarning(filenames, debug, "Underflow of integral constant (smaller than " + std::to_string(std::numeric_limits<long>::min()) + ")")
            {}

        };
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
    class Token {
    protected:
        /* Internal, polymorphic print function. */
        virtual std::ostream& print(std::ostream& os) const;

    public:
        /* The type of this Token. */
        TokenType type;
        /* Debug information for this token. */
        DebugInfo debug;
        /* The raw value of this token, if applicable. */
        std::string raw;

        /* Default constructor for the Token class. */
        Token(): debug(di_empty) {}
        /* Virtual destructor for the Token class. */
        virtual ~Token() {}

        /* Allows a token to be written to an outstream. */
        inline friend std::ostream& operator<<(std::ostream& os, const Token& token) { return token.print(os); }
        /* Allows a token to be written to an outstream. */
        inline friend std::ostream& operator<<(std::ostream& os, Token* token) { return token->print(os); }

        /* Allows the Token to be copied polymorphically. */
        virtual Token* copy() const;

    };

    /* Derived ValueToken class, which can store an already parsed, non-string value. */
    template <class T>
    class ValueToken : public Token {
    private:
        /* Internal, polymorphic print function. */
        virtual std::ostream& print(std::ostream& os) const;

    public:
        /* The parsed value this token stores. */
        T value;

        /* Default constructor for the ValueToken class. */
        ValueToken() {}
        
        /* "Promotes" an existing Token to a ValueToken, by copying it and adding our value. Note that the given token will be deallocated. */
        static ValueToken<T>* promote(Token* other, const T& value);

        /* Allows the ValueToken to be copied polymorphically. */
        virtual ValueToken<T>* copy() const;

    };



    /* Returns whether or not a tokentype is one of the value symbols. */
    #define IS_VALUE(TOKEN) \
        ((TOKEN)->type == TokenType::string || (TOKEN)->type == TokenType::regex || (TOKEN)->type == TokenType::number || (TOKEN)->type == TokenType::decimal)



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
        /* If set to true, will not tokenize any further. */
        bool done_tokenizing;

        /* Used to temporarily store tokens that were put back. */
        std::vector<Token*> temp;

        /* Used internally to read the first token off the stream. */
        Token* read_head();
        /* Reads the entire, current line from the internal file. */
        LineSnippet get_line();

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
        Token* peek();
        /* Removes the top token of the stream and returns it. */
        Token* pop();
        /* Puts a token back on the stream. Note that it will not be parsed again, so may retrieving it will be much faster than the first time. */
        inline void push(const Token& token) { return this->push((Token*) &token); }
        /* Puts a token back on the stream. Note that it will not be parsed again, so may retrieving it will be much faster than the first time. */
        template <class T>
        inline void push(const ValueToken<T>& token) { return this->push((Token*) &token); }
        /* Puts a token back on the stream. Note that it will not be parsed again, so may retrieving it will be much faster than the first time. */
        void push(Token* token);

        /* Returns true if an end-of-file has been reached. */
        inline bool eof() const { return this->done_tokenizing; }

    };

}

#endif
