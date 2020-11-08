/* ADLTOKENIZER.hpp
 *   by Anonymous
 *
 * Created:
 *   05/11/2020, 16:17:58
 * Last edited:
 *   08/11/2020, 18:14:03
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Class that is used to Tokenize a given ADL file. Is used to read one
 *   token at a time from the stream, with the option to put tokens back.
**/

#ifndef ADL_TOKENIZER_HPP
#define ADL_TOKENIZER_HPP

#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <exception>

namespace ArgumentParser {
    namespace Exceptions {
        /* Baseclass exception from which all Tokenizer-related errors derive. */
        class TokenizerException : public std::exception {
        protected:
            /* Message that describes the error. */
            std::string message;
            /* Name of the file that we are trying to tokenize. */
            std::string name;

            /* Used to let derived classes add their own part of the message through the tree. */
            std::string generate_message(const std::string& filename, const std::string& message) {
                return "Error while tokenizing '" + filename + "'" + (message.empty() ? "." : ": " + message);
            }
        
        public:
            /* Constructor for the TokenizerException that takes the name of the file that we attempted to Tokenize and optionally a message. */
            TokenizerException(const std::string& file_name, const std::string& message = "") :
                message(message),
                name(file_name)
            {}

            /* Returns a message describing what went wrong. */
            inline std::string get_message() const { return this->message; }
            /* Implementation for std::exception's what() function, so that it's compatible with C++'s uncaught-exception handling. */
            virtual const char* what() const noexcept { return this->message.c_str(); }
            /* Returns the name of the file we attempted to tokenize. */
            inline std::string get_file_name() const { return this->name; }

            /* Allows the TokenizerException to be copied polymorphically. */
            virtual TokenizerException* copy() const { return new TokenizerException(*this); }

        };

        /* Baseclass exception for all file related exceptions. */
        class IOException : public TokenizerException {
        protected:
            /* Error number that occurred. */
            int err_no;

        public:
            /* Constructor for the IOException class, which takes the name of the file that we tried to open, the errno that occured and optionally a message. */
            IOException(const std::string& name, const int err_no, const std::string& message = "") :
                TokenizerException(name, message),
                err_no(err_no)
            {}

            /* Returns the error number that occurred. */
            inline int get_errno() const { return this->err_no; }

            /* Allows the IOException to be copied polymorphically. */
            virtual IOException* copy() const { return new IOException(*this); }

        };
        /* Exception for when a file could not be opened. */
        class FileOpenException : public IOException {
        public:
            /* Constructor for the FileOpenException class, which takes the name of the file that we tried to open and the errno that occured. */
            FileOpenException(const std::string& name, const int err_no) :
                IOException(name, err_no, generate_message(name, "Could not open file: " + std::string(std::strerror(err_no))))
            {}

            /* Allows the FileOpenException to be copied polymorphically. */
            virtual FileOpenException* copy() const { return new FileOpenException(*this); }

        };
        /* Exception for when a read error occurred. */
        class FileReadException : public IOException {
        public:
            /* Constructor for the FileReadException class, which takes the name of the file that we tried to open and the errno that occured. */
            FileReadException(const std::string& name, const int err_no) :
                IOException(name, err_no, generate_message(name, "Could not read from file: " + std::string(std::strerror(err_no))))
            {}

            /* Allows the FileReadException to be copied polymorphically. */
            virtual FileReadException* copy() const { return new FileReadException(*this); }

        };

        /* Baseclass exception for all syntax errors. */
        class SyntaxError: public TokenizerException {
        protected:
            /* Line number where the syntax error occurred. */
            size_t line;
            /* Column number where the syntax error occurred. */
            size_t col;

            /* Used to let derived classes add their own part of the message through the tree. */
            std::string generate_message(const std::string& filename, const size_t line, const size_t col, const std::string& message) {
                return "Syntax error in '" + filename + "' at line " + std::to_string(line) + ", col " + std::to_string(col) + (message.empty() ? "." : ": " + message);
            }

        public:
            /* Constructor for the SyntaxError class, which takes the file where the syntax error occurred, the line number of the occurence, the column number and optionally a message. */
            SyntaxError(const std::string& filename, const size_t line, const size_t col, const std::string& message = "") :
                TokenizerException(filename, message),
                line(line),
                col(col)
            {}

