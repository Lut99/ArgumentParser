/* ADLADLPreprocessor.cpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 21:52:46
 * Last edited:
 *   07/12/2020, 21:14:25
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the implementation of the ADLPreprocessor; a
 *   wrapper around the Tokenizer which implements features like includes
 *   and conditional compilation.
**/

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "ADLBaked.hpp"
#include "ADLPreprocessor.hpp"

using namespace std;
using namespace ArgumentParser;

/***** HELPER FUNCTIONS *****/

/* Either peeks or pops from the given Tokenizers, depending on the given boolean value. */
#define PEEK(TOKEN, TOKENIZER, SHOULD_POP) \
    if ((SHOULD_POP)) { (TOKEN) = (TOKENIZER)->pop(); } \
    else { (TOKEN) = (TOKENIZER)->peek(); }





/***** ADLPreprocessor CLASS *****/

/* Constructor for the Preprocessor class, which takes a filename to open and a vector containing all the defines from the CLI. */
Preprocessor::Preprocessor(const std::string& filename, const std::vector<std::string>& defines) :
    length(1),
    max_length(1),
    done_tokenizing(false),
    defines(defines),
    ifdefs(0)
{
    // Create space for at least one tokenizer
    this->tokenizers = new Tokenizer*[this->max_length];

    // Create a new Tokenizer for the first file
    this->tokenizers[0] = new Tokenizer(new ifstream(filename), { filename });
    this->current = this->tokenizers[0];
}

/* Move constructor for the Preprocessor class. */
Preprocessor::Preprocessor(Preprocessor&& other) :
    current(other.current),
    tokenizers(other.tokenizers),
    length(other.length),
    max_length(other.max_length),
    done_tokenizing(other.done_tokenizing),
    included_paths(other.included_paths),
    defines(other.defines),
    ifdefs(other.ifdefs)
{
    // Set the other's pointers list to nullptr, as we don't want him to allocate this
    other.tokenizers = nullptr;
}

/* Destructor for the Preprocessor class. */
Preprocessor::~Preprocessor() {
    // If we're not pointer to a nullptr, loop and delete
    if (this->tokenizers != nullptr) {
        for (size_t i = 0; i < this->length; i++) {
            delete this->tokenizers[i];
        }
        delete[] this->tokenizers;
    }
}



/* Handler for the include-macro. */
Token* Preprocessor::include_handler(bool pop, Token* token) {
    // Get the next token (always pop, since it'll be a relevant value)
    if (!pop) { this->current->pop(); }
    delete token;
    token = this->current->pop();

    // Check if it's either a string (for local file search) or an identifier (for build-ins)
    if (token->type == TokenType::string) {
        #ifdef DEBUG
        cout << "[ADLPreprocessor] Including local file '" << token->raw << "'" << endl;
        #endif

        // Only add a new tokenizer if we never seen it before
        if (!this->contains(this->included_paths, token->raw)) {
            // Try to open a Tokenizer at the given path
            std::vector<std::string> new_filenames = this->current->filenames;
            new_filenames.push_back(token->raw);
            Tokenizer* new_tokenizer = new Tokenizer(new ifstream(token->raw), new_filenames);

            // Set it as the current tokenizer & add to the list
            this->current = new_tokenizer;
            if (this->length >= this->max_length) { this->resize(); }
            this->tokenizers[this->length++] = new_tokenizer;

            // Don't forget to add the new path to the list of included paths
            this->included_paths.push_back(token->raw);
        }
        #ifdef DEBUG
        else {
            cout << "[ADLPreprocessor]  > No need, '" << token->raw << "' already included" << endl;
        }
        #endif

        // Use recursion to return the first token in that tokenizer
        delete token;
        return this->read_head(pop);

    } else if (token->type == TokenType::identifier) {
        #ifdef DEBUG
        cout << "[ADLPreprocessor] Including system file '" << token->raw << "'" << endl;
        #endif

        // Only add a new tokenizer if we never seen it before
        if (!this->contains(this->included_paths, token->raw)) {
            // Check if the given identifier exists
            size_t index = System::n_files;
            std::stringstream sstr;
            for (size_t i = 0; i < System::n_files; i++) {
                if (token->raw == System::names[i]) {
                    index = i;
                }
                if (i > 0) {
                    if (i == System::n_files - 1) { sstr << " or "; }
                    else { sstr << ", "; }
                }
                sstr << '\'' << System::names[i] << '\'';
            }
            if (index == System::n_files) {
                // Not found; throw an error that it was an illegal system file
                Exceptions::log(Exceptions::IllegalSysFileException(token->debug, token->raw, sstr.str()));
                // Use recursion to find the next token instead
                return this->read_head(pop);
            }
            
            // Since it's valid, we add create a new tokenizer with the given string as input
            std::vector<std::string> new_filenames = this->current->filenames;
            new_filenames.push_back(token->raw);
            Tokenizer* new_tokenizer = new Tokenizer(new stringstream(System::files[index]), new_filenames);

            // Set it as the current tokenizer & add to the list
            this->current = new_tokenizer;
            if (this->length >= this->max_length) { this->resize(); }
            this->tokenizers[this->length++] = new_tokenizer;

            // Don't forget to add the new path to the list of included paths
            this->included_paths.push_back(token->raw);
        }
        #ifdef DEBUG
        else {
            cout << "[ADLPreprocessor]  > No need, '" << token->raw << "' already included" << endl;
        }
        #endif

        // Use recursion to return the first token in that tokenizer
        delete token;
        return this->read_head(pop);

    } else {
        Exceptions::log(Exceptions::IllegalMacroValueException(token->debug, "include", tokentype_names[(int) token->type], "string or build-in identifier"));
        // Use recursion to find the next token instead
        return this->read_head(pop);
    }
}

