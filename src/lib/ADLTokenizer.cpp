/* ADLTOKENIZER.cpp
 *   by Anonymous
 *
 * Created:
 *   05/11/2020, 16:17:44
 * Last edited:
 *   10/11/2020, 18:01:15
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Class that is used to Tokenize a given ADL file. Is used to read one
 *   token at a time from the stream, with the option to put tokens back.
**/

#include <cerrno>

#include "ADLTokenizer.hpp"

using namespace std;
using namespace ArgumentParser;


/***** HELPER FUNCTIONS *****/

/* Determines if given token is a whitespace character or not. */
#define is_whitespace(C) \
    ((C) == ' ' || (C) == '\t' || (C) == '\n')

/* Shortcut for fetching the head character of the internal stream. */
#define GET_HEAD(C) \
    (C) = fgetc(this->file); \
    if ((C) == EOF && ferror(this->file)) { throw Exceptions::FileReadException(this->path, errno); } \
    if ((C) != EOF) { ++this->col; }





/***** TOKENIZER CLASS *****/

/* Constructor for the Tokenizer class, which takes the path to the file we should read. */
Tokenizer::Tokenizer(const std::string& path) :
    path(path),
    line(0),
    col(0)
{
    // Try to open the file
    this->file = fopen(path.c_str(), "r");
    if (this->file == NULL) {
        throw Exceptions::FileOpenException(path, errno);   
    }

    // Reserve space for at least one token on the stream
    this->temp.reserve(1);
}

/* Destructor for the Tokenizer class. */
Tokenizer::~Tokenizer() {
    // Close the file
    if (this->file != NULL) { fclose(this->file); }
}



/* Used internally to read the first token off the stream. */
Token Tokenizer::_read_head() {
    // Check if there are any put-back tokens we wanna see
    if (this->temp.size() > 0) {
        Token result = this->temp[this->temp.size() - 1];
        this->temp.pop_back();
        return result;
    }

    // Otherwise, parse from the stream
    char c;
    Token result;
    size_t result_index = 0;

start:
    {
        result.value = "";
        // Get the head character on the stream
        GET_HEAD(c);

        // Choose the correct path forward
        if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') {
            // Mark the Token as a general identifier and move to parsing the rest of it
            result.type = TokenType::identifier;
            result.line = this->line;
            result.col = this->col;
            result.value.push_back(c);
            goto id_start;
        } else if (c == '-') { 
            result.line = this->line;
            result.col = this->col;
            goto dash_start;
        } else if (c == '<') {
            result.line = this->line;
            result.col = this->col;
            goto type_start;
        } else if (c == '\"') {
            result.line = this->line;
            result.col = this->col;
            goto string_start;
        } else if (c >= '0' && c <= '9') {
            result.line = this->line;
            result.col = this->col;
            result.value.push_back(c);
            goto number_start;
        } else if (c == '[') {
            // Simply return the appropriate token
            result.type = TokenType::l_square;
            result.line = this->line;
            result.col = this->col;
            return result;
        } else if (c == ']') {
            // Simply return the appropriate token
            result.type = TokenType::r_square;
            result.line = this->line;
            result.col = this->col;
            return result;
        } else if (c == '{') {
            // Simply return the appropriate token
            result.type = TokenType::l_square;
            result.line = this->line;
            result.col = this->col;
            return result;
        } else if (c == '}') {
            // Simply return the appropriate token
            result.type = TokenType::r_curly;
            result.line = this->line;
            result.col = this->col;
            return result;
        } else if (c == ';') {
            // Simply return the appropriate token
            result.type = TokenType::semicolon;
            result.line = this->line;
            result.col = this->col;
            return result;
        } else if (c == '\n') {
            // Increment the line and then try again
            ++this->line;
            this->col = 0;
            goto start;
        } else if (is_whitespace(c)) {
            // Re-try
            goto start;
        } else if (c == EOF) {
            // Return an empty token
            result.type = TokenType::empty;
            result.line = this->line;
            result.col = this->col;
            return result;
        } else {
            throw Exceptions::UnexpectedCharException(this->path, this->line, this->col, c);
        }
    }