            /* Returns the line number of where the syntax error occurred. */
            inline size_t get_line() const { return this->line; }
            /* Returns the column number of where the syntax error occurred. */
            inline size_t get_col() const { return this->col; }
            
            /* Allows the SyntaxError to be copied polymorphically. */
            virtual SyntaxError* copy() const { return new SyntaxError(*this); }

        };
        /* Exception for when an illegal character occurred. */
        class IllegalCharException : public SyntaxError {
        private:
            /* The character that was illegal. */
            char c;

        public:
            /* Constructor for the IllegalCharException, which takes the file where the illegal character occurred, the line number of its occurrence, the column number and the illegal character itself. */
            IllegalCharException(const std::string& filename, const size_t line, const size_t col, const char c) :
                SyntaxError(filename, line, col, generate_message(
                                filename, col, line, (std::string("Encountered illegal character '") += c) + "'."
                            )),
                c(c)
            {}
            
            /* Returns the illegal character that we encountered. */
            inline char get_c() const { return this->c; }

            /* Allows the IllegalCharException to be copied polymorphically. */
            virtual IllegalCharException* copy() const { return new IllegalCharException(*this); }

        };
        /* Exception for when too many characters have been read for a single value. */
        class ValueOverflowException : public SyntaxError {
        private:
            /* The maximum number of characters allowed (excl. null-terminator). */
            size_t max;
        
        public:
            /* Constructor for the ValueOverflowException, which takes the file where the overflow occurred, the line number of its occurrence, the column number and the maximum number of characters allowed. */
            ValueOverflowException(const std::string& filename, const size_t line, const size_t col, const size_t max_chars) :
                SyntaxError(filename, line, col, generate_message(
                    filename, line, col, "Too many consecutive characters encountered (maximum: " + std::to_string(max_chars) + ")."
                )),
                max(max_chars)
            {}

            /* Returns the maximum number of characters allowed. */
            inline size_t get_max_chars() const { return this->max; }

            /* Allows the ValueOverflowException to be copied polymorphically. */
            virtual ValueOverflowException* copy() const { return new ValueOverflowException(*this); }

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
        semicolon = 11
    };



    /* The Token struct, which is used to convey information to the higher-level parser. */
    struct Token {
        /* The maximum number of characters allowed in a Token value. */
        static constexpr size_t max_size = 2048;

        /* The type of this Token. */
        TokenType type;
        /* The line number where this token started. */
        size_t line;
        /* The column number where this token started. */
        size_t col;
        /* The raw value of this token, if applicable. */
        char value[max_size];

    };



    /* The Tokenizer class can be used to open a file and read it token-by-token. Might throw any of the abovely-defined exceptions if syntax errors occur. */
    class Tokenizer {
    private:
        /* Path to the file which we will read the tokens from. */
        std::string path;
        /* The file from which we will read the tokens. */
        FILE* file;
        /* Counter used internally over the line numbers. */
        size_t line;
        /* Counter used internall over the column numbers. */
        size_t col;

        /* Used to temporarily store tokens that were put back. */
        std::vector<Token> temp;

        /* Used internally to read the first token off the stream. */
        Token _read_head();

    public:
        /* Constructor for the Tokenizer class, which takes the path to the file we should read. */
        Tokenizer(const std::string& path);
        /* The copy constructor for the Tokenizer class has been deleted, as it makes no sense to copy a stream (which the Tokenizer pretends to be). */
        Tokenizer(const Tokenizer& other) = delete;
        /* Move constructor for the Tokenizer class. */
        Tokenizer(Tokenizer&& other);
        /* Destructor for the Tokenizer class. */
        ~Tokenizer();

        /* Looks at the top token of the stream without removing it. */
        Token peek() const;
        /* Removes the top token of the stream and returns it. */
        Token pop();
        /* Puts a token back on the stream. Note that it will not be parsed again, so may retrieving it will be much faster than the first time. */
        void push(const Token& token);

        /* There is no copy assignment operator for the Tokenizer class, as it makes no sense to copy a stream (which the Tokenizer pretends to be). */
        Tokenizer& operator=(const Tokenizer& other) = delete;
        /* Move assignment operator for the Tokenizer class. */
        Tokenizer& operator=(Tokenizer&& other);
        /* Swap operator for the Tokenizer class. */
        friend void swap(Tokenizer& t1, Tokenizer& t2);

    };
    /* Swap operator for the Tokenizer class. */
    void swap(Tokenizer& t1, Tokenizer& t2);

}

#endif
