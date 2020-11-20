/* ADLTOKENIZER.cpp
 *   by Anonymous
 *
 * Created:
 *   05/11/2020, 16:17:44
 * Last edited:
 *   11/20/2020, 2:18:33 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Class that is used to Tokenize a given ADL file. Is used to read one
 *   token at a time from the stream, with the option to put tokens back.
**/

#include <cerrno>
#include <sstream>

#include "ADLTokenizer.hpp"

using namespace std;
using namespace ArgumentParser;


/***** HELPER FUNCTIONS *****/

/* Determines if given token is a whitespace character or not. */
#define is_whitespace(C) \
    ((C) == ' ' || (C) == '\t' || (C) == '\n')

/* Shortcut for fetching the head character of the internal stream. */
#define PEEK(C) \
    (C) = fgetc(this->file); \
    if ((C) == EOF && ferror(this->file)) { throw Exceptions::FileReadError(this->filenames, errno); }

/* Shortcut for accepting a token and storing it. */
#define STORE(C) \
    result.value.push_back((C)); \
    ++this->col;

/* Shortcut for accepting but not storing a token. */
#define ACCEPT(C) \
    if ((C) != EOF) { ++this->col; }

/* Rejects a token from the stream, by putting it back. */
#define REJECT(C) \
    ungetc((C), this->file);





/***** TOKEN STRUCT *****/

/* Allows a token to be written to an outstream. */
std::ostream& ArgumentParser::operator<<(std::ostream& os, const Token& token) {
    os << tokentype_names[(int) token.type];
    if (!token.value.empty()) {
        os << "(" << token.value << ")";
    }
    return os;
}





/***** TOKENIZER CLASS *****/

/* Constructor for the Tokenizer class, which takes the path to the file we should read. */
Tokenizer::Tokenizer(const std::vector<std::string>& filenames) :
    filenames(filenames),
    line(1),
    col(1),
    last_newline(0),
    path(filenames[filenames.size() - 1])
{
    // Try to open the file
    this->file = fopen(this->path.c_str(), "r");
    if (this->file == NULL) {
        throw Exceptions::FileOpenError(this->filenames, errno);   
    }

    // Reserve space for at least one token on the stream
    this->temp.reserve(1);
}


/* Move constructor for the Tokenizer class. */
Tokenizer::Tokenizer(Tokenizer&& other) :
    filenames(other.filenames),
    file(other.file),
    line(other.line),
    col(other.col),
    last_newline(other.last_newline),
    temp(other.temp),
    path(other.path)
{
    // Set the other's file pointer to NULL
    other.file = NULL;
}

/* Destructor for the Tokenizer class. */
Tokenizer::~Tokenizer() {
    // Close the file
    if (this->file != NULL) { fclose(this->file); }
}



/* Used internally to read the first token off the stream. */
Token Tokenizer::read_head() {
    // Check if there are any put-back tokens we wanna see
    if (this->temp.size() > 0) {
        Token result = this->temp[this->temp.size() - 1];
        this->temp.pop_back();
        return result;
    }

    // Otherwise, parse from the stream
    char c;
    Token result;

start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            // Mark the Token as a general identifier and move to parsing the rest of it
            result.type = TokenType::identifier;
            result.line = this->line;
            result.col = this->col;
            STORE(c);
            goto id_start;
        } else if (c == '-') { 
            result.line = this->line;
            result.col = this->col;
            ACCEPT(c);
            goto dash_start;
        } else if (c == '<') {
            result.type = TokenType::type;
            result.line = this->line;
            result.col = this->col;
            ACCEPT(c);
            goto type_start;
        } else if (c == '\"') {
            result.type = TokenType::string;
            result.line = this->line;
            result.col = this->col;
            ACCEPT(c);
            goto string_start;
        } else if (c >= '0' && c <= '9') {
            result.line = this->line;
            result.col = this->col;
            STORE(c);
            goto number_contd;
        } else if (c == '.') {
            result.line = this->line;
            result.col = this->col;
            ACCEPT(c);
            goto dot_start;
        } else if (c == '/') {
            // Possible comment or regex expression
            result.line = this->line;
            result.col = this->col;
            ACCEPT(c);
            goto slash_start;
        } else if (c == '[') {
            // Simply return the appropriate token
            result.type = TokenType::l_square;
            result.line = this->line;
            result.col = this->col;
            ACCEPT(c);
            return result;
        } else if (c == ']') {
            // Simply return the appropriate token
            result.type = TokenType::r_square;
            result.line = this->line;
            result.col = this->col;
            ACCEPT(c);
            return result;
        } else if (c == '{') {
            // Simply return the appropriate token
            result.type = TokenType::l_curly;
            result.line = this->line;
            result.col = this->col;
            ACCEPT(c);
            return result;
        } else if (c == '}') {
            // Simply return the appropriate token
            result.type = TokenType::r_curly;
            result.line = this->line;
            result.col = this->col;
            ACCEPT(c);
            return result;
        } else if (c == '=') {
            // Simply return the appropriate token
            result.type = TokenType::equals;
            result.line = this->line;
            result.col = this->col;
            ACCEPT(c);
            return result;
        } else if (c == ';') {
            // Simply return the appropriate token
            result.type = TokenType::semicolon;
            result.line = this->line;
            result.col = this->col;
            ACCEPT(c);
            return result;
        } else if (c == '\n') {
            // Increment the line and then try again
            ++this->line;
            this->col = 1;
            this->last_newline = ftell(this->file);
            goto start;
        } else if (is_whitespace(c)) {
            // Re-try
            ACCEPT(c);
            goto start;
        } else if (c == EOF) {
            // Return an empty token
            result.type = TokenType::empty;
            result.line = this->line;
            result.col = this->col;
            return result;
        } else {
            throw Exceptions::UnexpectedCharException(this->filenames, this->line, this->col, this->get_line(), c);
        }
    }



