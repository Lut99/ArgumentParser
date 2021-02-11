/* CHECK REFERENCES.cpp
 *   by Lut99
 *
 * Created:
 *   11/02/2021, 15:57:48
 * Last edited:
 *   11/02/2021, 17:57:41
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File that implements the traversal that links (and thus verifies)
 *   references made throughout an ADL file.
**/

#include "ADLTypes.hpp"
#include "ADLTypeDef.hpp"
#include "ADLReference.hpp"
#include "ADLDefinition.hpp"
#include "ADLConfig.hpp"
#include "TraversalExceptions.hpp"
#include "CheckReferences.hpp"

using namespace std;
using namespace ArgumentParser;


/***** TRAVERSAL FUNCTIONS *****/
/* Main traversal function for the second traversal. */
static ADLNode* traversal_function(const char*, ADLNode* node, void* vsymbol_table) {
    SymbolTable* symbol_table = (SymbolTable*) vsymbol_table;

    // First, do different things for the two node types
    if (node->type == NodeType::types) {
        // We are looking at a Types-node, so check each of the given types
        ADLTypes* types = (ADLTypes*) node;

        // First, set enough space in the definitions list
        types->definitions.resize(types->size());

        // Loop through all the IDs and try to find them in the symbol table
        for (size_t i = 0; i < types->children.size(); i++) {
            ADLIdentifier* id = types->get_node<ADLIdentifier>(i);
            if (!symbol_table->contains(id->identifier)) {
                // We didn't find it
                Exceptions::log(Exceptions::UnknownTypeError(id->debug, id->identifier));
                continue;
            }

            // Get that entry & link it to this index of the type
            SymbolTableEntry& entry = symbol_table->at(id->identifier);
            types->definitions[i] = (ADLTypeDef*) entry.node;
            entry.references.push_back(node);
        }

    } else {
        // We are looking at a reference-node, so try to match the definition/config pair
        ADLReference* reference = (ADLReference*) node;
        
        // First, try to find the definition
        if (symbol_table->contains(reference->definition->identifier)) {
            // Definition found; let's first get that entry
            SymbolTableEntry& entry = symbol_table->at(reference->definition->identifier);

            // Get the matching definition (with a sanity check)
            if (!dynamic_cast<ADLDefinition*>(entry.node)) { throw runtime_error("ERROR: Encountered non-definition as valid definition entry in the symbol table"); }
            ADLDefinition* definition = (ADLDefinition*) entry.node;

            // Now, try to find the referenced property in this definition's symbol table
            if (definition->symbol_table.contains(reference->property->identifier)) {
                // We found the correct property! Get the entry & link it to the reference
                SymbolTableEntry& pentry = definition->symbol_table.at(reference->property->identifier);
                reference->reference = (ADLConfig*) pentry.node;
                pentry.references.push_back(node);

            } else {
                // No property found; let the user know
                // Exceptions::log(Exceptions::UnknownPropertyError(reference->property->debug, reference->definition->identifier, reference->property->identifier));
            }

        } else {
            // Definition not found; switch on the type of identifier to get the correct error
            switch(reference->definition->type) {
                case IdentifierType::meta:
                    // Exceptions::log(Exceptions::UnknownMetaError(reference->definition->debug, reference->definition->identifier));
                    break;

                case IdentifierType::positional:
                case IdentifierType::shortlabel:
                case IdentifierType::longlabel:
                    // Exceptions::log(Exceptions::UnknownArgumentError(reference->definition->debug, reference->definition->identifier));
                    break;

                case IdentifierType::type:
                    // Exceptions::log(Exceptions::UnknownTypeError(reference->definition->debug, reference->definition->identifier));
                    break;

                default:
                    // Shouldn't happen!
                    throw std::runtime_error("ERROR: Encountered illegal identifier type as reference definition identifier");
            }
        }
    }

    // We won't change nodes, so always return the OG one
    return node;
}





/***** ENTRY POINT *****/
/* The traversal function for the second traversal, that links all (type-)references in the parsed AST. */
void ArgumentParser::traversal_check_references(ADLTree* tree) {
    // First, link what we can
    tree->traverse(
        check_references_id,
        check_references_types,
        traversal_function,
        (void*) &tree->symbol_table
    );

    // Done!
}
