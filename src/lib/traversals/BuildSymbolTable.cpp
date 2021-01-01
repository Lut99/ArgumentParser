/* BUILD SYMBOL TABLE.cpp
 *   by Lut99
 *
 * Created:
 *   01/01/2021, 16:07:21
 * Last edited:
 *   01/01/2021, 16:29:37
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The first traversal of the compiler, which will use the parsed AST to
 *   construct a symbol table. At this stage, the compiler checks for
 *   duplicate identifiers in the correct namespaces.
**/

#include "ADLIdentifier.hpp"
#include "ADLMeta.hpp"
#include "ADLPositional.hpp"
#include "ADLOption.hpp"
#include "ADLTypeDef.hpp"
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
        ADLNode* parent = node->parent->parent;

        // Switch to the correct parent node
        switch(parent->type) {
            case NodeType::meta:
                // Add it the meta table
                ((ADLMeta*) parent)->symbol_table.add(((ADLIdentifier*) ((ADLBranch*) node)->children[0])->identifier, node);
                break;

            case NodeType::positional:
                // Add it the meta table
                ((ADLPositional*) parent)->symbol_table.add(((ADLIdentifier*) ((ADLBranch*) node)->children[0])->identifier, node);
                break;

            case NodeType::option:
                // Add it the meta table
                ((ADLOption*) parent)->symbol_table.add(((ADLIdentifier*) ((ADLBranch*) node)->children[0])->identifier, node);
                break;

            case NodeType::type_def:
                // Add it the meta table
                ((ADLTypeDef*) parent)->symbol_table.add(((ADLIdentifier*) ((ADLBranch*) node)->children[0])->identifier, node);
                break;

            default:
                // Should NEVER occur
                throw std::runtime_error("Parent with illegal type '" + nodetype_name.at(parent->type) + "' occurred while building symbol table");
        }
    } else {
        // Simply add the node, then recurse through it
        symbol_table->add(((ADLIdentifier*) ((ADLBranch*) node)->children[0])->identifier, node);
        node->traverse(
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

    // We're done
    return result;
}
