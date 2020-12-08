/* ADLTOKENIZER.cpp
 *   by Lut99
 *
 * Created:
 *   05/11/2020, 16:17:44
 * Last edited:
 *   08/12/2020, 17:45:37
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
#include <fstream>

#include "ADLTokenizer.hpp"

using namespace std;
using namespace ArgumentParser;


/***** HELPER FUNCTIONS *****/

/* Determines if given token is a whitespace character or not. */
#define is_whitespace(C) \
    ((C) == ' ' || (C) == '\t' || (C) == '\r' || (C) == '\n')

/* Macro that scans through the tokenizer's input stream until a whitespace is encountered, then it goes back to start. */
#define RETRY_AT_WHITESPACE() \
    ACCEPT(c); \
    while (true) { \
        char c; \
        PEEK(c); \
        if (is_whitespace(c)) { \
            REJECT(c); \
            goto start; \
        } else { \
            ACCEPT(c); \
        } \
    }

/* Shortcut for fetching the head character of the internal stream. */
#define PEEK(C) \
    (C) = this->file->get(); \
    if ((C) == EOF && this->file->bad()) { throw Exceptions::log(Exceptions::FileReadError(this->filenames, errno)); }

/* Shortcut for accepting a token and storing it. */
#define STORE(C) \
    result->raw.push_back((C)); \
    ++this->col;

/* Shortcut for accepting but not storing a token. */
#define ACCEPT(C) \
    if ((C) != EOF) { ++this->col; }

/* Rejects a token from the stream, by putting it back. */
#define REJECT(C) \
    this->file->putback((C));