/* Handler for the define-macro. */
Token* Preprocessor::define_handler(bool pop, Token* token) {
    // Check if the next token is an identifier
    if (!pop) { this->current->pop(); }
    delete token;
    token = this->current->pop();
    if (token->type != TokenType::identifier) {
        Exceptions::log(Exceptions::IllegalMacroValueException(token->debug, "define", tokentype_names[(int) token->type], "define identifier"));
        // Use recursion to find the next token instead
        return this->read_head(pop);
    }

    #ifdef DEBUG
    cout << "[ADLPreprocessor] Marking define '" << token->raw << "' as present..." << endl; 
    #endif

    // Check if we already included this one
    if (this->contains(this->defines, token->raw)) {
        Exceptions::log(Exceptions::DuplicateDefineWarning(token->debug, token->raw));
    } else {
        // Add it to the internal list
        this->defines.push_back(token->raw);
    }

    // Use recursion to return the next token
    delete token;
    return this->read_head(pop);
}

/* Handler for the undefine-macro. */
Token* Preprocessor::undefine_handler(bool pop, Token* token) {
    // Check if the next token is an identifier
    if (!pop) { this->current->pop(); }
    delete token;
    token = this->current->pop();
    if (token->type != TokenType::identifier) {
        Exceptions::log(Exceptions::IllegalMacroValueException(token->debug, "undefine", tokentype_names[(int) token->type], "define identifier"));
        // Use recursion to find the next token instead
        return this->read_head(pop);
    }

    #ifdef DEBUG
    cout << "[ADLPreprocessor] Removing define '" << token->raw << "' from list of present defines..." << endl; 
    #endif

    // Check if we can remove this one
    size_t index;
    if (!this->contains(index, this->defines, token->raw)) {
        Exceptions::log(Exceptions::MissingDefineWarning(token->debug, token->raw));
    } else {
        // Remove it from the internal list
        for (size_t i = index; i < this->defines.size() - 1; i++) {
            this->defines[i] = this->defines[i + 1];
        }
        this->defines.pop_back();
    }

    // Use recursion to return the next token
    delete token;
    return this->read_head(pop);
}

