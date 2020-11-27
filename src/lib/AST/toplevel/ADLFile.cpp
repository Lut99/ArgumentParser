/* ADLFILE.cpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:47:57
 * Last edited:
 *   27/11/2020, 15:06:17
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include <algorithm>

#include "ADLFile.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLFILE CLASS *****/

/* Constructor for the ADLFile class, which takes a trail of filenames and optionally a toplevel node. */
ADLFile::ADLFile(const std::vector<std::string>& filenames, ADLNode* toplevel) :
    ADLBranch(NodeType::file, filenames, di_empty, nt_toplevel)
{
    // Add the node if it isn't NULL
    if (toplevel != nullptr) { this->add_node(toplevel); }
}



/* Prints all nodes in this file, reflecting the AST structure. */
std::ostream& ADLFile::print(std::ostream& os) const {
    // Print a header of the filename, making sure it has an even size
    std::string header = "=== FILE";
    if (this->filenames[this->filenames.size() - 1].size() % 2 == 1) { header += ' '; }
    header += "(" + this->filenames[this->filenames.size() - 1] + ") ===";
    os << header << endl << endl;

    // Print each toplevel node, separated by newlines
    for (size_t i = 0; i < this->children.size(); i++) {
        if (i > 0) { os << endl; }
        this->children[i]->print(os);
    }

    // Print some form of footer with equal size of the header and we're done
    os << endl;
    for (size_t i = 0; i < (header.size() - 6) / 2; i++) { os << '='; }
    os << " FILE ";
    for (size_t i = 0; i < (header.size() - 6) / 2; i++) { os << '='; }
    return os << endl;
}



/* Allows the ADLFile to be copied polymorphically. */
ADLFile* ADLFile::copy() const { return new ADLFile(*this); }
