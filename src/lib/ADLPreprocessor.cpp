/* ADLPREPROCESSOR.cpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 21:52:46
 * Last edited:
 *   03/12/2020, 23:26:52
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "ADLPreprocessor.hpp"

using namespace std;
using namespace ArgumentParser;


/***** PREPROCESSOR CLASS *****/

/* Constructor for the Preprocessor class, which takes a filename to open and a vector containing all the defines from the CLI. */
Preprocessor::Preprocessor(const std::string& filename, const std::vector<std::string>& defines) :
    length(0),
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
    done_tokenizing(other.done_tokenizing)
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



/* Used internally to read the first token off the stream. */
Token* Preprocessor::read_head(bool pop) {
    // Get a token from the Tokenizer
    Token* token;
    if (pop) { token = this->current->pop(); }
    else { token = this->current->peek(); }

    // Do clever stuff
    if (token->type == TokenType::empty) { this->done_tokenizing = true; }

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
