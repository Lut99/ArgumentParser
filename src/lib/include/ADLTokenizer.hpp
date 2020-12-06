/* ADLTOKENIZER.hpp
 *   by Lut99
 *
 * Created:
 *   05/11/2020, 16:17:58
 * Last edited:
 *   06/12/2020, 13:56:51
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Class that is used to Tokenize a given ADL file. Is used to read one
 *   token at a time from the stream, with the option to put tokens back.
**/

#ifndef ADL_TOKENIZER_HPP
#define ADL_TOKENIZER_HPP

#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <cstring>
#include <exception>
#include <unordered_map>
#include <limits>

#include "DebugInfo.hpp"
#include "ADLExceptions.hpp"
#include "TokenTypes.hpp"

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

            /* Copies the IOError polymorphically. */
            virtual IOError* copy() const { return new IOError(*this); }

        };
        /* Exception for when a file could not be opened. */
        class FileOpenError : public IOError {
        public:
            /* Constructor for the FileOpenError class, which takes the name of the file that we tried to open and the errno that occured. */
            FileOpenError(const std::vector<std::string>& filenames, const int err_no) :
                IOError(filenames, err_no, "Could not open file: " + std::string(std::strerror(err_no)))
            {}

            /* Copies the FileOpenError polymorphically. */
            virtual FileOpenError* copy() const { return new FileOpenError(*this); }

        };
        /* Exception for when a read error occurred. */
        class FileReadError : public IOError {
        public:
            /* Constructor for the FileReadError class, which takes the name of the file that we tried to open and the errno that occured. */
            FileReadError(const std::vector<std::string>& filenames, const int err_no) :
                IOError(filenames, err_no, "Could not read from file: " + std::string(std::strerror(err_no)))
            {}

            /* Copies the FileReadError polymorphically. */
            virtual FileReadError* copy() const { return new FileReadError(*this); }

        };

        /* Baseclass exception for all syntax errors. */
        class SyntaxError: public ADLCompileError {
        public:
            /* Constructor for the SyntaxError class, which takes:
             *   - a DebugInfo struct linking this exception to a place in the source file
             *   - [optional] a message
             */
            SyntaxError(const DebugInfo& debug, const std::string& message = "") :
                ADLCompileError(debug, message)
            {}

        };
        /* Exception for when an illegal character occurred. */
        class UnexpectedCharException : public SyntaxError {
        public:
            /* The character that was illegal. */
            const char c;

            /* Constructor for the UnexpectedCharException, which takes a DebugInfo struct linking this exception to a place in the source file and the illegal character itself. */
            UnexpectedCharException(const DebugInfo& debug, const char c) :
                SyntaxError(debug, (std::string("Unexpected character '") += c) + "'."),
                c(c)
            {}

            /* Copies the UnexpectedCharException polymorphically. */
            virtual UnexpectedCharException* copy() const { return new UnexpectedCharException(*this); }

        };
        /* Exception for when a single dash isn't followed by an expected character. */
        class EmptyShortlabelException: public SyntaxError {
        public:
            /* Constructor for the EmptyShortlabelException class, which only takes a DebugInfo struct linking this exception to a place in the source file. */
            EmptyShortlabelException(const DebugInfo& debug) :
                SyntaxError(debug, "Encountered empty shortlabel.")
            {}

            /* Copies the EmptyShortlabelException polymorphically. */
            virtual EmptyShortlabelException* copy() const { return new EmptyShortlabelException(*this); }

        };
        /* Exception for when a dash isn't empty, but isn't followed by an expected character either. */
        class IllegalShortlabelException: public SyntaxError {
        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalShortlabelException class, which takes a DebugInfo struct linking this exception to a place in the source file and the illegal character itself. */
            IllegalShortlabelException(const DebugInfo& debug, const char c) :
                SyntaxError(debug, (std::string("Encountered illegal character '") += c) + "' for shortlabel."),
                c(c)
            {}

            /* Copies the IllegalShortlabelException polymorphically. */
            virtual IllegalShortlabelException* copy() const { return new IllegalShortlabelException(*this); }

        };
        /* Exception for when a double dash isn't followed by an expected character. */
        class EmptyLonglabelException: public SyntaxError {
        public:
            /* Constructor for the EmptyLonglabelException class, which only takes a DebugInfo struct linking this exception to a place in the source file. */
            EmptyLonglabelException(const DebugInfo& debug) :
                SyntaxError(debug,  "Encountered empty longlabel.")
            {}

            /* Copies the EmptyLonglabelException polymorphically. */
            virtual EmptyLonglabelException* copy() const { return new EmptyLonglabelException(*this); }

        };
        /* Exception for when a dash isn't empty, but isn't followed by an expected character either. */
        class IllegalLonglabelException: public SyntaxError {
        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalLonglabelException class, which takes a DebugInfo struct linking this exception to a place in the source file and the illegal character itself. */
            IllegalLonglabelException(const DebugInfo& debug, const char c) :
                SyntaxError(debug, (std::string("Encountered illegal character '") += c) + "' for longlabel."),
                c(c)
            {}

            /* Copies the IllegalLonglabelException polymorphically. */
            virtual IllegalLonglabelException* copy() const { return new IllegalLonglabelException(*this); }

        };
        /* Exception for when a triple dash isn't followed by an expected character. */
        class EmptyNegativeException: public SyntaxError {
        public:
            /* Constructor for the EmptyNegativeException class, which only takes a DebugInfo struct linking this exception to a place in the source file. */
            EmptyNegativeException(const DebugInfo& debug) :
                SyntaxError(debug, "Encountered negative sign without value.")
            {}

            /* Copies the EmptyNegativeException polymorphically. */
            virtual EmptyNegativeException* copy() const { return new EmptyNegativeException(*this); }

        };
        /* Exception for when a dash isn't empty, but isn't followed by an expected character either. */
        class IllegalNegativeException: public SyntaxError {
        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalNegativeException class, which takes a DebugInfo struct linking this exception to a place in the source file and the illegal character itself. */
            IllegalNegativeException(const DebugInfo& debug, const char c) :
                SyntaxError(debug, (std::string("Encountered non-numeric character '") += c) + "' while parsing a numeric value."),
                c(c)
            {}

            /* Copies the IllegalNegativeException polymorphically. */
            virtual IllegalNegativeException* copy() const { return new IllegalNegativeException(*this); }

        };
        /* Exception for when a triple dash isn't followed by an expected character. */
        class EmptyTypeException: public SyntaxError {
        public:
            /* Constructor for the EmptyTypeException class, which only takes a DebugInfo struct linking this exception to a place in the source file. */
            EmptyTypeException(const DebugInfo& debug) :
                SyntaxError(debug, "Encountered empty type identifier.")
            {}

            /* Copies the EmptyTypeException polymorphically. */
            virtual EmptyTypeException* copy() const { return new EmptyTypeException(*this); }

        };
        /* Exception for when a type contains illegal characters. */
        class IllegalTypeException: public SyntaxError {
        public:
            /* The illegal character we encountered. */
            const char c;

            /* Constructor for the IllegalTypeException class, which takes a DebugInfo struct linking this exception to a place in the source file and the illegal character itself. */
            IllegalTypeException(const DebugInfo& debug, const char c) :
                SyntaxError(debug, (std::string("Encountered illegal character '") += c) + "' for a type identifier."),
                c(c)
            {}

            /* Copies the IllegalTypeException polymorphically. */
            virtual IllegalTypeException* copy() const { return new IllegalTypeException(*this); }

        };
        /* Exception for when boolean brackets () are given but nothing is in them. */
        class EmptyBooleanException: public SyntaxError {
        public:
            /* Constructor for the EmptyBooleanException class, which only takes a DebugInfo struct linking this exception to a place in the source file. */
            EmptyBooleanException(const DebugInfo& debug) :
                SyntaxError(debug, "Encountered empty boolean value.")
            {}
  
            /* Copies the EmptyBooleanException polymorphically. */
            virtual EmptyBooleanException* copy() const { return new EmptyBooleanException(*this); }

        };
        /* Exception for when boolean brackets () are given but neither 'true' nor 'false' is in them. */
        class IllegalBooleanException: public SyntaxError {
        public:
            /* The illegal value that was encountered. */
            std::string value;

            /* Constructor for the IllegalBooleanException class, which takes a DebugInfo struct linking this exception to a place in the source file and the string that was given instead. */
            IllegalBooleanException(const DebugInfo& debug, const std::string& value) :
                SyntaxError(debug, "Encountered illegal boolean value '" + value + "' (expected 'true' or 'false' only)."),
                value(value)
            {}
  
            /* Copies the IllegalBooleanException polymorphically. */
            virtual IllegalBooleanException* copy() const { return new IllegalBooleanException(*this); }

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

            /* Constructor for the IllegalStringException class, which takes a DebugInfo struct linking this exception to a place in the source file and the illegal character itself. */
            IllegalStringException(const DebugInfo& debug, const char c) :
                SyntaxError(debug, "Encountered non-readable character '" + this->make_readable(c) + "' while parsing string."),
                c(c)
            {}

            /* Copies the IllegalStringException polymorphically. */
            virtual IllegalStringException* copy() const { return new IllegalStringException(*this); }

        };
        /* Exception for when a macro contains no characters. */
        class EmptyMacroException: public SyntaxError {
        public:
            /* Constructor for the EmptyMacroException class, which only takes a DebugInfo struct linking this exception to a place in the source file. */
            EmptyMacroException(const DebugInfo& debug) :
                SyntaxError(debug, "Encountered macro without name.")
            {}

            /* Copies the EmptyMacroException polymorphically. */
            virtual EmptyMacroException* copy() const { return new EmptyMacroException(*this); }

        };

        /* Exception for when a type identifier comment is unterminated. Contains a nested exception that contains an extra note. */
        class UnterminatedTypeException: public SyntaxError {
        public:
            /* Constructor for the UnterminatedTypeException class, which only takes a DebugInfo struct linking this exception to a place in the source file. */
            UnterminatedTypeException(const DebugInfo& debug) :
                SyntaxError(debug, "Unterminated type identifier encountered.")
            {}

            /* Copies the UnterminatedTypeException polymorphically. */
            virtual UnterminatedTypeException* copy() const { return new UnterminatedTypeException(*this); }

        };
        /* Exception for when a string value is unterminated. Contains a nested exception that contains an extra note. */
        class UnterminatedStringException: public SyntaxError {
        public:
            /* Constructor for the UnterminatedStringException class, which only takes a DebugInfo struct linking this exception to a place in the source file. */
            UnterminatedStringException(const DebugInfo& debug) :
                SyntaxError(debug, "Unterminated string encountered.")
            {}

            /* Copies the UnterminatedStringException polymorphically. */
            virtual UnterminatedStringException* copy() const { return new UnterminatedStringException(*this); }

        };
        /* Exception for when a boolean is unterminated. Contains a nested exception that contains an extra note. */
        class UnterminatedBooleanException: public SyntaxError {
        public:
            /* Constructor for the UnterminatedBooleanException class, which only takes a DebugInfo struct linking this exception to a place in the source file. */
            UnterminatedBooleanException(const DebugInfo& debug) :
                SyntaxError(debug, "Unterminated boolean value encountered.")
            {}

            /* Copies the UnterminatedBooleanException polymorphically. */
            virtual UnterminatedBooleanException* copy() const { return new UnterminatedBooleanException(*this); }

        };
        /* Exception for when a multi-line comment is unterminated. Contains a nested exception that contains an extra note. */
        class UnterminatedMultilineException: public SyntaxError {
        public:
            /* Constructor for the UnterminatedMultilineException class, which only takes a DebugInfo struct linking this exception to a place in the source file. */
            UnterminatedMultilineException(const DebugInfo& debug) :
                SyntaxError(debug, "Unterminated multi-comment encountered.")
            {}

            /* Copies the UnterminatedMultilineException polymorphically. */
            virtual UnterminatedMultilineException* copy() const { return new UnterminatedMultilineException(*this); }

        };

        /* Baseclass exception for when a value goes out of the allowed range. */
        class OutOfRangeWarning: public ADLCompileWarning {
        public:
            /* Constructor for the OutOfRangeError class, which takes:
             *   - the line number where the error occurred
             *   - the column number where the error occurred
             *   - the line where the error occurred itself
             *   - [optional] a message
             */
            OutOfRangeWarning(const DebugInfo& debug, const std::string& message = "") :
                ADLCompileWarning("out-of-range", debug, message)
            {}

        };
        /* Exception that is thrown when a given number overflows (i.e., it's larger than T::max()). */
        class OverflowWarning: public OutOfRangeWarning {
        public:
            /* Constructor for the OverflowWarning class, which only takes debugging information that links this error to a location in a source file. */
            OverflowWarning(const DebugInfo& debug) :
                OutOfRangeWarning(debug, "Overflow of integral constant (larger than " + std::to_string(std::numeric_limits<long>::max()) + ")")
            {}

            /* Copies the OverflowWarning polymorphically. */
            virtual OverflowWarning* copy() const { return new OverflowWarning(*this); }

        };
        /* Exception that is thrown when a given floating-point number overflows. */
        class FloatOverflowWarning: public OutOfRangeWarning {
        public:
            /* Constructor for the FloatOverflowWarning class, which only takes debugging information that links this error to a location in a source file. */
            FloatOverflowWarning(const DebugInfo& debug) :
                OutOfRangeWarning(debug, "Overflow of decimal constant (larger than " + std::to_string(std::numeric_limits<double>::max()) + ")")
            {}

            /* Copies the FloatOverflowWarning polymorphically. */
            virtual FloatOverflowWarning* copy() const { return new FloatOverflowWarning(*this); }

        };
        /* Exception that is thrown when a given number underflows (i.e., it's smaller than T::min()). */
        class UnderflowWarning: public OutOfRangeWarning {
        public:
            /* Constructor for the UnderflowWarning class, which only takes debugging information that links this error to a location in a source file. */
            UnderflowWarning(const DebugInfo& debug) :
                OutOfRangeWarning(debug, "Underflow of integral constant (smaller than " + std::to_string(std::numeric_limits<long>::min()) + ")")
            {}

            /* Copies the UnderflowWarning polymorphically. */
            virtual UnderflowWarning* copy() const { return new UnderflowWarning(*this); }

        };
    }



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
        /* The raw value of this token. */
        std::string raw;

        /* Default constructor for the Token class. */
        Token(): debug(di_empty) {}
        /* Constructor for the Token class, which takes each of its parameters. */
        Token(TokenType type, const DebugInfo& debug, const std::string& raw) :
            type(type), debug(debug), raw(raw)
        {}
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
        /* Pointer to the istream from which we read characters. */
        std::istream* file;
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
        /* The breadcrumbs of included files. */
        std::vector<std::string> filenames;
        /* The path we are currently parsing. */
        const std::string path;

        /* Constructor for the Tokenizer class, which takes an input stream and a path of file breadcrumbs telling the Tokenizer from where it's reading. */
        Tokenizer(std::istream* stream, const std::vector<std::string>& filenames);
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
