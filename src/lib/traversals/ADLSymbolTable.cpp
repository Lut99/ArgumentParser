/* ADLSYMBOL TABLE.cpp
 *   by Lut99
 *
 * Created:
 *   10/12/2020, 14:44:57
 * Last edited:
 *   10/12/2020, 17:08:16
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains a class for the "symbol table", which is used to
 *   collect the different definitions and references to them.
**/

#include "ADLPositional.hpp"
#include "ADLOption.hpp"
#include "ADLTypeDef.hpp"
#include "ADLConfigs.hpp"
#include "ADLSymbolTable.hpp"

using namespace std;
using namespace ArgumentParser;


/***** SYMBOLTABLE CLASS *****/

/* Default constructor for the SymbolTable class. */
SymbolTable::SymbolTable() {}



/* Tries to add a new symbol to the table. Throws a DuplicateSymbolError if it's already defined, unless it's a meta definition. */
void SymbolTable::add(const std::string& id, ADLNode* node) {
    // Check if it already exists
    if (!this->contains(id)) {
        // Switch based on the node's type
        switch(node->type) {
            case NodeType::positional:
                Exceptions::log(Exceptions::DuplicatePositionalError(
                    ((ADLPositional*) node)->children[0]->debug,
                    id
                ));
                return;
            case NodeType::option:
            case NodeType::type_def:
                Exceptions::log(Exceptions::DuplicateTypeError(
                    ((ADLTypeDef*) node)->children[0]->debug,
                    id
                ));
                return;
            case NodeType::configs:
                Exceptions::log(Exceptions::DuplicatePropertyError(
                    ((ADLConfigs*) node)->debug,
                    id
                ));
                return;
            default:
                // Just ignore
                return;
        }
    }

    // Otherwise, add it to the correct list
    this->symbol_table.insert(std::make_pair(id, node));
    this->id_list.push_back(id);
}

/* Removes a symbol with given id from the table. Note that it simply does nothing if the id isn't present. */
void SymbolTable::remove(const std::string& id) {
    // Try to find a correct one with the id
    std::unordered_map<std::string, ADLNode*>::const_iterator iter = this->symbol_table.find(id);
    if (iter == this->symbol_table.end()) {
        // Nothing to remove, we're done
        return;
    }

    // Remove the found element from the map
    this->symbol_table.erase(iter);

    // Find it from the vector and remove it there also
    for (std::vector<std::string>::const_iterator i = this->id_list.begin(); i != this->id_list.end(); ++i) {
        if (*i == id) {
            this->id_list.erase(i);
            return;
        }
    }

    // Otherwise, we don't really bat an eye
    return;
}
