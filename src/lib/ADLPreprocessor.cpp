/* ADLPREPROCESSOR.cpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 21:52:46
 * Last edited:
 *   04/12/2020, 18:06:14
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the implementation of the ADLPreprocessor; a
 *   wrapper around the Tokenizer which implements features like includes
 *   and conditional compilation.
**/

#include <iostream>
#include <algorithm>

#include "ADLPreprocessor.hpp"

using namespace std;
using namespace ArgumentParser;

/***** HELPER FUNCTIONS *****/

/* Either peeks or pops from the given Tokenizers, depending on the given boolean value. */
#define PEEK(TOKEN, TOKENIZER, SHOULD_POP) \
    if ((SHOULD_POP)) { (TOKEN) = (TOKENIZER)->pop(); } \
    else { (TOKEN) = (TOKENIZER)->peek(); }





/***** PREPROCESSOR CLASS *****/

/* Constructor for the Preprocessor class, which takes a filename to open and a vector containing all the defines from the CLI. */
Preprocessor::Preprocessor(const std::string& filename, const std::vector<std::string>& defines) :
    length(1),
    max_length(1),
    done_tokenizing(false)
{
    // Create space for at least one tokenizer
    this->tokenizers = new Tokenizer*[this->max_length];

    // Create a new Tokenizer for the first file
    this->tokenizers[0] = new Tokenizer({ filename });
    this->current = this->tokenizers[0];
}

/* Move constructor for the Preprocessor class. */
Preprocessor::Preprocessor(Preprocessor&& other) :
    current(other.current),
    tokenizers(other.tokenizers),
    length(other.length),
    max_length(other.max_length),
    done_tokenizing(other.done_tokenizing),
    included_paths(other.included_paths)
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

    #ifdef DEBUG
    cout << "[PREPROCESSOR] Read token " << *token << " from file '" << this->current->filenames[this->current->filenames.size() - 1] << "'" << endl;
    #endif

    // Do clever stuff
    if (token->type == TokenType::macro) {
        // Determine which macro
        if (token->raw == "include") {
            // Get the next token (always pop, since it'll be a relevant value)
            if (!pop) { this->current->pop(); }
            delete token;
            token = this->current->pop();

            // Check if it's either a string (for local file search) or an identifier (for build-ins)
            if (token->type == TokenType::string) {
                #ifdef DEBUG
                cout << "[PREPROCESSOR] Including local file '" << token->raw << "'" << endl;
                #endif

                // Only add a new tokenizer if we never seen it before
                if (std::find(this->included_paths.begin(), this->included_paths.end(), token->raw) == this->included_paths.end()) {
                    // Try to open a Tokenizer at the given path
                    std::vector<std::string> new_filenames = this->current->filenames;
                    new_filenames.push_back(token->raw);
                    Tokenizer* new_tokenizer = new Tokenizer(new_filenames);

                    // Set it as the current tokenizer & add to the list
                    this->current = new_tokenizer;
                    if (this->length >= this->max_length) { this->resize(); }
                    this->tokenizers[this->length++] = new_tokenizer;
                    cout << "[PREPROCESSER] " << this->length << endl;

                    // Don't forget to add the new path to the list of included paths
                    this->included_paths.push_back(token->raw);
                }
                #ifdef DEBUG
                else {
                    cout << "[PREPROCESSOR]  > No need, '" << token->raw << "' already included" << endl;
                }
                #endif

                // Use recursion to return the first token in that tokenizer
                delete token;
                return this->read_head(pop);

            } else if (token->type == TokenType::identifier) {
                #ifdef DEBUG
                cout << "[PREPROCESSOR] Including system file '" << token->raw << "'" << endl;
                #endif
                
                /* TBD */
            } else {
                throw Exceptions::IllegalMacroValueException(token->debug, "include", tokentype_names[(int) token->type], "string or build-in identifier");
            }

        } else {
            throw Exceptions::UnknownMacroException(token->debug, token->raw);
        }

    } else if (token->type == TokenType::empty) {
        // This Tokenizer is depleted; check if we have more
        if (this->length > 1) {
            #ifdef DEBUG
            cout << "[PREPROCESSOR] Done including file '" << token->raw << "', moving back to '" << this->tokenizers[this->length - 2]->filenames[this->tokenizers[this->length - 2]->filenames.size() - 1] << "'" << endl;
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
            cout << "[PREPROCESSOR] Nothing more to tokenize." << endl;
            #endif
            this->done_tokenizing = true;
        }
    }

    // Return the token
    return token;
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
