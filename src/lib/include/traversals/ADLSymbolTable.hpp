/* ADLSYMBOL TABLE.hpp
 *   by Lut99
 *
 * Created:
 *   10/12/2020, 14:43:50
 * Last edited:
 *   10/12/2020, 17:07:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains a class for the "symbol table", which is used to
 *   collect the different definitions and references to them.
**/

#ifndef ADLSYMBOL_TABLE_HPP
#define ADLSYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "ADLNode.hpp"
#include "TraversalExceptions.hpp"

namespace ArgumentParser {
    /* The SymbolTable class is used to keep track of all definitions and properties, and references thereof, in a parsed file. */
    class SymbolTable {
    private:
        /* Map which maps a symbol (i.e., a definition or one of its defined properties) to the relevant pointer in the AST. */
        std::unordered_map<std::string, ADLNode*> symbol_table;
        /* Vector which lists all the ids given in this symbol table. */
        std::vector<std::string> id_list;

    public:
        /* Default constructor for the SymbolTable class. */
        SymbolTable();

        /* Tries to add a new symbol to the table. Throws a DuplicateSymbolError if it's already defined, unless it's a meta definition. */
        void add(const std::string& id, ADLNode* node);
        /* Removes a symbol with given id from the table. Note that it simply does nothing if the id isn't present. */
        void remove(const std::string& id);

        /* Returns whether or not a symbol with the given id exists. */
        inline bool contains(const std::string& id) const { return this->symbol_table.find(id) != this->symbol_table.end(); }
        /* Returns the pointer belonging to the given symbol. Might throw errors if not present. */
        inline ADLNode* operator[](const std::string& id) const { return this->symbol_table.at(id); }
        /* Returns the i'th symbol stored in this SymbolTable. Might throw errors if not present or even a segmentation fault. */
        inline ADLNode* operator[](size_t index) const { return this->symbol_table.at(this->id_list[index]); }
        /* Returns a vector containing all the ids stored in this symbol table (ordered). */
        inline std::vector<std::string> ids() const { return this->id_list; }
        /* Returns the number of symbols stored in this symbol table. */
        inline size_t size() const { return this->symbol_table.size(); }

        /* Returns a reference to the start iterator of the internal symbol table map. Note that this may not be ordered. */
        inline std::unordered_map<std::string, ADLNode*>::const_iterator begin() const { return this->symbol_table.begin(); }
        /* Returns a reference to the end iterator of the internal symbol table map. Note that this may not be ordered. */
        inline std::unordered_map<std::string, ADLNode*>::const_iterator end() const { return this->symbol_table.end(); }

    };
}

#endif
