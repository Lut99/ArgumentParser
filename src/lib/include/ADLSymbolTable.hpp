/* ADLSYMBOL TABLE.hpp
 *   by Lut99
 *
 * Created:
 *   10/12/2020, 14:43:50
 * Last edited:
 *   11/02/2021, 15:12:00
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

namespace ArgumentParser {
    /* Struct that represents a single entry in the Symbol Table. */
    struct SymbolTableEntry {
        /* The ID of the entry's node. */
        std::string id;
        /* The 'raw' ID of the entry's node, i.e., on without slashes to throw Positionals and Options in the same namespace. */
        std::string raw_id;
        /* The type of the entry's node. */
        NodeType node_type;
        /* A pointer to the relevant node in the AST. */
        ADLNode* node;  
    };



    /* The SymbolTable class is used to keep track of all definitions and properties, and references thereof, in a parsed file. */
    class SymbolTable {
    private:
        /* Vector which lists all the ids given in this symbol table, in an ordered fashion. */
        std::vector<SymbolTableEntry> entries;

    public:
        /* Default constructor for the SymbolTable class. */
        SymbolTable();

        /* Tries to add a new symbol to the table. Throws a DuplicateSymbolError if it's already defined, unless it's a meta definition. */
        void add(const std::string& id, ADLNode* node);
        /* Removes a symbol with given id from the table. Note that it simply does nothing if the id isn't present. */
        void remove(const std::string& id);
        /* Removes a symbol at the given location from the table. Note that it simply does nothing if the table is not that large. */
        void remove(size_t index);

        /* Returns whether or not a symbol with the given id exists. */
        bool contains(const std::string& id) const;

        /* Returns a muteable reference to the symbol table entry with the given identifier. Might throw errors if not present. */
        SymbolTableEntry& at(const std::string& id);
        /* Returns a constant reference to the symbol table entry with the given identifier. Might throw errors if not present. */
        const SymbolTableEntry& at(const std::string& id) const;
        /* Returns a muteable reference to the index'th symbol table entry. Might throw errors or even segfault if not present. */
        inline SymbolTableEntry& operator[](size_t index) { return this->entries[index]; }
        /* Returns a constant reference to the index'th symbol table entry. Might throw errors or even segfault if not present. */
        inline const SymbolTableEntry& operator[](size_t index) const { return this->entries[index]; }
        /* Returns the number of symbols stored in this symbol table. */
        inline size_t size() const { return this->entries.size(); }

        /* Returns a muteable reference to the start iterator of the internal symbol table map. Note that this may not be ordered. */
        inline std::vector<SymbolTableEntry>::iterator begin() { return this->entries.begin(); }
        /* Returns a constant reference to the start iterator of the internal symbol table map. Note that this may not be ordered. */
        inline std::vector<SymbolTableEntry>::const_iterator begin() const { return this->entries.begin(); }
        /* Returns a muteable reference to the end iterator of the internal symbol table map. Note that this may not be ordered. */
        inline std::vector<SymbolTableEntry>::iterator end() { return this->entries.end(); }
        /* Returns a constant reference to the end iterator of the internal symbol table map. Note that this may not be ordered. */
        inline std::vector<SymbolTableEntry>::const_iterator end() const { return this->entries.end(); }

    };
}

#endif