/* Parses given Token as a number (i.e., integral value) and returns it as a ValueToken. */
Token* parse_number(Token* token) {
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
                Exceptions::log(Exceptions::OverflowWarning(token->debug));
                warned = true;
            }

            // If it didn't, add it
            result *= 10;
            result += value;
        } else {
            // Check if it won't get out-of-bounds for the negative add
            if (!warned && result < (numeric_limits<long>::min() / 10.0) + value) {
                Exceptions::log(Exceptions::UnderflowWarning(token->debug));
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
Token* parse_decimal(Token* token) {
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
        Exceptions::log(Exceptions::FloatOverflowWarning(token->debug));
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

/* Override for the polymorphic print function to beautify boolean printing. */
template <>
std::ostream& ValueToken<bool>::print(std::ostream& os) const {
    return os << tokentype_names[(int) this->type] << "(" << (this->value ? "true" : "false") << ")";
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


/* Constructor for the Tokenizer class, which takes an input stream and a path of file breadcrumbs telling the Tokenizer from where it's reading. */
Tokenizer::Tokenizer(std::istream* stream, const std::vector<std::string>& filenames) :
    file(stream),
    line(1),
    col(1),
    last_newline(0),
    done_tokenizing(false),
    filenames(filenames),
    path(filenames[filenames.size() - 1])
{
    // Check if opening the file succeeded
    if (dynamic_cast<ifstream*>(stream) && !((ifstream*) stream)->is_open()) {
        throw Exceptions::log(
            Exceptions::FileOpenError(filenames, errno)
        );
    }

    // Reserve space for at least one token on the stream
    this->temp.reserve(1);
}

/* Move constructor for the Tokenizer class. */
Tokenizer::Tokenizer(Tokenizer&& other) :
    file(other.file),
    line(other.line),
    col(other.col),
    last_newline(other.last_newline),
    done_tokenizing(other.done_tokenizing),
    temp(other.temp),
    filenames(other.filenames),
    path(other.path)
{
    // Set the other's file pointer to NULL
    other.file = nullptr;

    // Also clear the other's list of tokens
    other.temp.clear();
}

/* Destructor for the Tokenizer class. */
Tokenizer::~Tokenizer() {
    // Close the file
    if (this->file != nullptr) { delete this->file; }

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

    // Prepare additional debug struct used for multi-comment error handling
    DebugInfo debug = di_empty;
    debug.filenames = this->filenames;

    // Otherwise, parse from the stream
    char c;
    Token* result = new Token();

start:
    {
        // Get the head character on the stream
        PEEK(c);

        // We can already deduce the line at this point, so put it in the result
        result->raw.clear();
        result->debug.filenames = this->filenames;
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
        } else if (c == '(') {
            // Aha! Parse as boolean
            result->type = TokenType::boolean;
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            ACCEPT(c);
            goto bool_start;
        } else if (c == '+') {
            // Probably a C++-snippet
            result->type = TokenType::snippet;
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            ACCEPT(c);
            goto snippet_start;
        } else if (c == '.') {
            // Might be triple dot, could also be a config parameter
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            ACCEPT(c);
            goto dot_start;
        } else if (c == '/') {
            // Comment start; note its location for multi-line debugging
            debug.line1 = this->line;
            debug.col1 = this->col;
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
        } else if (c == '#') {
            // A macro!
            result->type = TokenType::macro;
            result->debug.line1 = this->line;
            result->debug.col1 = this->col;
            ACCEPT(c);
            goto macro_start;
        } else if (c == '\n') {
            // Increment the line and then try again
            ++this->line;
            this->col = 1;
            this->last_newline = this->file->tellg();
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
            // Log the exception, and then move on to a newline
            Exceptions::log(
                Exceptions::UnexpectedCharException(DebugInfo(this->filenames, this->line, this->col, this->get_line()), c)
            );
            RETRY_AT_WHITESPACE();
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
        } else if (c == '.') {
            // It's a reference!
            result->type = TokenType::reference;
            STORE(c);
            goto reference_dot;
        } else {
            // We parsed what we could, we are done
            result->type = TokenType::identifier;
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
            // Done, see if the shortlabel becomes a reference
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            STORE(c);
            goto shortlabel_end;
        } else if (c == '-') {
            // Parse as longlabel or number, we still don't know
            ACCEPT(c);
            goto dash_dash;
        } else if (!is_whitespace(c)) {
            // Let the user know we encountered an illegal character
            Exceptions::log(
                Exceptions::IllegalShortlabelException(DebugInfo(this->filenames, this->line, this->col, this->get_line()), c)
            );
            RETRY_AT_WHITESPACE();
        } else {
            // Let the user know we encountered an empty option
            Exceptions::log(
                Exceptions::EmptyShortlabelException(DebugInfo(DebugInfo(this->filenames, this->line, this->col - 1, this->get_line())))
            );
            ACCEPT(c);
            goto start;
        }
    }



shortlabel_end:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '.') {
            // It's a reference!
            result->type = TokenType::reference;
            result->raw = "-" + result->raw;
            STORE(c);
            goto reference_dot;
        } else {
            // Just a type identifier; we're done
            result->type = TokenType::shortlabel;
            REJECT(c);
            return result;
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
            STORE(c);
            goto dash_dash_longlabel;
        } else if (c == '-') {
            // Parse as a number or float, depending on if we find a dash
            STORE(c);
            goto number_start;
        } else if (!is_whitespace(c)) {
            // Let the user know we encountered an illegal character
            Exceptions::log(
                Exceptions::IllegalLonglabelException(DebugInfo(this->filenames, this->line, this->col, this->get_line()), c)
            );
            RETRY_AT_WHITESPACE();
        } else {
            // Let the user know we encountered an empty option
            Exceptions::log(
                Exceptions::EmptyLonglabelException(DebugInfo(DebugInfo(this->filenames, this->line, this->col - 1, this->get_line())))
            );
            ACCEPT(c);
            goto start;
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
        } else if (c == '.') {
            // It's a reference
            result->type = TokenType::reference;
            result->raw = "--" + result->raw;
            STORE(c);
            goto reference_dot;
        } else {
            // We parsed what we could, we are done
            result->type = TokenType::longlabel;
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
        } else if (is_whitespace(c) || c == EOF) {
            // Unterminated type definition
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            Exceptions::log(
                Exceptions::UnterminatedTypeException(result->debug)
            );
        } else if (c != '>') {
            // Let the user know we encountered an illegal character
            Exceptions::log(
                Exceptions::IllegalTypeException(DebugInfo(this->filenames, this->line, this->col, this->get_line()), c)
            );
            RETRY_AT_WHITESPACE();
        } else {
            // Let the user know we encountered an empty type id
            Exceptions::log(
                Exceptions::EmptyTypeException(DebugInfo(this->filenames, this->line, this->col, this->get_line()))
            );
            ACCEPT(c);
            goto start;
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
            // Type parsing complete; final check to see if this happens to be a reference
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            ACCEPT(c);
            goto type_end;
        } else if (is_whitespace(c) || c == EOF) {
            // Unterminated type definition
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            Exceptions::log(
                Exceptions::UnterminatedTypeException(result->debug)
            );
        } else {
            // Illegal character
            Exceptions::log(
                Exceptions::IllegalTypeException(DebugInfo(this->filenames, this->line, this->col, this->get_line()), c)
            );
            RETRY_AT_WHITESPACE();
        }
    }



type_end:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '.') {
            // It's a reference!
            
            // ...unless it's followed by an _additional_ dot, in which case it's probably a triple dot
            char old_c = c;
            PEEK(c);
            REJECT(c);
            if (c == '.') {
                // Just a type identifier; we're done
                result->type = TokenType::type;
                REJECT(old_c);
                return result;
            }

            // Update the type and the value and parse the property we reference
            result->type = TokenType::reference;
            result->raw = "<" + result->raw + ">";
            STORE(old_c);
            goto reference_dot;
        } else {
            // Just a type identifier; we're done
            result->type = TokenType::type;
            REJECT(c);
            return result;
        }
    }



