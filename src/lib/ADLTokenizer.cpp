/* ADLTOKENIZER.cpp
 *   by Anonymous
 *
 * Created:
 *   05/11/2020, 16:17:44
 * Last edited:
 *   27/11/2020, 18:05:57
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Class that is used to Tokenize a given ADL file. Is used to read one
 *   token at a time from the stream, with the option to put tokens back.
**/

#include <cerrno>
#include <cstdint>
#include <iostream>
#include <sstream>

#include "ADLTokenizer.hpp"

using namespace std;
using namespace ArgumentParser;


/***** HELPER FUNCTIONS *****/

/* Determines if given token is a whitespace character or not. */
#define is_whitespace(C) \
    ((C) == ' ' || (C) == '\t' || (C) == '\r' || (C) == '\n')

/* Shortcut for fetching the head character of the internal stream. */
#define PEEK(C) \
    (C) = fgetc(this->file); \
    if ((C) == EOF && ferror(this->file)) { throw Exceptions::FileReadError(this->filenames, errno); }

/* Shortcut for accepting a token and storing it. */
#define STORE(C) \
    result->raw.push_back((C)); \
    ++this->col;

/* Shortcut for accepting but not storing a token. */
#define ACCEPT(C) \
    if ((C) != EOF) { ++this->col; }

/* Rejects a token from the stream, by putting it back. */
#define REJECT(C) \
    ungetc((C), this->file);

/* Parses given Token as a number (i.e., integral value) and returns it as a ValueToken. */
Token* parse_number(const std::vector<std::string>& filenames, Token* token) {
    // Init some variables
    std::string raw = token->raw;
    long result = 0;
    long modifier = 1;

    // If the number is negative, reflect that in the modifier and get rid of the raw minus
    if (raw.substr(0, 3) == "---") {
        raw = raw.substr(3);
        modifier *= -1;
    }

    // Loop and add the numbers
    bool warned = false;
    for (size_t i = 0; i < raw.size(); i++) {
        long value = (long) (raw[i] - '0');
        if (modifier > 0) {
            // Check if it won't get out-of-bounds for the positive add
            if (!warned && result > (numeric_limits<long>::max() / 10.0) - value) {
                Exceptions::print_warning(cerr, Exceptions::OverflowWarning(filenames, token->debug));
                warned = true;
            }

            // If it didn't, add it
            result *= 10;
            result += value;
        } else {
            // Check if it won't get out-of-bounds for the negative add
            if (!warned && result < (numeric_limits<long>::min() / 10.0) + value) {
                Exceptions::print_warning(cerr, Exceptions::UnderflowWarning(filenames, token->debug));
                warned = true;
            }

            // If it didn't, add it
            result *= 10;
            result -= value;
        }
    }

    // Done, so create a ValueToken and return it
    return (Token*) ValueToken<long>::promote(token, result);
}

/* Parses given Token as a decimal (i.e., floating-point value) and returns it as a ValueToken. */
Token* parse_decimal(const std::vector<std::string>& filenames, Token* token) {
    // Init some variables
    std::string raw = token->raw;
    double result = 0;
    double modifier = 1;

    // If the number is negative, reflect that in the modifier and get rid of the raw minus
    if (raw.substr(0, 3) == "---") {
        raw = raw.substr(3);
        modifier *= -1;
    }

    // Since parsing doubles is hard, let's just call a library function
    try {
        result = modifier * std::stod(raw);
    } catch (std::out_of_range& e) {
        // Print as the overflow warning
        Exceptions::print_warning(cerr, Exceptions::FloatOverflowWarning(filenames, token->debug));
        result = std::numeric_limits<double>::max();
    }

    // Done, so create a ValueToken and return it
    return (Token*) ValueToken<double>::promote(token, result);
}





/***** TOKEN STRUCT *****/

/* Internal, polymorphic print function. */
std::ostream& Token::print(std::ostream& os) const {
    os << tokentype_names[(int) this->type];
    if (!this->raw.empty()) {
        os << "(" << this->raw << ")";
    }
    return os;
}



/* Allows the Token to be copied polymorphically. */
Token* Token::copy() const { return new Token(*this); }





/***** VALUETOKEN STRUCT *****/

/* Internal, polymorphic print function. */
template <class T>
std::ostream& ValueToken<T>::print(std::ostream& os) const {
    return os << tokentype_names[(int) this->type] << "(" << this->value << ")";
}

/* "Promotes" an existing Token to a ValueToken, by copying it and adding our value. Note that the given token will be deallocated. */
template <class T>
ValueToken<T>* ValueToken<T>::promote(Token* other, const T& value) {
    // Create a new ValueToken
    ValueToken* result = new ValueToken<T>();

    // Copy the token's properties
    result->type = other->type;
    result->debug = other->debug;
    result->raw = other->raw;

    // Inject the value
    result->value = value;

    // Delete the old token
    delete other;

    // Return the new one
    return result;
}



/* Allows the ValueToken to be copied polymorphically. */
template <class T>
ValueToken<T>* ValueToken<T>::copy() const { return new ValueToken<T>(*this); }