id_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (    (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                 c == '_' || c == '-') {
            // Remember the value and try to parse more
            STORE(c);
            goto id_start;
        } else {
            // We parsed what we could, we are done
            REJECT(c);
            return result;
        }
    }



dash_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (    (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                 c == '?') {
            // Done, it was a shortlabel
            result.type = TokenType::shortlabel;
            STORE(c);
            return result;
        } else if (c == '-') {
            // Parse as longlabel or number, we still don't know
            ACCEPT(c);
            goto dash_dash;
        } else if (!is_whitespace(c)) {
            // Let the user know we encountered an illegal character
            throw Exceptions::IllegalShortlabelException(this->filenames, this->line, this->col, this->get_line(), c);
        } else {
            // Let the user know we encountered an empty option
            throw Exceptions::EmptyShortlabelException(this->filenames, this->line, this->col - 1, this->get_line());
        }
    }



dash_dash:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (    (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                 c == '_') {
            // Parse it as a longlabel
            result.type = TokenType::longlabel;
            STORE(c);
            goto dash_dash_longlabel;
        } else if (c == '-') {
            // Parse as a number or float, depending on if we find a dash
            STORE(c);
            goto number_start;
        } else if (!is_whitespace(c)) {
            // Let the user know we encountered an illegal character
            throw Exceptions::IllegalLonglabelException(this->filenames, this->line, this->col, this->get_line(), c);
        } else {
            // Let the user know we encountered an empty option
            throw Exceptions::EmptyLonglabelException(this->filenames, this->line, this->col - 1, this->get_line());
        }
    }



dash_dash_longlabel:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (    (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                 c == '_' || c == '-') {
            // Keep on parsing
            STORE(c);
            goto dash_dash_longlabel;
        } else {
            // We parsed what we could, we are done
            REJECT(c);
            return result;
        }
    }



type_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (    (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                 c == '_' || c == '-') {
            // Remember the value and continue to the rest of the type
            STORE(c);
            goto type_contd;
        } else if (c != '>') {
            // Let the user know we encountered an illegal character
            throw Exceptions::IllegalTypeException(this->filenames, this->line, this->col, this->get_line(), c);
        } else {
            // Let the user know we encountered an empty option
            throw Exceptions::EmptyTypeException(this->filenames, this->line, this->col, this->get_line());
        }
    }



type_contd:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (    (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                 c == '_' || c == '-') {
            // Remember the value and try to parse more
            STORE(c);
            goto type_contd;
        } else if (c == '>') {
            // Type parsing complete
            ACCEPT(c);
            return result;
        } else {
            // Illegal character
            throw Exceptions::IllegalTypeException(this->filenames, this->line, this->col, this->get_line(), c);
        }
    }



string_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '\\') {
            // Escaping a character; move to the escape state
            ACCEPT(c);
            goto string_escape;
        } else if (c == '"') {
            // Done!
            ACCEPT(c);
            return result;
        } else if (c == '\n') {
            // Add the value, plus update the line count
            result.value.push_back(c);
            this->col = 1;
            this->line++;
            this->last_newline = ftell(this->file);
            goto string_start;
        } else {
            // Simply add the value and continue parsing
            STORE(c);
            goto string_start;
        }
    }



string_escape:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '\\') {
            // Simply add a backslash
            STORE('\\');
            goto string_start;
        } else if (c == 'n') {
            // Add a newline character
            STORE('\n');
            goto string_start;
        } else if (c == 'r') {
            // Add a return carriage character
            STORE('\r');
            goto string_start;
        } else if (c == '"') {
            // Add a normal '"'
            STORE('"');
            goto string_start;
        } else {
            // We cannot escape this character
            throw Exceptions::IllegalEscapeException(this->filenames, this->line, this->col, this->get_line(), c);
        }
    }



