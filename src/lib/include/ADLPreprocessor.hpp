/* ADLPREPROCESSOR.hpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 18:19:27
 * Last edited:
 *   12/9/2020, 4:48:28 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the implementation of the ADLPreprocessor; a
 *   wrapper around the Tokenizer which implements features like includes
 *   and conditional compilation.
**/

#ifndef ADL_PREPROCESSOR_HPP
#define ADL_PREPROCESSOR_HPP

#include <string>
#include <vector>

#include "ADLTokenizer.hpp"
#include "TokenTypes.hpp"
#include "ADLExceptions.hpp"

namespace ArgumentParser {
    namespace Exceptions {
        /* Baseclass exception for all Preprocessor-related exceptions. */
        class PreprocessorException: public ADLCompileError {
        public:
            /* Constructor for the PreprocessorException class, which takes a DebugInfo struct to locate this error in a source file and optionally a message. */
            PreprocessorException(const DebugInfo& debug, const std::string& message = "") :
                ADLCompileError(debug, message)
            {}

        };

        /* Exception for when a macro-token is received, but we haven't implemented that particular token. */
        class UnknownMacroException: public PreprocessorException {
        public:
            /* The name of the unknown macro that we got. */
            std::string macro;

            /* Constructor for the UnknownMacroException class, which takes a DebugInfo struct to locate this error in a source file and the unknown macro name. */
            UnknownMacroException(const DebugInfo& debug, const std::string& macro) :
                PreprocessorException(debug, "Encountered unknown macro '" + macro + "'."),
                macro(macro)
            {}

            /* Copies the UnknownMacroException polymorphically. */
            virtual UnknownMacroException* copy() const { return new UnknownMacroException(*this); }

        };
        /* Exception for when a macro-token is followed by an unsupported value type for that macro, but we haven't implemented that particular token. */
        class IllegalMacroValueException: public PreprocessorException {
        public:
            /* The name of the macro where the error occurred. */
            std::string macro;
            /* The type of the value we got. */
            std::string given;
            /* List of the allowed types. */
            std::string expected;

            /* Constructor for the IllegalMacroValueException class, which takes a DebugInfo struct to locate this error in a source file, a macro name where the error occurred, the name of the given type and the name(s) of the type(s) we expected. */
            IllegalMacroValueException(const DebugInfo& debug, const std::string& macro_name, const std::string& given_type, const std::string& expected_type = "") :
                PreprocessorException(debug, macro_name + "-macro can't be followed up by a " + given_type + (!expected_type.empty() ? (" (expected " + expected_type + ").") : ".")),
                macro(macro_name),
                given(given_type),
                expected(expected_type)
            {}

            /* Copies the IllegalMacroValueException polymorphically. */
            virtual IllegalMacroValueException* copy() const { return new IllegalMacroValueException(*this); }

        };
        /* Exception for when a system file is included that is not baked into this compiler. */
        class IllegalSysFileException: public PreprocessorException {
        public:
            /* Constructor for the IllegalSysFileException class, which takes a DebugInfo struct linking this error to the source, the system file that was attempted to be included and a string describing those that can be included. */
            IllegalSysFileException(const DebugInfo& debug, const std::string& given, const std::string& expected) :
                PreprocessorException(debug, "Unknown system file '" + given + "' (this compiler only knows " + expected + ").")
            {}

            /* Copies the IllegalSysFileException polymorphically. */
            virtual IllegalSysFileException* copy() const { return new IllegalSysFileException(*this); }

        };
        /* Exception for when an ifdef is not closed before the end of the file. */
        class UnmatchedIfdefException: public PreprocessorException {
        public:
            /* Constructor for the UnmatchedIfdefException class, which only takes a DebugInfo struct s.t. we can link the error to a source file. */
            UnmatchedIfdefException(const DebugInfo& debug) :
                PreprocessorException(debug, "#ifdef not closed by an #endif.")
            {}

            /* Copies the UnmatchedIfdefException polymorphically. */
            virtual UnmatchedIfdefException* copy() const { return new UnmatchedIfdefException(*this); }

        };
        /* Exception for when an ifndef is not closed before the end of the file. */
        class UnmatchedIfndefException: public PreprocessorException {
        public:
            /* Constructor for the UnmatchedIfndefException class, which only takes a DebugInfo struct s.t. we can link the error to a source file. */
            UnmatchedIfndefException(const DebugInfo& debug) :
                PreprocessorException(debug, "#ifndef not closed by an #endif.")
            {}

            /* Copies the UnmatchedIfndefException polymorphically. */
            virtual UnmatchedIfndefException* copy() const { return new UnmatchedIfndefException(*this); }

        };
        /* Exception for when an endif is given without starting ifdef or ifndef. */
        class UnmatchedEndifException: public PreprocessorException {
        public:
            /* Constructor for the UnmatchedEndifException class, which only takes a DebugInfo struct s.t. we can link the error to a source file. */
            UnmatchedEndifException(const DebugInfo& debug) :
                PreprocessorException(debug, "Encountered #endif without starting #ifdef or #ifndef.")
            {}

            /* Copies the UnmatchedEndifException polymorphically. */
            virtual UnmatchedEndifException* copy() const { return new UnmatchedEndifException(*this); }

        };



