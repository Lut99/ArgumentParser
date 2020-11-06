/* ADLTOKENIZER.hpp
 *   by Anonymous
 *
 * Created:
 *   05/11/2020, 16:17:58
 * Last edited:
 *   06/11/2020, 16:53:57
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

    }



    /* Enum for the Tokens, which is used to identify each separate token. */
    enum class TokenType {

    };



    /* The Token baseclass, which is used to convey information to the higher-level parser. */
    class Token {
    protected:
        /* Constructor for the Token class, which takes its type, the line number where it was found and the column number. */
        Token(TokenType type, size_t line, size_t col) :
            type(type),
            line(line),
            col(col)
        {}

    public:
        /* The type of this Token. */
        TokenType type;
        /* The line number where this token started. */
        size_t line;
        /* The column number where this token started. */
        size_t col;

    };



    /* The Tokenizer class can be used to open a file and read it token-by-token. Might throw any of the abovely-defined exceptions if syntax errors occur. */
    class Tokenizer {
    private:
        /* Path to the file which we will read the tokens from. */
        std::string path;
        /* The file from which we will read the tokens. */
        FILE* file;

        /* Used to temporarily store tokens that were put back. */
        std::vector<Token*> temp;

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