/* Handler for the ifdef-macro. */
Token* Preprocessor::ifdef_handler(bool pop, Token* token) {
    // Store the debug information of this macro for error handling
    DebugInfo debug = token->debug;

    // Check if the next token is an identifier
    if (!pop) { this->current->pop(); }
    delete token;
    token = this->current->pop();
    if (token->type != TokenType::identifier) {
        Exceptions::log(Exceptions::IllegalMacroValueException(token->debug, "ifdef", tokentype_names[(int) token->type], "define identifier"));
        // Use recursion to find the next token instead
        return this->read_head(pop);
    }

    // Update the debug info with the identifier
    debug.line2 = token->debug.line2;
    debug.col2 = token->debug.col2;

    #ifdef DEBUG
    cout << "[ADLPreprocessor] Handling ifdef with define '" << token->raw << "'..." << endl; 
    #endif

    // Check if the token is present
    if (this->contains(this->defines, token->raw)) {
        // It is, so mark internally that we are now one deep in an if/else statement
        ++this->ifdefs;

        #ifdef DEBUG
        cout << "[ADLPreprocessor]  > Compiling nested code" << endl; 
        #endif
    } else {
        // It isn't, so skip until we see our endif
        size_t ifdefs = 1;
        while (ifdefs > 0) {
            delete token;
            token = this->current->pop();
            if (token->type == TokenType::macro) {
                if (token->raw == "ifdef") {
                    ++ifdefs;
                } else if (token->raw == "endif") {
                    --ifdefs;
                }
            } else if (token->type == TokenType::empty) {
                // Unclosed if-statement encountered!
                Exceptions::log(Exceptions::UnmatchedIfdefException(debug));
                // Use recursion to find the next token instead
                return this->read_head(pop);
            }
        }

        #ifdef DEBUG
        cout << "[ADLPreprocessor]  > Leaving nested code out" << endl; 
        #endif

        // We're now past the untaken if-block
    }

    // Then, recurse to find the next valid token
    delete token;
    return this->read_head(pop);
}

/* Handler for the ifndef-macro. */
Token* Preprocessor::ifndef_handler(bool pop, Token* token) {
    // Store the debug information of this macro for error handling
    DebugInfo debug = token->debug;

    // Check if the next token is an identifier
    if (!pop) { this->current->pop(); }
    delete token;
    token = this->current->pop();
    if (token->type != TokenType::identifier) {
        Exceptions::log(Exceptions::IllegalMacroValueException(token->debug, "ifndef", tokentype_names[(int) token->type], "define identifier"));
        // Use recursion to find the next token instead
        return this->read_head(pop);
    }

    // Update the debug info with the identifier
    debug.line2 = token->debug.line2;
    debug.col2 = token->debug.col2;

    #ifdef DEBUG
    cout << "[ADLPreprocessor] Handling ifndef with define '" << token->raw << "'..." << endl; 
    #endif

    // Check if the token is present
    if (this->contains(this->defines, token->raw)) {
        // It isn't, so mark internally that we are now one deep in an if/else statement
        ++this->ifdefs;

        #ifdef DEBUG
        cout << "[ADLPreprocessor]  > Leaving nested code out" << endl; 
        #endif
    } else {
        // It is, so skip until we see our endif
        size_t ifdefs = 1;
        while (ifdefs > 0) {
            delete token;
            token = this->current->pop();
            if (token->type == TokenType::macro) {
                if (token->raw == "ifdef") {
                    ++ifdefs;
                } else if (token->raw == "endif") {
                    --ifdefs;
                }
            } else if (token->type == TokenType::empty) {
                // Unclosed if-statement encountered!
                Exceptions::log(Exceptions::UnmatchedIfndefException(debug));
                // Use recursion to find the next token instead
                return this->read_head(pop);
            }
        }

        #ifdef DEBUG
        cout << "[ADLPreprocessor]  > Compiling nested code" << endl; 
        #endif

        // We're now past the untaken if-block
    }

    // Then, recurse to find the next valid token
    delete token;
    return this->read_head(pop);
}