/***** TOKENIZER CLASS *****/

/* Constructor for the Tokenizer class, which takes the path to the file we should read. */
Tokenizer::Tokenizer(const std::vector<std::string>& filenames) :
    filenames(filenames),
    line(1),
    col(1),
    last_newline(0),
    done_tokenizing(false),
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
    done_tokenizing(other.done_tokenizing),
    temp(other.temp),
    path(other.path)
{
    // Set the other's file pointer to NULL
    other.file = NULL;

    // Also clear the other's list of tokens
    other.temp.clear();
}

/* Destructor for the Tokenizer class. */
Tokenizer::~Tokenizer() {
    // Close the file
    if (this->file != NULL) { fclose(this->file); }

    // Deallocate the temporary tokens
    for (size_t i = 0; i < this->temp.size(); i++) {
        delete this->temp[i];
    }
}



/* Used internally to read the first token off the stream. */
Token* Tokenizer::read_head() {
    // Check if there are any put-back tokens we wanna see
    if (this->temp.size() > 0) {
        Token* result = this->temp[this->temp.size() - 1];
        this->temp.pop_back();
        return result;
    }

    // Otherwise, parse from the stream
    char c;
    Token* result = new Token();

start:
    {
        // Get the head character on the stream
        PEEK(c);

        // We can already deduce the line at this point, so put it in the result
        result->debug.raw_line = this->get_line();

        // Choose the correct path forward
        if (c == 'r') {
            // See if this turns into a regex-expression or an identifier
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            STORE(c);
            goto r_start;
        } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            // Mark the Token as a general identifier and move to parsing the rest of it
            result->type = TokenType::identifier;
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            STORE(c);
            goto id_start;
        } else if (c == '-') { 
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            ACCEPT(c);
            goto dash_start;
        } else if (c == '<') {
            result->type = TokenType::type;
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            ACCEPT(c);
            goto type_start;
        } else if (c == '\"') {
            result->type = TokenType::string;
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            ACCEPT(c);
            goto string_start;
        } else if (c >= '0' && c <= '9') {
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            STORE(c);
            goto number_contd;
        } else if (c == '.') {
            result->type = TokenType::triple_dot;
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            ACCEPT(c);
            goto dot_start;
        } else if (c == '/') {
            // Comment
            ACCEPT(c);
            goto comment_start;
        } else if (c == '[') {
            // Simply return the appropriate token
            result->type = TokenType::l_square;
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            STORE(c);
            return result;
        } else if (c == ']') {
            // Simply return the appropriate token
            result->type = TokenType::r_square;
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            STORE(c);
            return result;
        } else if (c == '{') {
            // Simply return the appropriate token
            result->type = TokenType::l_curly;
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            STORE(c);
            return result;
        } else if (c == '}') {
            // Simply return the appropriate token
            result->type = TokenType::r_curly;
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            STORE(c);
            return result;
        } else if (c == ';') {
            // Simply return the appropriate token
            result->type = TokenType::semicolon;
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            STORE(c);
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
            result->type = TokenType::empty;
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            this->done_tokenizing = true;
            return result;
        } else {
            throw Exceptions::UnexpectedCharException(this->filenames, DebugInfo(this->line, this->col, this->get_line()), c);
        }
    }



r_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '\"') {
            // Parse as regex

            // First, clear the first 'r'
            result->raw = "";

            // Then, parse as string (but with a different type)
            result->type = TokenType::regex;
            ACCEPT(c);
            goto string_start;
        } else if ((c >= 'a' && c <= 'z') ||
                   (c >= 'A' && c <= 'Z') ||
                   (c >= '0' && c <= '9') ||
                    c == '_' || c == '-') {
            // Start parsing the value as id
            result->type = TokenType::identifier;
            STORE(c);
            goto id_start;
        } else {
            // We parsed what we could, 'pparently this is a one-char identifier
            result->type = TokenType::identifier;
            result->debug.line2 = this->line;
            result->debug.col2 = this->col - 1;
            REJECT(c);
            return result;
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
            result->debug.line2 = this->line;
            result->debug.col2 = this->col - 1;
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
            result->type = TokenType::shortlabel;
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            STORE(c);
            return result;
        } else if (c == '-') {
            // Parse as longlabel or number, we still don't know
            ACCEPT(c);
            goto dash_dash;
        } else if (!is_whitespace(c)) {
            // Let the user know we encountered an illegal character
            throw Exceptions::IllegalShortlabelException(this->filenames, DebugInfo(this->line, this->col, this->get_line()), c);
        } else {
            // Let the user know we encountered an empty option
            throw Exceptions::EmptyShortlabelException(this->filenames, DebugInfo(DebugInfo(this->line, this->col - 1, this->get_line())));
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
            result->type = TokenType::longlabel;
            STORE(c);
            goto dash_dash_longlabel;
        } else if (c == '-') {
            // Parse as a number or float, depending on if we find a dash
            STORE(c);
            goto number_start;
        } else if (!is_whitespace(c)) {
            // Let the user know we encountered an illegal character
            throw Exceptions::IllegalLonglabelException(this->filenames, DebugInfo(this->line, this->col, this->get_line()), c);
        } else {
            // Let the user know we encountered an empty option
            throw Exceptions::EmptyLonglabelException(this->filenames, DebugInfo(DebugInfo(this->line, this->col - 1, this->get_line())));
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
            result->debug.line2 = this->line;
            result->debug.col2 = this->col - 1;
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
            throw Exceptions::IllegalTypeException(this->filenames, DebugInfo(this->line, this->col, this->get_line()), c);
        } else {
            // Let the user know we encountered an empty option
            throw Exceptions::EmptyTypeException(this->filenames, DebugInfo(this->line, this->col, this->get_line()));
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
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            ACCEPT(c);
            return result;
        } else {
            // Illegal character
            throw Exceptions::IllegalTypeException(this->filenames, DebugInfo(this->line, this->col, this->get_line()), c);
        }
    }