number_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c >= '0' && c <= '9') {
            // Start to parse as number
            STORE(c);
            goto number_contd;
        } else if (!is_whitespace(c)) {
            // Let the user know we encountered an illegal character
            throw Exceptions::IllegalNegativeException(this->filenames, this->line, this->col, this->get_line(), c);
        } else {
            // Let the user know we encountered an empty option
            throw Exceptions::EmptyNegativeException(this->filenames, this->line, this->col - 1, this->get_line());
        }
    }



number_contd:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c >= '0' && c <= '9') {
            // Keep on parsing as number
            STORE(c);
            goto number_contd;
        } else if (c == '.') {
            // It's decimal, so do that instead
            result.type = TokenType::decimal;
            STORE(c);
            goto decimal_contd;
        } else {
            // Done, return as number
            result.type = TokenType::number;
            REJECT(c);
            return result;
        }
    }



decimal_contd:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c >= '0' && c <= '9') {
            // Keep on parsing as float
            STORE(c);
            goto decimal_contd;
        } else {
            // Done, return
            REJECT(c);
            return result;
        }
    }



dot_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (    (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                 c == '_' || c == '-') {
            // Try to parse as directive
            result.type = TokenType::directive;
            STORE(c);
            goto dot_directive;
        } else if (c == '.') {
            // So far so good, move to next state
            result.type = TokenType::triple_dot;
            ACCEPT(c);
            goto dot_dot;
        } else {
            // Unexpected!
            throw Exceptions::UnexpectedCharException(this->filenames, this->line, this->col, this->get_line(), c);
        }
    }



dot_dot:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '.') {
            // It's confirmed a triple-dot token; done
            return result;
        } else {
            // Unexpected!
            throw Exceptions::UnexpectedCharException(this->filenames, this->line, this->col, this->get_line(), c);
        }
    }



dot_directive:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (    (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                 c == '_' || c == '-') {
            // Keep parsing as directive
            STORE(c);
            goto dot_directive;
        } else {
            // Done
            REJECT(c);
            return result;
        }
    }



slash_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '/') {
            // Single comment
            ACCEPT(c);
            goto singleline_start;
        } else if (c == '*') {
            // Multi-line comment
            ACCEPT(c);
            goto multiline_start;
        } else if (!is_whitespace(c)) {
            // Regex-expression
            result.type = TokenType::regex;
            STORE(c);
            goto regex_start;
        } else {
            // Otherwise, it doesn't make a lot of sense, so error
            throw Exceptions::UnexpectedCharException(this->filenames, this->line, this->col, this->get_line(), c);
        }
    }



singleline_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '\n') {
            // We're done; put it back on the stream and go back to start
            REJECT(c);
            goto start;
        } else {
            // Simply keep discarding all tokens
            ACCEPT(c);
            goto singleline_start;
        }
    }



multiline_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '*') {
            // Might need to stop, let's examine
            ACCEPT(c);
            goto multiline_star;
        } else if (c == '\n') {
            // Skip, but do update the line counters
            this->col = 1;
            this->line++;
            this->last_newline = ftell(this->file);
            goto multiline_start;
        } else {
            // Simply skip this item
            ACCEPT(c);
            goto multiline_start;
        }
    }



multiline_star:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '/') {
            // We're done; continue parsing normally
            ACCEPT(c);
            goto start;
        } else if (c == '*') {
            // Continue parsing these stars
            ACCEPT(c);
            goto multiline_star;
        } else {
            // We see any non-star, so go back to the multiline_start (but do put it back on the stream for newlines)
            REJECT(c);
            goto multiline_start;
        }
    }



regex_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c != '\n') {
            // Keep parsing as regex
            STORE(c);
            goto regex_start;
        } else if (c == '/') {
            // End of regex
            ACCEPT(c);
            return result;
        } else {
            // That's iwwegal
            throw Exceptions::IllegalRegexException(this->filenames, this->line, this->col, this->get_line());
        }
    }
}

/* Reads the entire, given line from the internal file, used for error messaging. */
std::string Tokenizer::get_line() {
    // Store the current file position
    long cursor = ftell(this->file);

    // Restore the file stream indicator to the most recent newline
    fseek(this->file, this->last_newline, SEEK_SET);

    // We are now at the start of the correct line; read until we encounter EOF or newline
    std::stringstream sstr;
    while (true) {
        char c = fgetc(this->file);
        sstr << c;
        if (c == '\n' || (c == EOF && feof(this->file))) {
            // Restore the initial file position and return
            fseek(this->file, cursor, SEEK_SET);
            return sstr.str();
        } else if (c == EOF && ferror(this->file)) {
            // Restore the initial file position and return and empty one
            fseek(this->file, cursor, SEEK_SET);
            return std::string();
        }
    }
}





/* Looks at the top token of the stream without removing it. */
Token Tokenizer::peek() {
    // Read the token at the head of the stream
    Token head = this->read_head();
    // Always put it back
    this->push(head);
    // Then, return
    return head;
}

/* Removes the top token of the stream and returns it. */
Token Tokenizer::pop() {
    // Read the token at the head of the stream
    Token head = this->read_head();
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