/* Handler for the endif-macro. */
Token* Preprocessor::endif_handler(bool pop, Token* token) {
    // Check if we have unmatched if-statements
    if (this->ifdefs == 0) {
        // We don't; unmatched endif
        Exceptions::log(Exceptions::UnmatchedEndifException(token->debug));
        // Use recursion to find the next token instead
        return this->read_head(pop);
    }

    #ifdef DEBUG
    cout << "[ADLPreprocessor] Found endif for previous ifdef or ifndef" << endl; 
    #endif

    // Otherwise, mark one as closed and use recursion to return the next token
    --this->ifdefs;
    delete token;
    return this->read_head(pop);
}



/* Resizes the internal tokenizers list by doubling its size. */
void Preprocessor::resize() {
    // Start by allocating a new array
    size_t new_max_length = this->max_length * 2;
    Tokenizer** new_tokenizers = new Tokenizer*[new_max_length];

    // Copy all the elements over
    for (size_t i = 0; i < this->length; i++) {
        new_tokenizers[i] = this->tokenizers[i];
    }

    // Replace the old tokenizers array with the new one
    delete[] this->tokenizers;
    this->max_length = new_max_length;
    this->tokenizers = new_tokenizers;
} 

/* Used internally to read the first token off the stream. */
Token* Preprocessor::read_head(bool pop) {
    // Get a token from the Tokenizer
    Token* token;
    PEEK(token, this->current, pop)

    // #ifdef DEBUG
    // cout << "[ADLPreprocessor] Read token " << *token << " from file '" << this->current->filenames[this->current->filenames.size() - 1] << "'" << endl;
    // #endif

    // Do clever stuff
    if (token->type == TokenType::macro) {
        // Determine which macro
        if (token->raw == "include") {
            return this->include_handler(pop, token);
        } else if (token->raw == "define" || token->raw == "def") {
            return this->define_handler(pop, token);
        } else if (token->raw == "undefine" || token->raw == "undef") {
            return this->undefine_handler(pop, token);
        } else if (token->raw == "ifdef") {
            return this->ifdef_handler(pop, token);
        } else if (token->raw == "ifndef") {
            return this->ifndef_handler(pop, token);
        } else if (token->raw == "endif") {
            return this->endif_handler(pop, token);
        } else {
            Exceptions::log(Exceptions::UnknownMacroException(token->debug, token->raw));
            // Use recursion to find the next token instead
            return this->read_head(pop);
        }

    } else if (token->type == TokenType::empty) {
        // This Tokenizer is depleted; check if we have more
        if (this->length > 1) {
            #ifdef DEBUG
            cout << "[ADLPreprocessor] Done including file '" << token->raw << "', moving back to '" << this->tokenizers[this->length - 2]->filenames[this->tokenizers[this->length - 2]->filenames.size() - 1] << "'" << endl;
            #endif

            // Destroy the depleted one and move one tokenizer back on the stack
            delete this->tokenizers[--this->length];
            this->current = this->tokenizers[this->length - 1];

            // Use recursion to get the first token on that Tokenizer
            delete token;
            return this->read_head(pop);
        } else {
            // We're really done
            #ifdef DEBUG
            cout << "[ADLPreprocessor] Nothing more to tokenize." << endl;
            #endif
            this->done_tokenizing = true;
        }
    }

    // Return the token
    return token;
}

/* Used to check if a given vector of strings contains the given string. */
bool Preprocessor::contains(const std::vector<std::string>& haystack, const std::string& needle) {
    for (size_t i = 0; i < haystack.size(); i++) {
        if (haystack[i] == needle) { return true; }
    }
    return false;
}

/* Used to check if a given vector of strings contains the given string. Returns the index of the found result as the first argument. */
bool Preprocessor::contains(size_t& index, const std::vector<std::string>& haystack, const std::string& needle) {
    for (size_t i = 0; i < haystack.size(); i++) {
        if (haystack[i] == needle) {
            index = i;
            return true;
        }
    }
    return false;
}



/* Returns the top element on the merged streams of the root file and all included files, but doesn't remove it. */
Token* Preprocessor::peek() {
    // Read the token at the head of the stream
    Token* head = this->read_head(false);
    // Then, return
    return head;
}

/* Removes the top token of the stream and returns it. */
Token* Preprocessor::pop() {
    // Read the token at the head of the stream
    Token* head = this->read_head(true);
    // Then, return
    return head;
}