reference_dot:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (    (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                 c == '_' || c == '-') {
            // Keep on parsing the reference property
            STORE(c);
            goto reference_dot;
        } else {
            // We're done here

            // Check if we parsed anything at all
            if (result->raw[result->raw.size() - 1] == '.') {
                // We didn't
                Exceptions::log(
                    Exceptions::EmptyReferenceException(DebugInfo(this->filenames, this->line, this->col, this->get_line()))
                );
                REJECT(c);
                goto start;
            }

            // Otherwise, store the debug info and return
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            REJECT(c);
            return result;
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
        } else if (c == '\n' || c == EOF) {
            // Unterminated string; do some debug magic and log it
            result->debug.line2 = result->debug.line1;
            result->debug.col2 = result->debug.col1;
            Exceptions::ExceptionHandler& eh = Exceptions::log(
                Exceptions::UnterminatedStringException(DebugInfo(this->filenames, this->line, this->col, this->get_line())),
                Exceptions::ADLNote(result->debug, "String started here.")
            );
            
            // If we didn't see an EOF, continue trying; otherwise just throw and be done with it
            if (c == '\n') {
                ACCEPT(c);
                goto start;
            } else {
                throw eh;
            }
        } else {
            // Don't accept direct newlines
            Exceptions::log(
                Exceptions::IllegalStringException(DebugInfo(this->filenames, this->line, this->col, this->get_line()), c)
            );
            RETRY_AT_WHITESPACE();
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
            Exceptions::log(
                Exceptions::IllegalStringException(DebugInfo(this->filenames, this->line, this->col, this->get_line()), c)
            );
            ACCEPT(c);
            goto start;
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
            Exceptions::log(
                Exceptions::IllegalNegativeException(DebugInfo(this->filenames, this->line, this->col, this->get_line()), c)
            );
            RETRY_AT_WHITESPACE();
        } else {
            // Let the user know we encountered an empty number
            Exceptions::log(
                Exceptions::EmptyNegativeException(DebugInfo(this->filenames, this->line, this->col - 1, this->get_line()))
            );
            ACCEPT(c);
            goto start;
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
            return parse_number(result);
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
            return parse_decimal(result);
        }
    }



