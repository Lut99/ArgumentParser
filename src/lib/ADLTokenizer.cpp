/* ADLTOKENIZER.cpp
 *   by Anonymous
 *
 * Created:
 *   05/11/2020, 16:17:44
 * Last edited:
 *   05/11/2020, 21:52:47
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



/* Constructor for the Tokenizer class, which takes the path to the file we should read. */
Tokenizer::Tokenizer(const std::string& path) {
    // Try to open the file
    this->file = fopen(path.c_str(), "r");
    if (this->file == NULL) {
        throw Exceptions::IOException(path, errno);   
    }

    // Reserve space for at least one token on the stream
    this->temp.reserve(1);
}

/* Move constructor for the Tokenizer class. */
Tokenizer::Tokenizer(Tokenizer&& other) :
    file(std::move(other.file)),
    temp(std::move(other.temp))
{
    other.file = NULL;
    other.temp = vector<Token*>();
}

/* Destructor for the Tokenizer class. */
Tokenizer::~Tokenizer() {
    // Close the file
    if (this->file != NULL) { fclose(this->file); }

    // Delete any leftover tokens
    for (size_t i = 0; i < this->temp.size(); i++) {
        delete this->temp[i];
    }
}



/* Used internally to read the first token off the stream. */
Token Tokenizer::_read_head() {
    
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

    swap(t1.file, t2.file);
    swap(t1.temp, t2.temp);
}