string_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '\\') {
            // Escaping a character; move to the escape state
            STORE(c);
            goto string_escape;
        } else if (c == '"') {
            // Done!
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            ACCEPT(c);
            return result;
        } else if (c >= ' ' && c <= '~') {
            // Simply add the value and continue parsing
            STORE(c);
            goto string_start;
        } else {
            // Don't accept direct newlines
            throw Exceptions::IllegalStringException(this->filenames, DebugInfo(this->line, this->col, this->get_line()), c);
        }
    }



string_escape:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c >= ' ' && c <= '~') {
            // Simply add the raw character, without the backslash
            STORE(c);
            goto string_start;
        } else {
            // Non-readable character is escaped!
            throw Exceptions::IllegalStringException(this->filenames, DebugInfo(this->line, this->col, this->get_line()), c);
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
            throw Exceptions::IllegalNegativeException(this->filenames, DebugInfo(this->line, this->col, this->get_line()), c);
        } else {
            // Let the user know we encountered an empty option
            throw Exceptions::EmptyNegativeException(this->filenames, DebugInfo(this->line, this->col - 1, this->get_line()));
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
            result->type = TokenType::decimal;
            STORE(c);
            goto decimal_contd;
        } else {
            // Done, it's a normal number
            result->type = TokenType::number;
            result->debug.line2 = this->line;
            result->debug.col2 = this->col - 1;
            REJECT(c);

            // Return with the parsed value
            return parse_number(this->filenames, result);
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
            // Done, return with the parsed value
            result->debug.line2 = this->line;
            result->debug.col2 = this->col - 1;
            REJECT(c);
            return parse_decimal(this->filenames, result);
        }
    }



dot_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '.') {
            // So far so good, move to next state
            ACCEPT(c);
            goto dot_dot;
        } else {
            // Unexpected!
            throw Exceptions::UnexpectedCharException(this->filenames, DebugInfo(this->line, this->col, this->get_line()), c);
        }
    }



dot_dot:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '.') {
            // It's confirmed a triple-dot token; done
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            return result;
        } else {
            // Unexpected!
            throw Exceptions::UnexpectedCharException(this->filenames, DebugInfo(this->line, this->col, this->get_line()), c);
        }
    }



comment_start:
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
        } else {
            // Otherwise, it doesn't make a lot of sense, so error
            throw Exceptions::UnexpectedCharException(this->filenames, DebugInfo(this->line, this->col, this->get_line()), c);
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

}

/* Reads the entire, given line from the internal file, used for error messaging. */
LineSnippet Tokenizer::get_line() {
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
            break;
        } else if (c == EOF && ferror(this->file)) {
            // Restore the initial file position and return an empty one
            fseek(this->file, cursor, SEEK_SET);
            return std::string();
        }
    }

    // Reduce the size of the string to 50 if needed, by laternatingly take of one at the end and the start
    std::string result = sstr.str();
    size_t c1 = 0;
    size_t c2 = result.size() - 1;
    size_t oversize = result.size() > 50 ? result.size() - 50 : 0;
    for (size_t i = 0; i < oversize; i++) {
        if (i % 2 == 0) {
            result.pop_back();
            --c2;
        } else {
            result = std::string(result.c_str() + 1);
            --c1;
        }
    }

    // Done, return
    return LineSnippet(c1, c2, result);
}



/* Looks at the top token of the stream without removing it. */
Token* Tokenizer::peek() {
    // Read the token at the head of the stream
    Token* head = this->read_head();
    // Always put it back
    this->push(head);
    // Then, return
    return head;
}

/* Removes the top token of the stream and returns it. */
Token* Tokenizer::pop() {
    // Read the token at the head of the stream
    Token* head = this->read_head();
    // Then, return
    return head;
}

/* Puts a token back on the stream. Note that it will not be parsed again, so retrieving it will be much faster than the first time. */
void Tokenizer::push(Token* token) {
    // Add the token to our temporary list of tokens, doubling capacity each time we go over it
    if (this->temp.size() == this->temp.capacity()) {
        this->temp.reserve(this->temp.capacity() * 2);
    }

    // Add it to the back of the temporary list
    this->temp.push_back(token);
}
