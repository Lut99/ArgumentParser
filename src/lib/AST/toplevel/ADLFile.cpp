/* ADLFILE.cpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:47:57
 * Last edited:
 *   26/11/2020, 15:37:09
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



/* Allows the ADLFile to be copied polymorphically. */
ADLFile* ADLFile::copy() const { return new ADLFile(*this); }