id_start:
    {
        // Get the head character on the stream
        GET_HEAD(c);

        // Choose the correct path forward
        if (    c >= 'a' && c <= 'z' ||
                c >= 'A' && c <= 'Z' ||
                c >= '0' && c <= '9' ||
                c == '_' || c == '-') {
            // Remember the value and try to parse more
            result.value.push_back(c);
            goto id_start;
        } else {
            // We parsed what we could, we are done
            ungetc(c, this->file);
            return result;
        }
    }



dash_start:
    {
        // Get the head character on the stream
        GET_HEAD(c);

        // Choose the correct path forward
        if (    c >= 'a' && c <= 'z' ||
                c >= 'A' && c <= 'Z' ||
                c >= '0' && c <= '9' ||
                c == '?') {
            // Done, it was a shortlabel
            result.type = TokenType::shortlabel;
            result.value.push_back(c);
            return result;
        } else if (c == '-') {
            // Parse as longlabel or number, we still don't know
            goto dash_dash;
        } else if (!is_whitespace(c)) {
            // Let the user know we encountered an illegal character
            throw Exceptions::IllegalShortlabelException(this->path, result.line, result.col, c);
        } else {
            // Let the user know we encountered an empty option
            throw Exceptions::EmptyShortlabelException(this->path, result.line, result.col);
        }
    }



dash_dash:
    {
        // Get the head character on the stream
        GET_HEAD(c);

        // Choose the correct path forward
        if (    c >= 'a' && c <= 'z' ||
                c >= 'A' && c <= 'Z' ||
                c >= '0' && c <= '9' ||
                c == '_' || c == '-') {
            // Parse it as a longlabel
            result.type = TokenType::longlabel;
            result.value.push_back(c);
            goto dash_dash_longlabel;
        } else if (c == '-') {
            // Parse as a number or float, depending on if we find a dash
            result.value.push_back(c);
            goto number_start;
        } else if (!is_whitespace(c)) {
            // Let the user know we encountered an illegal character
            throw Exceptions::IllegalLonglabelException(this->path, result.line, result.col, c);
        } else {
            // Let the user know we encountered an empty option
            throw Exceptions::EmptyLonglabelException(this->path, result.line, result.col);
        }
    }



dash_dash_longlabel:
    {
        // Get the head character on the stream
        GET_HEAD(c);

        // Choose the correct path forward
        if (    c >= 'a' && c <= 'z' ||
                c >= 'A' && c <= 'Z' ||
                c >= '0' && c <= '9' ||
                c == '_' || c == '-') {
            // Keep on parsing
            result.value.push_back(c);
            goto dash_dash_longlabel;
        } else {
            // We parsed what we could, we are done
            ungetc(c, this->file);
            return result;
        }
    }



type_start:
    {
        // Get the head character on the stream
        GET_HEAD(c);

        // Choose the correct path forward
        if (    c >= 'a' && c <= 'z' ||
                c >= 'A' && c <= 'Z' ||
                c >= '0' && c <= '9' ||
                c == '_' || c == '-') {
            // Remember the value and continue to the rest of the type
            result.value.push_back(c);
            goto id_start;
        } else if (!is_whitespace(c) && c != '>') {
            // Let the user know we encountered an illegal character
            throw Exceptions::IllegalTypeException(this->path, result.line, result.col, c);
        } else {
            // Let the user know we encountered an empty option
            throw Exceptions::EmptyTypeException(this->path, result.line, result.col);
        }
    }



type_contd:
    {
        // Get the head character on the stream
        GET_HEAD(c);

        // Choose the correct path forward
        if (    c >= 'a' && c <= 'z' ||
                c >= 'A' && c <= 'Z' ||
                c >= '0' && c <= '9' ||
                c == '_' || c == '-') {
            // Remember the value and try to parse more
            result.value.push_back(c);
            goto id_start;
        } else {
            // We parsed what we could, we are done
            if (c != '>') { ungetc(c, this->file); }
            return result;
        }
    }