bool_start:
    {
        // We use a different method here for brevity and better error handling; we collect the values quickly in a string, and then decide if it was legal or not
        stringstream sstr;
        while (true) {
            PEEK(c);
            if (is_whitespace(c) || c == EOF || c == ';') {
                Exceptions::log(
                    Exceptions::UnterminatedBooleanException(DebugInfo(this->filenames, result->debug.line1, result->debug.col1, this->line, this->col, result->debug.raw_line))
                );
                ACCEPT(c);
                goto start;
            } else if (c == ')') {
                break;
            }
            ACCEPT(c);
            sstr << c;
        }
        result->debug.line2 = this->line;
        result->debug.col2 = this->col;
        
        // Check the string's legality
        std::string val = sstr.str();
        if (val == "true") {
            // Return the boolean 'true' equivalent
            ACCEPT(c);
            return ValueToken<bool>::promote(result, true);
        } else if (val == "false") {
            // Return the boolean 'false' equivalent
            ACCEPT(c);
            return ValueToken<bool>::promote(result, false);
        } else if (val.empty()) {
            Exceptions::log(
                Exceptions::EmptyBooleanException(DebugInfo(this->filenames, this->line, this->col, this->get_line()))
            );
            ACCEPT(c);
            goto start;
        } else {
            Exceptions::log(
                Exceptions::IllegalBooleanException(result->debug, val)
            );
            RETRY_AT_WHITESPACE();
        }
    }



snippet_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '+') {
            // We collected the plus_plus so far; just a curly bracket to go, and we start with the snippet
            ACCEPT(c);
            goto snippet_pp;
        } else {
            // Otherwise, it doesn't make a lot of sense, so error
            Exceptions::log(
                Exceptions::UnexpectedCharException(DebugInfo(this->filenames, this->line, this->col, this->get_line()), c)
            );
            RETRY_AT_WHITESPACE();
        }
    }



snippet_pp:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '{') {
            // Alright! Start parsing it as a snippet!
            ACCEPT(c);
            goto snippet_code;
        } else {
            // Otherwise, it doesn't make a lot of sense, so error
            Exceptions::log(
                Exceptions::UnexpectedCharException(DebugInfo(this->filenames, this->line, this->col, this->get_line()), c)
            );
            RETRY_AT_WHITESPACE();
        }
    }



snippet_code:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '}') {
            // Might be the end of the snippet!
            STORE(c);
            goto snippet_bracket;
        } else if (c == '/') {
            // Go into any of the comment modes, noting the current position
            debug.line1 = this->line;
            debug.col1 = this->col;
            STORE(c);
            goto snippet_comment_start;
        } else if (c == '\n') {
            // Store, but also increment the line counter
            ++this->line;
            this->col = 1;
            this->last_newline = this->file->tellg();
            result->raw.push_back(c);
            goto snippet_code;
        } else {
            // Add to the token's value and keep parsing more snippet
            STORE(c);
            goto snippet_code;
        }
    }



snippet_bracket:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '+') {
            // Continue on the closing path
            STORE(c);
            goto snippet_end;
        } else {
            // It was nothing, keep on parsing it as usual
            REJECT(c);
            goto snippet_code;
        }
    }



snippet_end:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '+') {
            // Done! Remove the last two values as these turned out to be obsolute, then return
            result->raw.pop_back();
            result->raw.pop_back();
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            result->debug.raw_line = this->get_line();
            ACCEPT(c);
            return result;
        } else {
            // It was nothing, keep on parsing it as usual
            REJECT(c);
            goto snippet_code;
        }
    }



snippet_comment_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '/') {
            // Treat it as a single-line comment
            STORE(c);
            goto snippet_singleline_start;
        } else if (c == '*') {
            // Treat it as a multi-line comment, further noting its start
            debug.line2 = this->line;
            debug.col2 = this->col;
            debug.raw_line = this->get_line();
            STORE(c);
            goto snippet_multiline_start;
        } else {
            // It was just a normal slash, keep going
            REJECT(c);
            goto snippet_code;
        }
    }



snippet_singleline_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '\n') {
            // We're done; put it back on the stream and go back to normal snippet parsing
            REJECT(c);
            goto snippet_code;
        } else {
            // Simply keep adding all tokens to the token
            STORE(c);
            goto snippet_singleline_start;
        }
    }



snippet_multiline_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '*') {
            // Might need to stop, let's examine
            STORE(c);
            goto snippet_multiline_star;
        } else if (c == '\n') {
            // Skip, but do update the line counters
            this->col = 1;
            this->line++;
            this->last_newline = this->file->tellg();
            result->raw.push_back(c);
            goto snippet_multiline_start;
        } else if (c == EOF) {
            // Whoa! Reached unterminated comment!
            throw Exceptions::log(
                Exceptions::UnterminatedMultilineException(DebugInfo(this->filenames, this->line, this->col, this->get_line())),
                Exceptions::ADLNote(debug, "C++ multi-line comment started here.")
            );
        } else {
            // Simply skip this item
            STORE(c);
            goto snippet_multiline_start;
        }
    }



