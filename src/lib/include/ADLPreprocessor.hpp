/* ADLPREPROCESSOR.hpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 18:19:27
 * Last edited:
 *   03/12/2020, 23:25:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the implementation of the ADLPreprocessor; a
 *   wrapper around the Tokenizer which implements features like includes
 *   and conditional compilation.
**/

#ifndef ADL_PREPROCESSOR_HPP
#define ADL_PREPROCESSOR_HPP

#include <string>
#include <vector>

#include "ADLTokenizer.hpp"
#include "TokenTypes.hpp"

namespace ArgumentParser {
    /* The Preprocessor class is used to wrap around the ADLTokenizer and process things like includes and conditional compilation. */
    class Preprocessor {
    private:
        /* Reference to the current tokenizer. */
        Tokenizer* current;
        /* Place to store all internal Tokenizers, one per open file. */
        Tokenizer** tokenizers;
        /* The number of tokenizers currently open. */
        size_t length;
        /* The maximum number of Tokenizers we have allocated space for. */
        size_t max_length;
        
        /* Keeps track if we're able to still get elements from the stream. */
        bool done_tokenizing;
        
        /* Used internally to get any tokens from the combined input stream. */
        Token* read_head(bool pop);

    public:
        /* Constructor for the Preprocessor class, which takes a filename to open and a vector containing all the defines from the CLI. */
        Preprocessor(const std::string& filename, const std::vector<std::string>& defines);
        /* Copy constructor for the Preprocessor class, which is deleted, since we can't copy Tokenizers either */
        Preprocessor(const Preprocessor& other) = delete;
        /* Move constructor for the Preprocessor class. */
        Preprocessor(Preprocessor&& other);
        /* Destructor for the Preprocessor class. */
        ~Preprocessor();

        /* Returns the top element on the merged streams of the root file and all included files, but doesn't remove it. */
        Token* peek();
        /* Removes the top token of the stream and returns it. */
        Token* pop();
        /* Puts a token back on the stream. Note that it will not be parsed again, so may retrieving it will be much faster than the first time. */
        inline void push(const Token& token) { return this->push((Token*) &token); }
        /* Puts a token back on the stream. Note that it will not be parsed again, so may retrieving it will be much faster than the first time. */
        template <class T>
        inline void push(const ValueToken<T>& token) { return this->push((Token*) &token); }
        /* Puts a token back on the stream. Note that it will not be parsed again, so may retrieving it will be much faster than the first time. */
        inline void push(Token* token) { return this->current->push(token); }

        /* Returns the current trail of breadcrumbs. */
        inline const std::vector<std::string>& breadcrumbs() const { return this->current->filenames; }

        /* Returns true if an end-of-file has been reached. */
        inline bool eof() const { return this->done_tokenizing; }

    };
    
}

#endif
