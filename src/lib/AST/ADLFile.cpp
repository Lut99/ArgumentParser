/* ADLFILE.cpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:47:57
 * Last edited:
 *   26/11/2020, 12:24:22
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

/* Constructor for the ADLFile class, which only takes a trail of filenames where it originated. */
ADLFile::ADLFile(const std::vector<std::string>& filenames) :
    ADLBranch(NodeType::file, filenames, 0, 0, NodeType::directive)
{}



/* Allows the ADLFile to be copied polymorphically. */
ADLFile* ADLFile::copy() const { return new ADLFile(*this); }
