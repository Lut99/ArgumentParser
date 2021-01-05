/* ADLDEFINITION.hpp
 *   by Lut99
 *
 * Created:
 *   04/01/2021, 18:09:03
 * Last edited:
 *   05/01/2021, 13:56:39
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Baseclass for all classes that derive from the toplevel. In
 *   particular, allows easier access and organisation of all classes that
 *   can have symbol tables and to their identifiers.
**/

#ifndef ADL_DEFINITION_HPP
#define ADL_DEFINITION_HPP

#include "ADLBranch.hpp"
#include "ADLIdentifier.hpp"
#include "ADLConfigs.hpp"
#include "ADLSymbolTable.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLDefinition class forms a baseclass for all toplevel definitions in the ArgumentParser. */
    class ADLDefinition: public ADLBranch {
    public:
        /* Pointer that points to the identifier of this Definition. */
        ADLIdentifier* identifier;
        /* Pointer that points to the shortlabel of this Definition in case it's an Option. Will be a nullptr otherwise. */
        ADLIdentifier* shortlabel;
        /* Pointer that points to the longlabel of this Definition in case it's an Option. Will be a nullptr otherwise. */
        ADLIdentifier* longlabel;
        /* Pointer that points to the configs of this Definitions. */
        ADLConfigs* configs;
        /* Symbol table of the Definition. */
        SymbolTable symbol_table;


        /* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, an identifier of the definition and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
        ADLDefinition(NodeType type, const DebugInfo& debug, ADLIdentifier* identifier, ADLConfigs* configs = nullptr);
        /* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, a whitelist of NodeTypes to accept as children, an identifier of the definition and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
        ADLDefinition(NodeType type, const DebugInfo& debug, NodeType whitelist, ADLIdentifier* identifier, ADLConfigs* configs = nullptr);
        /* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, the maximum number of children, an identifier of the definition and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
        ADLDefinition(NodeType type, const DebugInfo& debug, size_t max_children, ADLIdentifier* identifier, ADLConfigs* configs = nullptr);
        /* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, the maximum number of children, a whitelist of NodeTypes to accept as children, an identifier of the definition and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
        ADLDefinition(NodeType type, const DebugInfo& debug, size_t max_children, NodeType whitelist, ADLIdentifier* identifier, ADLConfigs* configs = nullptr);
        /* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, an optional shortlabel for the node, and optional longlabel and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
        ADLDefinition(NodeType type, const DebugInfo& debug, ADLIdentifier* shortlabel, ADLIdentifier* longlabel, ADLConfigs* configs = nullptr);
        /* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, a whitelist of NodeTypes to accept as children, an optional shortlabel for the node, and optional longlabel and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
        ADLDefinition(NodeType type, const DebugInfo& debug, NodeType whitelist, ADLIdentifier* shortlabel, ADLIdentifier* longlabel, ADLConfigs* configs = nullptr);
        /* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, the maximum number of children, an optional shortlabel for the node, and optional longlabel and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
        ADLDefinition(NodeType type, const DebugInfo& debug, size_t max_children, ADLIdentifier* shortlabel, ADLIdentifier* longlabel, ADLConfigs* configs = nullptr);
        /* Constructor for the ADLDefinition class, which takes the node type, the debug information for this node, the maximum number of children, a whitelist of NodeTypes to accept as children, an optional shortlabel for the node, and optional longlabel and optionally a list of configs. Derived Option classes use the longlabel if both are given. */
        ADLDefinition(NodeType type, const DebugInfo& debug, size_t max_children, NodeType whitelist, ADLIdentifier* shortlabel, ADLIdentifier* longlabel, ADLConfigs* configs = nullptr);
        /* Copy constructor for the ADLDefinition class. */
        ADLDefinition(const ADLDefinition& other);
        /* Virtual destructor for the ADLDefinition class. */
        virtual ~ADLDefinition() = default;

        /* Adds a node as child of this node. */
        virtual void add_node(ADLNode* node);

        /* Checks whether this node matches with the given identifier. */
        inline bool has_identifier(ADLIdentifier* identifier) const {
            return
                (this->identifier != nullptr && this->identifier->identifier == identifier->identifier) ||
                (this->shortlabel != nullptr && this->shortlabel->identifier == identifier->identifier) ||
                (this->longlabel != nullptr && this->longlabel->identifier == identifier->identifier)
            ;
        }
    };
}

#endif
