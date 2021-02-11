/* BUILD SYMBOL TABLE.cpp
 *   by Lut99
 *
 * Created:
 *   01/01/2021, 16:07:21
 * Last edited:
 *   11/02/2021, 15:16:11
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The first traversal of the compiler, which will use the parsed AST to
 *   construct a symbol table. At this stage, the compiler checks for
 *   duplicate identifiers in the correct namespaces.
**/

#include "ADLIdentifier.hpp"
#include "ADLDefinition.hpp"
#include "ADLOption.hpp"
#include "ADLConfig.hpp"
#include "BuildSymbolTable.hpp"

using namespace std;
using namespace ArgumentParser;


/***** TRAVERSAL FUNCTIONS *****/
/* Main traversal function for the first traversal. */
static ADLNode* traversal_function(const char* trav_id, ADLNode* node, void* vsymbol_table) {
    SymbolTable* symbol_table = (SymbolTable*) vsymbol_table;

    // Do different things depending on if we're definitions or property statements
    if (node->type == NodeType::config) {
        // Get the parent of the node (skip the ADLConfigs list)
        ADLDefinition* parent = (ADLDefinition*) node->parent->parent;
        
        // Add this node to the symbol table
        parent->symbol_table.add(((ADLConfig*) node)->param.c_str(), node);
    } else {
        // Add one entry for each identifier that works
        ADLDefinition* def = (ADLDefinition*) node;
        if (def->identifier != nullptr) { symbol_table->add(def->identifier->identifier, node); }
        if (def->shortlabel != nullptr) { symbol_table->add(def->shortlabel->identifier, node); }
        if (def->longlabel != nullptr) { symbol_table->add(def->longlabel->identifier, node); }
        
        // Continue with the recursion
        node->traverse_recurse(
            trav_id,
            build_symbol_table_types,
            traversal_function,
            vsymbol_table
        );
    }

    // Simply return the current node, since we won't be replacing any
    return node;
}





/***** ENTRY POINT *****/
/* The traversal function for the first traversal, that builds a symbol table from the parsed AST. */
SymbolTable ArgumentParser::traversal_build_symbol_table(ADLTree* tree) {
    // Prepare the any class that we'll pass around
    SymbolTable result;

    // Call our own traversal function
    tree->traverse(
        build_symbol_table_id,
        build_symbol_table_types,
        traversal_function,
        (void*) &result
    );

    // Once done, merge any 'meta' divs
    ADLDefinition* first_meta = nullptr;
    std::vector<size_t> duplicate_metas;
    duplicate_metas.reserve(result.size());
    for (size_t i = 0; i < result.size(); i++) {
        SymbolTableEntry& entry = result[i];
        if (entry.node_type == NodeType::meta) {
            // Set it as the first meta if we didn't set one already
            if (first_meta == nullptr) {
                first_meta = (ADLDefinition*) entry.node;
                continue;
            }

            // Otherwise, merge the symbol table of the new meta with that of the first
            for (const SymbolTableEntry& nested_entry : ((ADLDefinition*) entry.node)->symbol_table) {
                first_meta->symbol_table.add(nested_entry.id, nested_entry.node);
            }
            // Mark this entry for removal
            duplicate_metas.push_back(i);
        }
    }

    // Remove the metas we marked to remove so
    for (size_t i = 0; i < duplicate_metas.size(); i++) {
        result.remove(duplicate_metas[i]);
    }

    // We're done
    return result;
}
