/* ADLFILE.cpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:47:57
 * Last edited:
 *   18/11/2020, 20:48:17
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "ADLFile.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLFILE CLASS *****/

/* Allows the ADLFile to be copied polymorphically. */
ADLFile* ADLFile::copy() const { return new ADLFile(*this); }
