/* TEST FSTREAM.cpp
 *   by Lut99
 *
 * Created:
 *   12/5/2020, 3:24:50 PM
 * Last edited:
 *   12/5/2020, 3:31:48 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file is used to test around playing with the fstream
**/

#include <iostream>
#include <fstream>

using namespace std;


int main() {
    // Open a filestream to one of the test files
    ifstream h("../test_extern.adl");
    while (!h.eof()) {
        char c = h.get();
        if (c == EOF) { cout << "Awesome!" << endl; break; }
        cout << c;
    }
    return 0;
}

