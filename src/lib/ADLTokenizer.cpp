/* ADLTOKENIZER.cpp
 *   by Anonymous
 *
 * Created:
 *   05/11/2020, 16:17:44
 * Last edited:
 *   08/11/2020, 18:15:07
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
    C = fgetc(this->file); \
    if (C == EOF && ferror(this->file)) { throw Exceptions::FileReadException(this->path, errno); } \
    ++this->col; \
    if (result_index >= Token::max_size - 1) { throw Exceptions::ValueOverflowException(this->path, this->line, this->col, Token::max_size - 1); } \
    result.value[result_index++] = C;





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
    char c;
    Token result;
    size_t result_index = 0;

start:
    // Get the head character on the stream
    GET_HEAD(c);

    // Choose the correct path forward
    if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') { goto id_start; }
    else if (c == '-') { goto dash_start; }
    else if (c == '<') { goto type_start; }
    else if (c == '\"') { goto string_start; }
    else if (c >= '0' && c <= '9') { goto number_start; }
    else if (c == '[') {
        // Simply return the appropriate token
        result.type = TokenType::l_square;
        result.line = this->line;
        result.col = this->col;
        result.value[0] = '\0';
        return result;
    } else if (c == ']') {
        // Simply return the appropriate token
        result.type = TokenType::r_square;
        result.line = this->line;
        result.col = this->col;
        result.value[0] = '\0';
        return result;
    } else if (c == '{') {
        // Simply return the appropriate token
        result.type = TokenType::l_square;
        result.line = this->line;
        result.col = this->col;
        result.value[0] = '\0';
        return result;
    } else if (c == '}') {
        // Simply return the appropriate token
        result.type = TokenType::r_curly;
        result.line = this->line;
        result.col = this->col;
        result.value[0] = '\0';
        return result;
    } else if (c == ';') {
        // Simply return the appropriate token
        result.type = TokenType::semicolon;
        result.line = this->line;
        result.col = this->col;
        result.value[0] = '\0';
        return result;
    } else if (c == '\n') {
        ++this->line;
        this->col = 0;
    } else if (!is_whitespace(c)) {
        throw Exceptions::IllegalCharException(this->path, this->line, this->col, c);
    }



id_start:
    // Get the head character on the stream
    GET_HEAD(c);

    // Choose the correct path forward
    if (    c >= 'a' && c <= 'z' ||
            c >= 'A' && c <= 'Z' ||
            c >= '0' && c <= '9' ||
            c == '_' || c == '-') {
        goto id_start;
    } else if (c == '\n' || is_whitespace(c)) {
        // Reached the end of the identifier
        result.value[result_index] = '\0';
        return result;
    } else {
        throw Exceptions::IllegalIDException(this->path, this->line, this->col, c);
    }



dash_start:
type_start:
string_start:
number_start:

}



/* Looks at the top token of the stream without removing it. */
Token Tokenizer::peek() const {
    
}

/* Removes the top token of the stream and returns it. */
Token Tokenizer::pop() {
    // Read the head of the token
}

/* Puts a token back on the stream. Note that it will not be parsed again, so retrieving it will be much faster than the first time. */
void Tokenizer::push(const Token& token) {

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
