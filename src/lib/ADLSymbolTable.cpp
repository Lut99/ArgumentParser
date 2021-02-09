/* ADLSYMBOL TABLE.cpp
 *   by Lut99
 *
 * Created:
 *   10/12/2020, 14:44:57
 * Last edited:
 *   2/9/2021, 3:43:54 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains a class for the "symbol table", which is used to
 *   collect the different definitions and references to them.
**/

#include "ADLDefinition.hpp"
#include "TraversalExceptions.hpp"
#include "ADLSymbolTable.hpp"

using namespace std;
using namespace ArgumentParser;


/***** HELPER FUNCTIONS *****/
/* Returns a raw version of the given ID, that is suitable for internal use within the SymbolTable. */
static std::string get_raw(const std::string& id) {
    if (id == "meta") {
        return "[meta]";
    } else if (id.size() > 0 && id[0] == '-') {
        if (id.size() > 1 && id[1] == '-') {
            return id.substr(2);
        } else {
            return id.substr(1);
        }
    } else {
        return id;
    }
}





/***** SYMBOLTABLE CLASS *****/

/* Default constructor for the SymbolTable class. */
SymbolTable::SymbolTable() {}



/* Tries to add a new symbol to the table. Throws a DuplicateSymbolError if it's already defined, unless it's a meta definition. */
void SymbolTable::add(const std::string& id, ADLNode* node) {
    // Check if it already exists
    string raw_id = get_raw(id);
    for (std::vector<SymbolTableEntry>::const_iterator iter = this->entries.begin(); iter != this->entries.end(); ++iter) {
        if ((*iter).raw_id == raw_id) {
            // Switch based on the node's type
            switch(node->type) {
                case NodeType::meta:
                    // This is allowed
                    break;

                case NodeType::positional:
                case NodeType::option:
                    // Throw an error that this node has a duplicate ID
                    Exceptions::log(
                        Exceptions::DuplicateArgumentError(
                            ((ADLDefinition*) node)->identifier->debug,
                            id
                        ),
                        Exceptions::DuplicateSymbolNote(((ADLDefinition*) (*iter).node)->identifier->debug)
                    );
                    return;

                case NodeType::type_def:
                    // Throw an error that this node has a duplicate ID
                    Exceptions::log(
                        Exceptions::DuplicateTypeError(
                            ((ADLDefinition*) node)->identifier->debug,
                            id
                        ),
                        Exceptions::DuplicateSymbolNote(((ADLDefinition*) (*iter).node)->identifier->debug)
                    );
                    return;

                case NodeType::config:
                    // Throw an error that this node has a duplicate ID
                    Exceptions::log(
                        Exceptions::DuplicatePropertyError(
                            ((ADLDefinition*) node)->debug,
                            id
                        ),
                        Exceptions::DuplicateSymbolNote(((ADLDefinition*) (*iter).node)->identifier->debug)
                    );
                    return;
                    
                default:
                    // Sanity check failed
                    throw std::runtime_error("Node with unexpected type '" + nodetype_name.at(node->type) + "' given to SymbolTable::add");
            }
            break;
        }
    }

    // Add a new entry to the list with that raw id
    this->entries.push_back(SymbolTableEntry({ id, raw_id, node->type, node }));
}

/* Removes a symbol with given id from the table. Note that it simply does nothing if the id isn't present. */
void SymbolTable::remove(const std::string& id) {
    // Compute the raw id first
    std::string raw_id = get_raw(id);

    // Check to see if this id (as a raw_id) occurs in the table
    for (std::vector<SymbolTableEntry>::const_iterator iter = this->entries.begin(); iter != this->entries.end(); ++iter) {
        if ((*iter).raw_id == raw_id) {
            // We can remove it!
            this->entries.erase(iter);
            return;
        }
    }

    // Otherwise, we did found nothing bu that's okay
    return;
}



/* Returns whether or not a symbol with the given id exists. */
bool SymbolTable::contains(const std::string& id) const {
    // Compute the raw id first
    std::string raw_id = get_raw(id);

    // Loop to search for it
    for (std::vector<SymbolTableEntry>::const_iterator iter = this->entries.begin(); iter != this->entries.end(); ++iter) {
        if ((*iter).raw_id == raw_id) {
            // Found it
            return true;
        }
    }

    // Did not find it
    return false;
}



/* Returns a muteable reference to the symbol table entry with the given identifier. Might throw errors if not present. */
SymbolTableEntry& SymbolTable::at(const std::string& id) {
    // Compute the raw id first
    std::string raw_id = get_raw(id);

    // Loop to search for it
    for (std::vector<SymbolTableEntry>::iterator iter = this->entries.begin(); iter != this->entries.end(); ++iter) {
        if ((*iter).raw_id == raw_id) {
            // Found it
            return *iter;
        }
    }

    // Did not find it
    throw std::runtime_error("Cannot return unknown SymbolTableEntry with id '" + id + "'");
}

/* Returns a constant reference to the symbol table entry with the given identifier. Might throw errors if not present. */
const SymbolTableEntry& SymbolTable::at(const std::string& id) const {
    // Compute the raw id first
    std::string raw_id = get_raw(id);

    // Loop to search for it
    for (std::vector<SymbolTableEntry>::const_iterator iter = this->entries.begin(); iter != this->entries.end(); ++iter) {
        if ((*iter).raw_id == raw_id) {
            // Found it
            return *iter;
        }
    }

    // Did not find it
    throw std::runtime_error("Cannot return unknown SymbolTableEntry with id '" + id + "'");
}