        /* Baseclass for all the preprocessor warnings. */
        class PreprocessorWarning: public ADLCompileWarning {
        public:
            /* Constructor for the PreprocessorWarning class, which takes the warning type, a DebugInfo struct to locate this error in a source file and optionally a message. */
            PreprocessorWarning(const std::string& type, const DebugInfo& debug, const std::string& message = "") :
                ADLCompileWarning(type, debug, message)
            {}

        };

        /* Warning for when a variable is defined twice. */
        class DuplicateDefineWarning: public PreprocessorWarning {
        public:
            /* Constructor for the DuplicateDefineWarning class, which takes a DebugInfo struct to link this error to a source file and the define that was a duplicate. */
            DuplicateDefineWarning(const DebugInfo& debug, const std::string& define) :
                PreprocessorWarning("duplicate-define", debug, "Define '" + define + "' is already defined.")
            {}

            /* Copies the DuplicateDefineWarning polymorphically. */
            virtual DuplicateDefineWarning* copy() const { return new DuplicateDefineWarning(*this); }

        };
        /* Warning for when a define was attempted to be undefined, but it was never defined in the first place. */
        class MissingDefineWarning: public PreprocessorWarning {
        public:
            /* Constructor for the MissingDefineWarning class, which takes a DebugInfo struct to link this error to a source file and the define that was missing. */
            MissingDefineWarning(const DebugInfo& debug, const std::string& define) :
                PreprocessorWarning("missing-define", debug, "Define '" + define + "' is not defined.")
            {}

            /* Copies the MissingDefineWarning polymorphically. */
            virtual MissingDefineWarning* copy() const { return new MissingDefineWarning(*this); }

        };

    }



    /* The Preprocessor class is used to wrap around the ADLTokenizer and process things like includes and conditional compilation. */
    class Preprocessor {
    private:
        /* Reference to the current tokenizer. */
        Tokenizer* current;
        /* Place to store all internal Tokenizers, one per open file. */
        Tokenizer** tokenizers;
        /* The number of tokenizers currently open. */
        size_t length;
        /* The maximum number of Tokenizers we have allocated space for. */
        size_t max_length;
        
        /* Keeps track if we're able to still get elements from the stream. */
        bool done_tokenizing;
        /* Keeps track of all the paths we seen so far. */
        std::vector<std::string> included_paths;
        /* Keeps track of all defines currently present. */
        std::vector<std::string> defines;
        /* Keeps track of how many compileable and unclosed ifdefs we saw. */
        std::vector<std::tuple<std::string, DebugInfo>> ifdefs;
        /* List of all previously defined ifdefs for each file in the include tree. */
        std::vector<std::vector<std::tuple<std::string, DebugInfo>>> ifdefs_stack;
        
        /* Resizes the internal tokenizers list by doubling its size. */
        void resize();
        /* Used internally to get any tokens from the combined input stream. */
        Token* read_head(bool pop);
        /* Used to check if a given vector of strings contains the given string. */
        static bool contains(const std::vector<std::string>& haystack, const std::string& needle);
        /* Used to check if a given vector of strings contains the given string. Returns the index of the found result as the first argument. */
        static bool contains(size_t& index, const std::vector<std::string>& haystack, const std::string& needle);

        /* Handler for the include-macro. */
        Token* include_handler(bool pop, Token* token);
        /* Handler for the define-macro. */
        Token* define_handler(bool pop, Token* token);
        /* Handler for the undefine-macro. */
        Token* undefine_handler(bool pop, Token* token);
        /* Handler for the ifdef-macro. */
        Token* ifdef_handler(bool pop, Token* token);
        /* Handler for the ifndef-macro. */
        Token* ifndef_handler(bool pop, Token* token);
        /* Handler for the endif-macro. */
        Token* endif_handler(bool pop, Token* token);

    public:
        /* Constructor for the Preprocessor class, which takes a filename to open and a vector containing all the defines from the CLI. */
        Preprocessor(const std::string& filename, const std::vector<std::string>& defines);
        /* Copy constructor for the Preprocessor class, which is deleted, since we can't copy Tokenizers either */
        Preprocessor(const Preprocessor& other) = delete;
        /* Move constructor for the Preprocessor class. */
        Preprocessor(Preprocessor&& other);
        /* Destructor for the Preprocessor class. */
        ~Preprocessor();

        /* Returns the top element on the merged streams of the root file and all included files, but doesn't remove it. */
        Token* peek();
        /* Removes the top token of the stream and returns it. */
        Token* pop();
        /* Puts a token back on the stream. Note that it will not be parsed again, so may retrieving it will be much faster than the first time. */
        inline void push(const Token& token) { return this->push((Token*) &token); }
        /* Puts a token back on the stream. Note that it will not be parsed again, so may retrieving it will be much faster than the first time. */
        template <class T>
        inline void push(const ValueToken<T>& token) { return this->push((Token*) &token); }
        /* Puts a token back on the stream. Note that it will not be parsed again, so may retrieving it will be much faster than the first time. */
        inline void push(Token* token) { return this->current->push(token); }

        /* Returns the current trail of breadcrumbs. */
        inline const std::vector<std::string>& breadcrumbs() const { return this->current->filenames; }

        /* Returns true if an end-of-file has been reached. */
        inline bool eof() const { return this->done_tokenizing; }

    };
    
}

#endif