snippet_multiline_star:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '/') {
            // We're done; continue parsing the snippet normally
            STORE(c);
            goto snippet_code;
        } else if (c == '*') {
            // Continue parsing these stars
            STORE(c);
            goto snippet_multiline_star;
        } else {
            // We see any non-star, so go back to the multiline_start (but do put it back on the stream for newlines)
            REJECT(c);
            goto snippet_multiline_start;
        }
    }



dot_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '.') {
            // It's a triple dot instead, so see if we can parse as such
            result->type = TokenType::triple_dot;
            result->raw.push_back('.');
            STORE(c);
            goto triple_dot_end;
        } else if ((c >= 'a' && c <= 'z') ||
                   (c >= 'A' && c <= 'Z') ||
                   (c >= '0' && c <= '9') ||
                    c == '_' || c == '-') {
            // It's a configuration parameter!
            result->type = TokenType::config;
            STORE(c);
            goto config_dot;
        } else {
            // Didn't expect that
            Exceptions::log(
                Exceptions::UnexpectedCharException(DebugInfo(this->filenames, this->line, this->col, this->get_line()), c)
            );
            RETRY_AT_WHITESPACE();
        }
    }



triple_dot_end:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (c == '.') {
            // Alright! That's a succesfull triple dot!
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            STORE(c);
            return result;
        } else {
            // Didn't expect that
            Exceptions::log(
                Exceptions::UnexpectedCharException(DebugInfo(this->filenames, this->line, this->col, this->get_line()), c)
            );
            RETRY_AT_WHITESPACE();
        }
    }



config_dot:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (    (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                 c == '_' || c == '-') {
            // It's still parsing as a config
            STORE(c);
            goto config_dot;
        } else {
            // Apparently, this is the end of the config token
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            REJECT(c);
            return result;
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
            // Multi-line comment, further noting the start of it
            debug.line2 = this->line;
            debug.col2 = this->col;
            debug.raw_line = this->get_line();
            ACCEPT(c);
            goto multiline_start;
        } else {
            // Otherwise, it doesn't make a lot of sense, so error
            Exceptions::log(
                Exceptions::UnexpectedCharException(DebugInfo(this->filenames, this->line, this->col, this->get_line()), c)
            );
            RETRY_AT_WHITESPACE();
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
            this->last_newline = this->file->tellg();
            goto multiline_start;
        } else if (c == EOF) {
            // Whoa! Reached unterminated comment!
            throw Exceptions::log(
                Exceptions::UnterminatedMultilineException(DebugInfo(this->filenames, this->line, this->col, this->get_line())),
                Exceptions::ADLNote(debug, "Multi-line comment started here.")
            );
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



macro_start:
    {
        // Get the head character on the stream
        PEEK(c);

        // Choose the correct path forward
        if (    (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                 c == '_' || c == '-') {
            // Keep parsing for this macro
            STORE(c);
            goto macro_start;
        } else if (result->raw.size() == 0) {
            // Empty macro
            Exceptions::log(
                Exceptions::EmptyMacroException(DebugInfo(this->filenames, this->line, this->col, this->get_line()))
            );
            REJECT(c);
            goto start;
        } else {
            // We're done with this token
            result->debug.line2 = this->line;
            result->debug.col2 = this->col;
            REJECT(c);
            return result;
        }
    }

}

/* Reads the entire, given line from the internal file, used for error messaging. */
LineSnippet Tokenizer::get_line() {
    // Store the current file position
    long cursor = this->file->tellg();

    // Restore the file stream indicator to the most recent newline
    this->file->seekg(this->last_newline);

    // We are now at the start of the correct line; read until we encounter EOF or newline
    std::stringstream sstr;
    while (true) {
        char c;
        this->file->get(c);
        sstr << c;
        if (c == '\n' || (c == EOF && this->file->eof())) {
            // Restore the initial file position and return
            this->file->seekg(cursor);
            break;
        } else if (c == EOF && this->file->fail()) {
            // Restore the initial file position and return an empty one
            this->file->seekg(cursor);
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