string_start:
    {
        // Get the head character on the stream
        GET_HEAD(c);

        // Choose the correct path forward
        if (c == '\\') {
            // Escaping a character; move to the escape state
            goto string_escape;
        } else if (c == '"') {
            // Done!
            return result;
        } else {
            // Simply add the value and continue parsing
            result.value.push_back(c);
            goto string_start;
        }
    }



string_escape:
    {
        // Get the head character on the stream
        GET_HEAD(c);

        // Choose the correct path forward
        if (c == '\\') {
            // Simply add a backslash
            result.value.push_back('\\');
            goto string_start;
        } else if (c == 'n') {
            // Add a newline character
            result.value.push_back('\n');
            goto string_start;
        } else if (c == 'r') {
            // Add a return carriage character
            result.value.push_back('\r');
            goto string_start;
        } else if (c == '"') {
            // Add a normal '"'
            result.value.push_back('"');
            goto string_start;
        } else {
            // We cannot escape this character
            throw Exceptions::IllegalEscapeException(this->path, this->line, this->col, c);
        }
    }



number_start:
    {
        // Get the head character on the stream
        GET_HEAD(c);

        // Choose the correct path forward
        if (c >= '0' && c <= '9') {
            // Start to parse as number
            result.value.push_back(c);
            goto number_contd;
        } else if (!is_whitespace(c)) {
            // Let the user know we encountered an illegal character
            throw Exceptions::IllegalNegativeException(this->path, result.line, result.col, c);
        } else {
            // Let the user know we encountered an empty option
            throw Exceptions::EmptyNegativeException(this->path, result.line, result.col);
        }
    }



number_contd:
    {
        // Get the head character on the stream
        GET_HEAD(c);

        // Choose the correct path forward
        if (c >= '0' && c <= '9') {
            // Keep on parsing as number
            result.value.push_back(c);
            goto number_contd;
        } else if (c == '.') {
            // It's decimal, so do that instead
            result.type = TokenType::decimal;
            goto decimal_contd;
        } else {
            // Done, return as number
            result.type = TokenType::number;
            ungetc(c, this->file);
            return result;
        }
    }



decimal_contd:
    {
        // Get the head character on the stream
        GET_HEAD(c);

        // Choose the correct path forward
        if (c >= '0' && c <= '9') {
            // Keep on parsing as float
            result.value.push_back(c);
            goto decimal_contd;
        } else {
            // Done, return
            ungetc(c, this->file);
            return result;
        }
    }
}





/* Looks at the top token of the stream without removing it. */
Token Tokenizer::peek() {
    // Read the token at the head of the stream
    Token head = this->_read_head();
    // Always put it back
    this->push(head);
    // Then, return
    return head;
}

/* Removes the top token of the stream and returns it. */
Token Tokenizer::pop() {
    // Read the token at the head of the stream
    Token head = this->_read_head();
    // Then, return
    return head;
}

/* Puts a token back on the stream. Note that it will not be parsed again, so retrieving it will be much faster than the first time. */
void Tokenizer::push(const Token& token) {
    // Add the token to our temporary list of tokens, doubling capacity each time we go over it
    if (this->temp.size() == this->temp.capacity()) {
        this->temp.reserve(this->temp.capacity() * 2);
    }

    // Add it to the back of the temporary list
    this->temp.push_back(token);
}



/* Move assignment operator for the Tokenizer class. */
Tokenizer& Tokenizer::operator=(Tokenizer&& other) {
    if (this != &other) { swap(*this, other); }
    return *this;
}

/* Swap operator for the Tokenizer class. */
void ArgumentParser::swap(Tokenizer& t1, Tokenizer& t2) {
    using std::swap;

    swap(t1.path, t2.path);
    swap(t1.file, t2.file);
    swap(t1.line, t2.line);
    swap(t1.col, t2.col);
    swap(t1.temp, t2.temp);
}
