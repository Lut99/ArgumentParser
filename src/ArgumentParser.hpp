/* ARGUMENT PARSER.hpp
 *   by Lut99
 *
 * Created:
 *   6/4/2020, 12:51:55 PM
 * Last edited:
 *   17/09/2020, 17:41:28
 * Auto updated?
 *   Yes
 *
 * Requirements:
 *   Note that this file requires at least C++17 due to it's use of std::any().
 * 
 * Description:
 *   This file implements the ArgumentParser. This is meant to be an
 *   easy-to-use parser for arguments, with a focus on parsing (custom)
 *   types automatically.
 * 
 *   For an in-depth description and documentation, see:
 *   https://github.com/Lut99/ArgumentParser/wiki
**/

#ifndef ARGUMENTPARSER_HPP
#define ARGUMENTPARSER_HPP

#include <vector>
#include <unordered_map>
#include <istream>
#include <sstream>
#include <any>
#include <exception>
#include <limits>
#include <algorithm>

#define STR_T(STRING) decltype(STRING ## _tstr)

namespace Lut99 {

    /******************** ENUMS ********************/

    enum class ArgumentType {
        positional = 0,
        option = 1,
        flag = 2,
        multi_argument = 3,
        included_group = 4,
        excluded_group = 5,
        required_group = 6
    };
    const static std::string argtype_name_map[] = {
        "Positional",
        "Option",
        "Flag",
        "MultiArgument",
        "IncludedGroup",
        "ExcludedGroup",
        "RequiredGroup"  
    };

    /******************** HELPER FUNCTIONS ********************/

    /* Returns the name of a given standard type. */
    template<class T> struct type_name { static constexpr const char* value = "other"; };
    template<> struct type_name<unsigned char> { static constexpr const char* value = "unsigned byte"; };
    template<> struct type_name<signed char> { static constexpr const char* value = "byte"; };
    template<> struct type_name<unsigned short> { static constexpr const char* value = "unsigned short"; };
    template<> struct type_name<signed short> { static constexpr const char* value = "short"; };
    template<> struct type_name<unsigned int> { static constexpr const char* value = "unsigned int"; };
    template<> struct type_name<signed int> { static constexpr const char* value = "int"; };
    template<> struct type_name<unsigned long> { static constexpr const char* value = "unsigned long"; };
    template<> struct type_name<signed long> { static constexpr const char* value = "long"; };
    template<> struct type_name<unsigned long long> { static constexpr const char* value = "unsigned long long"; };
    template<> struct type_name<signed long long> { static constexpr const char* value = "long long"; };
    template<> struct type_name<float> { static constexpr const char* value = "float"; };
    template<> struct type_name<double> { static constexpr const char* value = "double"; };
    template<> struct type_name<bool> { static constexpr const char* value = "boolean"; };
    template<> struct type_name<char> { static constexpr const char* value = "char"; };
    template<> struct type_name<std::string> { static constexpr const char* value = "string"; };
    template<> struct type_name<char*> { static constexpr const char* value = "string"; };

    /* Checks if a character is a whitespace. */
    inline bool is_whitespace(char c) { return c == ' ' || c == '\t' || c == '\n'; }

    /* Checks whether or not a name is valid (consists of letters, numbers, and a select few special characters). Returns '\0' if it's valid, or the illegal character otherwise. */
    char is_valid_name(const std::string& name) {
        for (char c : name) {
            if (  (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9') &&
                  c != '_' && c != '-') {
                return c;
            }
        }
        return '\0';
    }
    /* Returns whether or not a shortlabel is valid (consists of letters or numbers). */
    inline bool is_valid_shortlabel(char shortlabel) {
        return (shortlabel > '0' && shortlabel < '9') || (shortlabel > 'a' && shortlabel < 'z') || (shortlabel > 'A' && shortlabel < 'Z');
    }

    /* Wraps a bit of text into a vector of lines with the given maximum length. Words are tried to be wrapped whole, but they can be broken off if too long. */
    std::vector<std::string> linewrap(std::string text, size_t max_length) {
        std::vector<std::string> result;

        // Loop through the string and write word-by-word to the wordbuffer. These are grouped in the linebuffer until it is time to wrap.
        const char* ctext = text.c_str();
        size_t word_size = 0;
        size_t line_size = 0;
        std::stringstream wordbuffer;
        std::stringstream linebuffer;
        for (size_t i = 0; ; i++) {
            char c = ctext[i];

            // Switch based on the c
            switch(c) {
                case '\0':
                case '\n':
                case ' ':
                    // End of a word, so write it to the linebuffer if there still is space
                    if (line_size + 1 + word_size <= max_length) {
                        // It fits, so add it to the linebuffer
                        linebuffer << wordbuffer.str() << c;
                        line_size += 1 + word_size;

                        // Reset the word
                        wordbuffer.str("");
                        word_size = 0;
                    } else if (word_size + 1 <= max_length) {
                        // It does not fit this line but will fit and empty one, so we move to the next line
                        result.push_back(linebuffer.str());

                        // Put the word in the buffer for the next line
                        linebuffer.str("");
                        linebuffer << wordbuffer.str() << c;
                        line_size = word_size + 1;

                        // Reset the word
                        wordbuffer.str("");
                        word_size = 0;
                    } else {
                        // It does not fit this one nor an empty one, so instead we split the word and write it on new lines until there's no more word left.
                        std::string word = wordbuffer.str();
                        while (word_size > 0) {
                            // Write current linebuffer to the result
                            result.push_back(linebuffer.str());

                            // Fill it with the next part of the word
                            linebuffer.str("");
                            linebuffer << word.substr(0, max_length);
                            line_size = max_length;

                            // Truncate the word
                            word = word.substr(max_length);
                        }
                        linebuffer << c;
                    }
                    break;
                default:
                    wordbuffer << c;
                    ++word_size;
            }

            // If it was a zero-string, stop
            if (c == '\0') { break; }
        }

        // Write the final linebuffer and done (if it is non-empty)
        if (line_size > 0) {
            result.push_back(linebuffer.str());
        }

        // Done, return
        return result;
    }

    /* Converts all characters in a given string to uppercase */
    std::string upperify(std::string text) {
        std::stringstream sstr;
        for (char c : text) {
            if (c >= 'a' && c <= 'z') {
                sstr << ((char) (c + ('A' - 'a')));
            } else {
                sstr << c;
            }
        }
        return sstr.str();
    }





    /******************** ARGUMENTPARSER TYPE SYSTEM ********************/

    /* String placeholder type. */
    template <char... chars>
    using tstring = std::integer_sequence<char, chars...>;

    template <typename T, T... chars>
    constexpr tstring<chars...> operator""_tstr() { return { }; }

    /* Instantiated version of the DerivedType class, containing information that is moldable at runtime instead of compile time alone. */
    struct RuntimeType {
    public:
        const std::string type_name;
        const size_t type_hash;
        const std::any (*parse_func)(std::vector<std::string>);
        const size_t n_values;

        /* Constructor of the RuntimeType class which takes the name & hash of the target type, the assigned parser and the number of values. */
        RuntimeType(const char* type_name, const size_t type_hash, const std::any (*parse_func)(std::vector<std::string>), const size_t n_values) :
            type_name(type_name),
            type_hash(type_hash),
            parse_func(parse_func),
            n_values(n_values)
        {}

        /* Returns true if this and the other type are the same. */
        inline bool operator==(const RuntimeType& other) const { return this->type_name == other.type_name && this->type_hash == other.type_hash && this->parse_func == other.parse_func && this->n_values == other.n_values; }
        /* Returns true if this and the other type are different. */
        inline bool operator!=(const RuntimeType& other) const { return this->type_name != other.type_name || this->type_hash != other.type_hash || this->parse_func != other.parse_func || this->n_values != other.n_values; }

        /* Returns true if this and the given typeid point to the same type. */
        inline bool operator==(const type_info& rhs) const { return this->type_hash == rhs.hash_code(); }
        /* Returns true if this and the given typeid point to the same type. */
        friend inline bool operator==(const type_info& lhs, const RuntimeType& rhs);
        /* Returns true if this and the given typeid point to a different type. */
        inline bool operator!=(const type_info& rhs) const { return this->type_hash != rhs.hash_code(); }
        /* Returns true if this and the given typeid point to a different type. */
        friend inline bool operator!=(const type_info& lhs, const RuntimeType& rhs);

    };
    /* Returns true if this and the given typeid point to the same type. */
    inline bool operator==(const type_info& lhs, const RuntimeType& rhs) { return rhs == lhs; }
    /* Returns true if this and the given typeid point to a different type. */
    inline bool operator!=(const type_info& lhs, const RuntimeType& rhs) { return rhs == lhs; }

    /* Type marks the base of all ArgumentParser-supported types. */
    struct BaseType { };

    /* Template for types used in the ArgumentParser. Used to specify behaviour for a certain type. */
    template <typename, size_t, std::any (*)(std::vector<std::string>), typename...>
    struct Type : public BaseType { };

    /* Template for types used in the ArgumentParser. Used to specify behaviour for a certain type. This specialization focusses on string literals, and should be used with STR_T(). */
    template <typename T, size_t N_VALUES, std::any (*PARSE_FUNC)(std::vector<std::string>), char... TYPENAME>
    struct Type<T, N_VALUES, PARSE_FUNC, tstring<TYPENAME...>> : public BaseType {
        /* References the chosen type. */
        using type = T;
        /* The name / uid of the type. */
        static constexpr const char type_name[] = { TYPENAME..., '\0' };
        /* The parser for this type. */
        static constexpr const std::any (*parse_func)(std::vector<std::string>) = PARSE_FUNC;
        /* The number of values that this type uses. */
        static constexpr const size_t n_values = N_VALUES;

        /* Generates a RuntimeType from this specific DerivedType. */
        static RuntimeType runtime() {
            return RuntimeType(this->type_name, typeid(this->type).hash_code(), this->parse_func, this->n_values);
        }
    };





    /******************** ARGUMENTPARSER EXCEPTIONS ********************/

    /* General exception which forms the baseclass of all child exceptions. */
    class ArgumentParserException : public std::exception {
    protected:
        /* The message stored by any of the subclasses. Can be directly edited by them. */
        std::string msg;

        /* Returns the message of a derived class plus whatever the parent classes wants to add. */
        static inline std::string generate_message(const std::string& message) {
            return "[ERROR]" + (message.empty() ? "" : " " + message);
        }

    public:
        /* Constructor for the ArgumentParserException which optionally takes a message. */
        ArgumentParserException(const std::string& message = "") :
            msg(message)
        {}

        /* Returns the msg as string. */
        inline std::string get_msg() const { return this->msg; }

        /* Returns the c-string equivalent of the message (exception-compatible). */
        virtual const char* what() const throw() {
            return this->msg.c_str();
        }

        /* Allows derived classes to copy themselves. */
        virtual ArgumentParserException* copy() const {
            return new ArgumentParserException(*this);
        }
    };
    /* Exception that isn't really an exception, and used when --help is given and automatically handled. */
    class HelpHandledException : public ArgumentParserException {
    private:
        /* The message that was generated by the auto help implementation. */
        std::string help_message;

    public:
        /* The constructor for the HelpHandledException, which takes the help message generated. */
        HelpHandledException(const std::string& help_message) :
            ArgumentParserException(generate_message("Automatically handled the --help flag.")),
            help_message(help_message)
        {}

        /* Returns the automatically generated help message as string. Print to show it to the user, then exit cleanly. */
        inline std::string get_help_message() const { return this->help_message; }

        /* Allows derived classes to copy themselves. */
        virtual HelpHandledException* copy() const {
            return new HelpHandledException(*this);
        }
    };



    /* Exception which forms the base for all exceptions when something went wrong on the programmer's side. */
    class ProgrammingException : public ArgumentParserException {
    protected:
        /* The context of the exception, aka, some identifier of the function where it occurs. */
        std::string context;

        /* Returns the message of a derived class plus whatever the parent classes wants to add. */
        static inline std::string generate_message(const std::string& context, const std::string& message) {
            return ArgumentParserException::generate_message(context + (message.empty() ? "" : ": " + message));
        }

    public:
        /* Constructor for the ProgrammingException which takes a context (the function where it occured) and optionally a message. */
        ProgrammingException(const std::string& context, const std::string& message = "") :
            ArgumentParserException(message),
            context(context)
        {}

        /* Returns the context where the exception occurred. */
        inline std::string get_context() const { return this->context; }

        /* Allows derived classes to copy themselves. */
        virtual ProgrammingException* copy() const {
            return new ProgrammingException(*this);
        }
    };
    /* Exception for when a Positional was not optional after an optional Positional has been declared. */
    class OptionalPositionalException : public ProgrammingException {
    private:
        /* The name of the Positional that was illegal. */
        std::string name;

    public:
        /* Constructor for the OptionalPositionalException which takes the uid of the illegal Positional. */
        OptionalPositionalException(const std::string& context, const std::string& name) :
            ProgrammingException(context, generate_message(context, "Encountered mandatory Positional '" + name + "' after optional Positionals are declared.")),
            name(name)
        {}

        /* Returns the uid of the Positional that was causing this exception. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual OptionalPositionalException* copy() const {
            return new OptionalPositionalException(*this);
        }

    };
    /* Exception for when a Positional is set to be repeated any number of times, but is not the last Positional. */
    class AnyNumberPositionalException : public ProgrammingException {
    private:
        /* The name of the Positional that was illegal. */
        std::string name;

    public:
        /* Constructor for the AnyNumberPositionalException which takes the uid of the illegal Positional. */
        AnyNumberPositionalException(const std::string& context, const std::string& name) :
            ProgrammingException(context, generate_message(context, "Encountered non-last Positional '" + name + "' that can be repeated any number of times, which is ambigious.")),
            name(name)
        {}

        /* Returns the uid of the Positional that was causing this exception. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual AnyNumberPositionalException* copy() const {
            return new AnyNumberPositionalException(*this);
        }

    };

    /* Baseclass exception for when the same things are expected, but different things are given. */
    class DifferentException : public ProgrammingException {
    public:
        /* Constructor for the DifferentException which takes a context and optionally a message. */
        DifferentException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual DifferentException* copy() const {
            return new DifferentException(*this);
        }

    };
    /* Exception for when two arguments are given that are expected to be of the same type, but are actually of a different type. */
    class DifferentArgTypeException : public DifferentException {
    private:
        /* The type of the first argument given. */
        std::string type1;
        /* The type of the second argument given. */
        std::string type2;

    public:
        /* Constructor for the DifferentArgTypeException, which takes a context, the type of the first argument (as string) and the type of the second argument (as string). */
        DifferentArgTypeException(const std::string& context, const std::string& first_type, const std::string& second_type) :
            DifferentException(context, generate_message(context, "Expected arguments to have the same type, but instead got " + first_type + " and " + second_type + ".")),
            type1(first_type),
            type2(second_type)
        {}

        /* Returns the type of the first argument given (as string). */
        inline std::string get_first_type() const { return this->type1; }
        /* Returns the type of the second argument given (as string). */
        inline std::string get_second_type() const { return this->type2; }

        /* Allows derived classes to copy themselves. */
        virtual DifferentArgTypeException* copy() const {
            return new DifferentArgTypeException(*this);
        }
        
    };

    /* Baseclass exception for when different things are expected, but the same ones are given. */
    class SameException : public ProgrammingException {
    public:
        /* Constructor for the SameException which takes a context and optionally a message. */
        SameException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual SameException* copy() const {
            return new SameException(*this);
        }

    };
    /* Exception for when two arguments are given that are actually the same argument. */
    class SameArgumentException : public ProgrammingException {
    private:
        /* The name of the argument that occured more than once. */
        std::string name;

    public:
        /* Constructor for the SameArgumentException, which takes a context and the name of the Argument that was illegally given multiple times. */
        SameArgumentException(const std::string& context, const std::string& name) :
            ProgrammingException(context, generate_message(context, "Expected different arguments, but instead got argument '" + name + "' more than once.")),
            name(name)
        {}

        /* Returns the name of the argument that illegally occured more than once. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual SameArgumentException* copy() const {
            return new SameArgumentException(*this);
        }
        
    };

    /* Baseclass exception for when something isn't what we expected it to be. */
    class MismatchException : public ProgrammingException {
    public:
        /* Constructor for the MismatchException that takes a context and an optional message. */
        MismatchException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual MismatchException* copy() const {
            return new MismatchException(*this);
        }

    };
    /* Exception for when an accessed argument is supposed to be a Position, but it wasn't. */
    class PositionalMismatchException : public MismatchException {
    private:
        /* The name of the incorrectly accessed argument. */
        std::string name;

    public:
        /* Constructor for the PositionalMismatchException which takes a context (the function where it occured), the name of the Positional and the index of the Positional. */
        PositionalMismatchException(const std::string& context, const std::string& name) :
            MismatchException(context, generate_message(context, "Tried to access a non-Positional argument '" + name + "' as if it were a Positional")),
            name(name)
        {}

        /* Returns the name of the mismatched argument. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual PositionalMismatchException* copy() const {
            return new PositionalMismatchException(*this);
        }

    };
    /* Exception for when an accessed argument is supposed to be a Flag, but it wasn't. */
    class FlagMismatchException : public MismatchException {
    private:
        /* The name of the incorrectly accessed argument. */
        std::string name;

    public:
        /* Constructor for the PositionalMismatchException which take a context (the function where it occured) and the name of the mismatched argument. */
        FlagMismatchException(const std::string& context, const std::string& name) :
            MismatchException(context, generate_message(context, "Tried to access a non-Flag argument '" + name + "' as if it were a Flag")),
            name(name)
        {}

        /* Returns the name of the mismatched argument. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual FlagMismatchException* copy() const {
            return new FlagMismatchException(*this);
        }

    };
    /* Exception for when an accessed argument is supposed to be an Option, but it wasn't. */
    class OptionMismatchException : public MismatchException {
    private:
        /* The name of the incorrectly accessed argument. */
        std::string name;

    public:
        /* Constructor for the OptionMismatchException which take a context (the function where it occured) and the name of the mismatched argument. */
        OptionMismatchException(const std::string& context, const std::string& name) :
            MismatchException(context, generate_message(context, "Tried to access a non-Option argument '" + name + "' as if it were an Option")),
            name(name)
        {}

        /* Returns the name of the mismatched argument. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual OptionMismatchException* copy() const {
            return new OptionMismatchException(*this);
        }

    };
    /* Exception for when an accessed argument is supposed to have a value, but it didn't. */
    class ValueMismatchException : public MismatchException {
    private:
        /* Name of the argument which was incorrectly presumed to have a value. */
        std::string name;

    public:
        /* Constructor for the PositionalMismatchException which take a context (the function where it occured) and the name of the non-value carrying argument. */
        ValueMismatchException(const std::string& context, const std::string& name) :
            MismatchException(context, generate_message(context, "Tried to access argument '" + name + "' as if it had a value, but it didn't (it was a Flag).")),
            name(name)
        {}

        /* Returns the name of the argument which was supposed to have a value, but didn't. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual ValueMismatchException* copy() const {
            return new ValueMismatchException(*this);
        }

    };
    /* Exception for when the returned type of a parser does not match the defined type in a DerivedType. */
    class ParserMismatchException : public MismatchException {
    private:
        /* The type name of the DerivedType that went wrong. */
        std::string type_name;

    public:
        /* Constructor for the ParserMismatchException that takes a context and the name of the erroring DerivedType. */
        ParserMismatchException(const std::string& context, const std::string& type_name) :
            MismatchException(context, generate_message(context, "The return type of the parser for Type '" + type_name + "' does not match the Type's type.")),
            type_name(type_name)
        {}

        /* Returns the type name of the DerivedType who's parser and type do not match. */
        inline std::string get_type_name() const { return this->type_name; }

        /* Allows derived classes to copy themselves. */
        virtual ParserMismatchException* copy() const {
            return new ParserMismatchException(*this);
        }

    };
    /* Exception for when a given type is not the one we expect. */
    class TypeMismatchException : public MismatchException {
    private:
        /* The name of the expected type. */
        std::string expected;
        /* The name of the received type. */
        std::string given;

    public:
        TypeMismatchException(const std::string& context, const std::string& expected_type_name, const std::string& given_type_name) :
            MismatchException(context, generate_message(context, "The requested type is not what we expect: expected '" + expected_type_name + "', got '" + given_type_name + "'.")),
            expected(expected_type_name),
            given(given_type_name)
        {}

        /* Returns the expected type's name. */
        inline std::string get_expected() const { return this->expected; }
        /* Returns the given type's name. */
        inline std::string get_given() const { return this->given; }

        /* Allows derived classes to copy themselves. */
        virtual TypeMismatchException* copy() const {
            return new TypeMismatchException(*this);
        }

    };
    /* Exception for when the ArgumentType is not the one we expect. */
    class ArgTypeMismatchException : public MismatchException {
    private:
        /* The name of the Argument in question. */
        std::string name;
        /* The expected type. */
        ArgumentType expected;
        /* The received type. */
        ArgumentType given;

    public:
        ArgTypeMismatchException(const std::string& context, const std::string& name, const ArgumentType expected_type, const ArgumentType given_type) :
            MismatchException(context, generate_message(context, "Argument '" + name + "' is a " + argtype_name_map[(int) expected_type] + ", but is treated as a " + argtype_name_map[(int) given_type] + ".")),
            name(name),
            expected(expected_type),
            given(given_type)
        {}

        /* Returns the name of the Argument who's type was mismatching. */
        inline std::string get_name() const { return this->name; }
        /* Returns the expected type. */
        inline ArgumentType get_expected() const { return this->expected; }
        /* Returns the given type. */
        inline ArgumentType get_given() const { return this->given; }

        /* Allows derived classes to copy themselves. */
        virtual ArgTypeMismatchException* copy() const {
            return new ArgTypeMismatchException(*this);
        }

    };
    /* Exception for when a given argument can be repeated whereas we expect it not to. */
    class AnyNumberMismatchException : public MismatchException {
    private:
        /* The name of the argument that shouldn't be able to repeat. */
        std::string name;

    public:
        AnyNumberMismatchException(const std::string& context, const std::string& name) :
            MismatchException(context, generate_message(context, "Argument '" + name + "' can be specified multiple times, and must be retrieved using Arguments::getp().")),
            name(name)
        {}

        /* Returns the name of the argument that could be specified multiple times against our expectations. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual AnyNumberMismatchException* copy() const {
            return new AnyNumberMismatchException(*this);
        }

    };
    /* Exception for when an argument is expected to be Optional, but isn't. Not to be confused with the OptionMismatchException. */
    class OptionalMismatchException : public MismatchException {
    private:
        /* The name of the argument that wasn't optional. */
        std::string name;

    public:
        OptionalMismatchException(const std::string& context, const std::string& name) :
            MismatchException(context, generate_message(context, "Argument '" + name + "' is expected to be optional, but is in fact mandatory.")),
            name(name)
        {}

        /* Returns the name of the argument that could be specified multiple times against our expectations. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual OptionalMismatchException* copy() const {
            return new OptionalMismatchException(*this);
        }

    };

    /* Baseclass for all exceptions that have to do with an identifier being unknown. */
    class UnknownException : public ProgrammingException {
    public:
        /* Constructor for the UnknownException which takes a context and optionally a message. */
        UnknownException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual UnknownException* copy() const {
            return new UnknownException(*this);
        }
    };
    /* Exception for when an argument accessed from the Arguments dict is unknown. */
    class UnknownNameException : public UnknownException {
    private:
        /* The name that did not match with an argument. */
        std::string name;

    public:
        /* Constructor for the UnknownNameException which takes a context and the unknown uid. */
        UnknownNameException(const std::string& context, const std::string& name) :
            UnknownException(context, generate_message(context, "Could not find argument with name '" + name + "'")),
            name(name)
        {}

        /* Returns the name that couldn't be matched to a known argument. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual UnknownNameException* copy() const {
            return new UnknownNameException(*this);
        }
    };

    /* Baseclass for all exceptions that have to do with an identifier being duplicate. */
    class DuplicateException : public ProgrammingException {
    public:
        /* Constructor for the DuplicateException which takes a context and optionally a message. */
        DuplicateException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual DuplicateException* copy() const {
            return new DuplicateException(*this);
        }

    };
    /* Exception for when a name of an argument is already defined. */
    class DuplicateNameException : public DuplicateException {
    private:
        /* The name that already existed. */
        std::string name;

    public:
        /* Constructor for the DuplicateNameException which takes a context and the name that was a duplicate. */
        DuplicateNameException(const std::string& context, const std::string& name) :
            DuplicateException(context, generate_message(context, "An argument with name '" + name + "' already exists.")),
            name(name)
        {}

        /* Returns the name that already existed. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual DuplicateNameException* copy() const {
            return new DuplicateNameException(*this);
        }

    };
    /* Exception for when the shortlabel of an argument is already defined. */
    class DuplicateShortlabelException : public DuplicateException {
    private:
        /* The shortlabel that already existed. */
        char shortlabel;

    public:
        /* Constructor for the DuplicateShortlabelException which takes a context and the name that was a duplicate. */
        DuplicateShortlabelException(const std::string& context, const char shortlabel) :
            DuplicateException(context, generate_message(context, (std::string("An argument with shortlabel '") += shortlabel) + "' already exists.")),
            shortlabel(shortlabel)
        {}

        /* Returns the shortlabel that already existed. */
        inline char get_shortlabel() const { return this->shortlabel; }

        /* Allows derived classes to copy themselves. */
        virtual DuplicateShortlabelException* copy() const {
            return new DuplicateShortlabelException(*this);
        }

    };

    /* Baseclass exception for all exceptions that point to some illegal value at the programming side. */
    class IllegalException : public ProgrammingException {
    public:
        /* Constructor for the IllegalException which takes a context and optionally a message. */
        IllegalException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual IllegalException* copy() const {
            return new IllegalException(*this);
        }

    };
    /* Exception for when a name contains an illegal character. */
    class IllegalNameException : public IllegalException {
    private:
        /* The name that contained the illegal character. */
        std::string name;
        /* The character that was illegal. */
        char illegal_char;
    
    public:
        /* Constructor for the IllegalShortlabelException which takes a context and the illegal character. */
        IllegalNameException(const std::string& context, const std::string& illegal_name, const char illegal_char) :
            IllegalException(context, generate_message(context, "A name can only consists of 0-9, a-z and A-Z characters.")),
            name(illegal_name),
            illegal_char(illegal_char)
        {}

        /* Returns the name that contained an illegal character. */
        inline std::string get_illegal_name() const { return this->name; }
        /* Returns the character that was illegal. */
        inline char get_illegal_char() const { return this->illegal_char; }

        /* Allows derived classes to copy themselves. */
        virtual IllegalNameException* copy() const {
            return new IllegalNameException(*this);
        }

    };
    /* Exception for when a shortlabel consists of an illegal character. */
    class IllegalShortlabelException : public IllegalException {
    private:
        /* The character that was illegal. */
        char illegal_char;
    
    public:
        /* Constructor for the IllegalShortlabelException which takes a context and the illegal character. */
        IllegalShortlabelException(const std::string& context, const char illegal_char) :
            IllegalException(context, generate_message(context, "Shortlabel can only consists of 0-9, a-z and A-Z characters.")),
            illegal_char(illegal_char)
        {}

        /* Returns the character that was illegal. */
        inline char get_illegal_char() const { return this->illegal_char; }

        /* Allows derived classes to copy themselves. */
        virtual IllegalShortlabelException* copy() const {
            return new IllegalShortlabelException(*this);
        }

    };
    /* Exception for when an illegal default value was given. */
    class IllegalDefaultValueException : public IllegalException {
    private:
        /* The name of the argument that was given an illegal default value. */
        std::string name;
        /* The value (as any) of the argument that was given an illegal default value. */
        std::any value;
    
    public:
        /* Constructor for the IllegalDefaultValueException that takes a context, the name of the unfortunate argument and the illegal value itself. */
        IllegalDefaultValueException(const std::string& context, const std::string& name, const std::any& value) :
            IllegalException(context, generate_message(context, "Argument '" + name + "' cannot have a default value of type '" + value.type().name() + "'.")),
            name(name),
            value(value)
        {}

        /* Returns the name of the argument that was given an illegal default value. */
        inline std::string get_name() const { return this->name; }
        /* Returns the value that was illegally given to the argument. */
        inline std::any get_value() const { return this->value; }

        /* Allows derived classes to copy themselves. */
        virtual IllegalDefaultValueException* copy() const {
            return new IllegalDefaultValueException(*this);
        }

    };
    /* Exception for when the entered n_values is invalid. */
    class IllegalNValuesException : public IllegalException {
    private:
        /* The value that was illegal. */
        size_t n_values;
    
    public:
        /* Constructor for the IllegalNValuesException that takes a context and the illegal value of DerivedType's n_value field. */
        IllegalNValuesException(const std::string& context, const size_t n_values) :
            IllegalException(context, generate_message(context, "DerivedType::n_values cannot be equal to " + std::to_string(n_values))),
            n_values(n_values)
        {}

        /* Returns the value for DerivedType::n_values that was illegal. */
        inline size_t get_n_values() const { return this->n_values; }

        /* Allows derived classes to copy themselves. */
        virtual IllegalNValuesException* copy() const {
            return new IllegalNValuesException(*this);
        }

    };



    /* Exception which forms the base for all exceptions when something went wrong on the CLI-user's side - can be automatically handled by the ArgumentParser. */
    class ParseException : public ArgumentParserException {
    public:
        /* Constructor for the ParseException which optionally takes a message. */
        ParseException(const std::string& message = "") :
            ArgumentParserException(message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual ParseException* copy() const {
            return new ParseException(*this);
        }
    };
    /* Exception for when an argument is specified that wasn't allowed to be specified. */
    class ExcludedArgumentException : public ParseException {
    private:
        /* The name of the argument which was duplicate. */
        std::string name;
        /* The shortlabel of the argument which was duplicate. */
        char shortlabel;
    
    public:
        /* Constructor for the ExcludedArgumentException which takes the name of the argument that wasn't allowed to be specified and it's shortlabel. */
        ExcludedArgumentException(const std::string& name, const char shortlabel)
            : ParseException(generate_message("Cannot specify argument --" + name + ", as it is excluded by speficying another argument (see '--help' for details).")),
            name(name),
            shortlabel(shortlabel)
        {};

        /* Returns the name of the duplicate argument. */
        inline std::string get_name() const { return this->name; }
        /* Returns the shortlabel of the duplicate argument. */
        inline char get_shortlabel() const { return this->shortlabel; }

        /* Allows derived classes to copy themselves. */
        virtual ExcludedArgumentException* copy() const {
            return new ExcludedArgumentException(*this);
        }
        
    };
    /* Exception for when an argument is specified twice or more while it could only be specified once. */
    class DuplicateArgumentException : public ParseException {
    private:
        /* The name of the argument which was duplicate. */
        std::string name;
        /* The shortlabel of the argument which was duplicate. */
        char shortlabel;
    
    public:
        /* Constructor for the DuplicateArgumentException which takes the name of the argument and it's shortlabel. */
        DuplicateArgumentException(const std::string& name, const char shortlabel)
            : ParseException(generate_message("Cannot specify argument --" + name + " more than once.")),
            name(name),
            shortlabel(shortlabel)
        {};

        /* Returns the name of the duplicate argument. */
        inline std::string get_name() const { return this->name; }
        /* Returns the shortlabel of the duplicate argument. */
        inline char get_shortlabel() const { return this->shortlabel; }

        /* Allows derived classes to copy themselves. */
        virtual DuplicateArgumentException* copy() const {
            return new DuplicateArgumentException(*this);
        }
        
    };
    /* Exception for when a mandatory argument is missing. */
    class MissingMandatoryException : public ParseException {
    private:
        /* The name of the missing argument. */
        std::string name;

    public:
        /* Constructor for the MissingMandatoryException, which takes the uid of the missing positionals, a placeholder naming it and its index. */
        MissingMandatoryException(const std::string& name)
            : ParseException(generate_message("Missing mandatory Positional '" + name + "'.")),
            name(name)
        {}
        /* Constructor for the MissingMandatoryException, which takes the uid of the missing options and its shortlabel / longlabel. */
        MissingMandatoryException(const std::string& name, const char shortlabel)
            : ParseException(generate_message("Missing mandatory Option '--" + name + (std::string("' (-") += shortlabel) + ").")),
            name(name)
        {}

        /* Returns the name of the Argument that was missing. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual MissingMandatoryException* copy() const {
            return new MissingMandatoryException(*this);
        }

    };

    /* Baseclass exception for when a label is unknown. */
    class UnknownLabelException : public ParseException {
    public:
        /* Constructor for the UnknownLabelException which optionally takes a message. */
        UnknownLabelException(const std::string& message = "") :
            ParseException(message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual UnknownLabelException* copy() const {
            return new UnknownLabelException(*this);
        }

    };
    /* Exception for when given shortlabel is unknown. */
    class UnknownShortlabelException : public UnknownLabelException {
    private:
        /* The shortlabel that was unknown. */
        char shortlabel;

    public:
        /* Constructor for the UnknownLabelException which takes the shortlabel that was unknown. */
        UnknownShortlabelException(const char shortlabel) :
            UnknownLabelException(generate_message((std::string("Unknown argument '-") += shortlabel) + "'.")),
            shortlabel(shortlabel)
        {}

        /* Returns the unknown shortlabel. */
        inline char get_shortlabel() const { return this->shortlabel; }

        /* Allows derived classes to copy themselves. */
        virtual UnknownShortlabelException* copy() const {
            return new UnknownShortlabelException(*this);
        }

    };
    /* Exception for when given longlabel is unknown. */
    class UnknownLonglabelException : public UnknownLabelException {
    private:
        /* The longlabel that was unknown. */
        std::string longlabel;

    public:
        /* Constructor for the UnknownLabelException which takes the longlabel that was unknown. */
        UnknownLonglabelException(const std::string& longlabel) :
            UnknownLabelException(generate_message("Unknown argument '--" + longlabel + "'.")),
            longlabel(longlabel)
        {}

        /* Returns the unknown longlabel. */
        inline std::string get_longlabel() const { return this->longlabel; }

        /* Allows derived classes to copy themselves. */
        virtual UnknownLonglabelException* copy() const {
            return new UnknownLonglabelException(*this);
        }

    };

    /* Baseclass exception for when something is wrong during type parsing. */
    class TypeParseException : public ParseException {
    private:
        /* The message that any derived class gave us. */
        std::string message;

    protected:
        /* The name of the type that errored. */
        std::string type_name;
        /* The name of the argument where something went wrong. */
        std::string name;

        /* Returns the message of a derived class plus whatever the parent classes wants to add. This overload is for Positionals. */
        static inline std::string generate_message(const std::string& type_name, const std::string& name, const std::string& message) {
            return ParseException::generate_message("Could not parse value of the Positional '" + name + "' to " + type_name + ": " + message);
        }
        /* Returns the message of a derived class plus whatever the parent classes wants to add. This overload is for Options. */
        static inline std::string generate_message(const std::string& type_name, const std::string& name, const char shortlabel, const std::string& message) {
            return ParseException::generate_message("Could not parse value of the '--" + name + (std::string("' (-") += shortlabel) + ") to " + type_name + ": " + message);
        }

        /* Declare the parse functions as friend. */
        friend class Positional;
        friend class Option;

    public:
        /* Constructor for the TypeParseException which takes the name of the type that was incorrectly parsed and optionally a message. Use extend() to inject the uid. */
        TypeParseException(const std::string& type_name, const std::string& message = "") :
            ParseException(message),
            message(message),
            type_name(type_name),
            name("")
        {}
        /* Constructor for the TypeParseException which takes the name of the type that was incorrectly parsed, the uid of the Argument and optionally a message. */
        TypeParseException(const std::string& type_name, const std::string& name, const std::string& message) :
            ParseException(message),
            message(message),
            type_name(type_name),
            name(name)
        {}

        /* Returns the name of the type that was wrongly parsed. */
        inline std::string get_type_name() const { return this->type_name; }
        /* Returns the name of the Argument that was wrongly parsed. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual TypeParseException* copy() const {
            return new TypeParseException(*this);
        }

    };
    /* Baseclass exception for when there are not enough values to unpack from the given arguments. The derived exceptions specify whether it is an Option or Positional. */
    class NotEnoughValuesException : public TypeParseException {
    protected:
        /* The number of values we expect. */
        size_t expected;
        /* The number of values we got. */
        size_t got;

    public:
        /* Constructor for the NotEnoughValuesException that takes the name of the type parsed, how many values we expected and how many we got. Use extend() to inject the uid. */
        NotEnoughValuesException(const std::string& type_name, const size_t expected_n_values, const size_t got_n_values) :
            TypeParseException(type_name, "Not enough values given (expected " + std::to_string(expected_n_values) + ", got " + std::to_string(got_n_values) + ")"),
            expected(expected_n_values),
            got(got_n_values)
        {}
        /* Constructor for the NotEnoughValuesException that takes the name of the type parsed, how many values we expected, how many we got and the index of a Positional. */
        NotEnoughValuesException(const std::string& type_name, const size_t expected_n_values, const size_t got_n_values, const std::string& name) :
            TypeParseException(type_name, name, generate_message(type_name, name, "Not enough values given (expected " + std::to_string(expected_n_values) + ", got " + std::to_string(got_n_values) + ")")),
            expected(expected_n_values),
            got(got_n_values)
        {}
                /* Constructor for the NotEnoughValuesException that takes the name of the type parsed, how many values we expected, how many we got and the shortlabel / longlabel of an Option. */
        NotEnoughValuesException(const std::string& type_name, const size_t expected_n_values, const size_t got_n_values, const std::string& name, const char shortlabel) :
            TypeParseException(type_name, name, generate_message(type_name, name, shortlabel, "Not enough values given (expected " + std::to_string(expected_n_values) + ", got " + std::to_string(got_n_values) + ")")),
            expected(expected_n_values),
            got(got_n_values)
        {}

        /* Returns the number of values that we expected. */
        inline size_t get_expected() const { return this->expected; }
        /* Returns the number of values that we got. */
        inline size_t get_got() const { return this->got; }

        /* Allows derived classes to copy themselves. */
        virtual NotEnoughValuesException* copy() const {
            return new NotEnoughValuesException(*this);
        }

    };
    /* Exception for when an illegal character is encountered while parsing a Positional. */
    class IllegalCharException : public TypeParseException {
    protected:
        /* The character that was illegal. */
        char c;

    public:
        /* Constructor which takes the name of the type and the illegal character. Use extend() to inject the uid. */
        IllegalCharException(const std::string& type_name, const char illegal_char) :
            TypeParseException(type_name, (std::string("Illegal character '") += illegal_char) + "' encountered"),
            c(illegal_char)
        {}
        /* Constructor which takes the name of the type, the illegal character and the index of the Positional. */
        IllegalCharException(const std::string& type_name, const char illegal_char, const std::string& name) :
            TypeParseException(type_name, name, generate_message(type_name, name, (std::string("Illegal character '") += illegal_char) + "' encountered")),
            c(illegal_char)
        {}
        /* Constructor which takes the name of the type, the illegal character and the shortlabel / longlabel of the Option. */
        IllegalCharException(const std::string& type_name, const char illegal_char, const std::string& name, const char shortlabel) :
            TypeParseException(type_name, name, generate_message(type_name, name, shortlabel, (std::string("Illegal character '") += illegal_char) + "' encountered")),
            c(illegal_char)
        {}

        /* Returns the character that was illegal. */
        inline char get_c() const { return this->c; }

        /* Allows derived classes to copy themselves. */
        virtual IllegalCharException* copy() const {
            return new IllegalCharException(*this);
        }

    };
    /* Exception for when an invalid value is given (string rather than char). */
    class IllegalValueException : public TypeParseException {
    protected:
        /* The value that was illegal. */
        std::string value;

    public:
        /* Constructor which takes the name of the type and the illegal value. Use extend() to inject the uid. */
        IllegalValueException(const std::string& type_name, const std::string& illegal_value) :
            TypeParseException(type_name, "Illegal value '" + illegal_value + "' encountered"),
            value(illegal_value)
        {}
        /* Constructor which takes the name of the type, the illegal character and the index of the Positional. */
        IllegalValueException(const std::string& type_name, const std::string& illegal_value, const std::string& name) :
            TypeParseException(type_name, name, generate_message(type_name, name, "Illegal value '" + illegal_value + "' encountered")),
            value(illegal_value)
        {}
        /* Constructor which takes the name of the type, the illegal character and the shortlabel / longlabel of the Option. */
        IllegalValueException(const std::string& type_name, const std::string& illegal_value, const std::string& name, const char shortlabel) :
            TypeParseException(type_name, name, generate_message(type_name, name, shortlabel, "Illegal value '" + illegal_value + "' encountered")),
            value(illegal_value)
        {}

        /* Returns the string that was illegal. */
        inline std::string get_value() const { return this->value; }

        /* Allows derived classes to copy themselves. */
        virtual IllegalValueException* copy() const {
            return new IllegalValueException(*this);
        }

    };
    /* Exception for when a parsed value is out of bounds for some reason. */
    class OutOfBoundsException : public TypeParseException {
    protected:
        /* The boundry that should not be exceeded (in string). */
        std::string boundry;
        /* The value that was attempted to be parsed. */
        std::string value;

    public:
        /* Constructor which takes the type that was attempted to be parsed, the boundry that should not have been exceeded and the value that was given. The uid can be set using extend(). */
        OutOfBoundsException(const std::string& type_name, const std::string& boundry_value, const std::string& given_value)
            : TypeParseException(type_name, "Value '" + given_value + "' is out-of-bounds for bounding value " + boundry_value),
            boundry(boundry_value),
            value(given_value)
        {}
        /* Constructor which takes the type that was attempted to be parsed, the boundry that should not have been exceeded, the value that was given, the uid of the argument and the index of the Positional that is the argument. */
        OutOfBoundsException(const std::string& type_name, const std::string& boundry_value, const std::string& given_value, const std::string& name)
            : TypeParseException(type_name, name, generate_message(type_name, name, "Value '" + given_value + "' is out-of-bounds for bounding value " + boundry_value)),
            boundry(boundry_value),
            value(given_value)
        {}
        /* Constructor which takes the type that was attempted to be parsed, the boundry that should not have been exceeded, the value that was given, the uid of the argument and the shortlabel / longlabel of the Option that is the argument. */
        OutOfBoundsException(const std::string& type_name, const std::string& boundry_value, const std::string& given_value, const std::string& name, const char shortlabel)
            : TypeParseException(type_name, name, generate_message(type_name, name, shortlabel, "Value '" + given_value + "' is out-of-bounds for bounding value " + boundry_value)),
            boundry(boundry_value),
            value(given_value)
        {}

        /* Returns the boundry that should not have been exceeded (as a string). */
        inline std::string get_boundry() const { return this->boundry; }
        /* Returns the value that failed to be parsed. */
        inline std::string get_value() const { return this->value; }

        /* Allows derived classes to copy themselves. */
        virtual OutOfBoundsException* copy() const {
            return new OutOfBoundsException(*this);
        }

    };





    /******************** PARSERS ********************/

    /* Parser for signed integer types (signed char, short, int, long, long long). */
    template <class T, typename = std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value> >
    std::any parse_int(std::vector<std::string> values) {
        // Try to parse the first value only
        if (values.size() < 1) { throw NotEnoughValuesException(type_name<T>::value, 1, values.size()); }

        std::string text = values[0];

        // Strip any preceding whitespaces
        size_t strip_i = 0;
        for (char c : text) {
            if (is_whitespace(c)) {
                ++strip_i;
            } else {
                break;
            }
        }
        text = text.substr(strip_i);

        // Read any minusses preceding the text (ignoring whitespaces)
        int modifier = 1;
        strip_i = 0;
        for (char c : text) {
            if (c == '-') {
                modifier *= -1;
                strip_i++;
            } else if (!is_whitespace(c)) {
                break;
            }
        }
        text = text.substr(strip_i);

        // Strip any following whitespaces
        strip_i = text.size();
        for (size_t i = text.size(); i-- !=0; ) {
            if (is_whitespace(text[i])) {
                --strip_i;
            } else {
                break;
            }
        }
        text = text.substr(0, strip_i);

        // Parse the numbers
        T result = 0;
        for (char c : text) {
            if (c >= '0' && c <= '9') {
                // Check if out-of-bounds yet (use the maximum value if positive, the negative value if negative)
                int value = (int) (c - '0');
                if (modifier >= 0) {
                    // Positive check
                    if (result > std::numeric_limits<T>::max() / 10 || result * 10 > std::numeric_limits<T>::max() - value) {
                        throw OutOfBoundsException(type_name<T>::value, std::to_string(std::numeric_limits<T>::max()), text);
                    }
                } else {
                    // Negative check
                    if (result < std::numeric_limits<T>::min() / 10 || result * 10 < std::numeric_limits<T>::min() + value) {
                        throw OutOfBoundsException(type_name<T>::value, std::to_string(std::numeric_limits<T>::min()), "-" + text);
                    }
                }

                // Increment the result
                result *= 10;
                // Add the value
                result += modifier * value;
            } else {
                throw IllegalCharException(type_name<T>::value, c);
            }
        }

        // Return the number, modified
        return std::any(result);
    }

    /* Parser for unsigned integer types (unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long) */
    template <class T, typename = std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value> >
    std::any parse_uint(std::vector<std::string> values) {
        // Try to parse the first value only
        if (values.size() < 1) { throw NotEnoughValuesException(type_name<T>::value, 1, values.size()); }

        std::string text = values[0];

        // Strip any preceding whitespaces
        size_t strip_i = 0;
        for (char c : text) {
            if (is_whitespace(c)) {
                ++strip_i;
            } else {
                break;
            }
        }
        text = text.substr(strip_i);

        // Strip any following whitespaces
        strip_i = text.size();
        for (size_t i = text.size(); i-- !=0; ) {
            if (is_whitespace(text[i])) {
                --strip_i;
            } else {
                break;
            }
        }
        text = text.substr(0, strip_i);

        // Parse the numbers
        T result = 0;
        for (char c : text) {
            if (c >= '0' && c <= '9') {
                // Check if out-of-bounds yet (use the maximum value if positive, the negative value if negative)
                int value = (int) (c - '0');
                // Positive check
                if (result > std::numeric_limits<T>::max() / 10 || result * 10 > std::numeric_limits<T>::max() - value) {
                    throw OutOfBoundsException(type_name<T>::value, std::to_string(std::numeric_limits<T>::max()), text);
                }

                // Increment the result
                result *= 10;
                // Add the value
                result += value;
            } else {
                throw IllegalCharException(type_name<T>::value, c);
            }
        }

        // Return the number, modified
        return std::any(result);
    }

    /* Parser for single-precision floating-point types. */
    std::any parse_float(std::vector<std::string> values) {
        // Try to parse the first value only
        if (values.size() < 1) { throw NotEnoughValuesException(type_name<float>::value, 1, values.size()); }

        std::string text = values[0];

        // Strip any preceding whitespaces
        size_t strip_i = 0;
        for (char c : text) {
            if (is_whitespace(c)) {
                ++strip_i;
            } else {
                break;
            }
        }
        text = text.substr(strip_i);

        // Strip any following whitespaces
        strip_i = text.size();
        for (size_t i = text.size(); i-- !=0; ) {
            if (is_whitespace(text[i])) {
                --strip_i;
            } else {
                break;
            }
        }
        text = text.substr(0, strip_i);

        // Scout for illegal characters
        for (size_t i = 0; i < text.size(); ++i) {
            char c = text[i];
            if (c != '-' && c != '.' && (c < '0' || c > '9')) {
                throw IllegalCharException(type_name<float>::value, c);
            }
        }

        // Run the text through C++'s default parser
        float result;
        try {
            result = std::stof(text);
        } catch (std::invalid_argument& e) {
            throw IllegalValueException(type_name<float>::value, text);
        } catch (std::out_of_range& e) {
            throw OutOfBoundsException(type_name<float>::value, std::to_string(std::numeric_limits<float>::max()), text);
        }

        // Done, return
        return std::any(result);
    }

    /* Parser for double-precision floating-point types. */
    std::any parse_double(std::vector<std::string> values) {
        // Try to parse the first value only
        if (values.size() < 1) { throw NotEnoughValuesException(type_name<double>::value, 1, values.size()); }

        std::string text = values[0];

        // Strip any preceding whitespaces
        size_t strip_i = 0;
        for (char c : text) {
            if (is_whitespace(c)) {
                ++strip_i;
            } else {
                break;
            }
        }
        text = text.substr(strip_i);

        // Strip any following whitespaces
        strip_i = text.size();
        for (size_t i = text.size(); i-- !=0; ) {
            if (is_whitespace(text[i])) {
                --strip_i;
            } else {
                break;
            }
        }
        text = text.substr(0, strip_i);

        // Scout for illegal characters
        for (size_t i = 0; i < text.size(); ++i) {
            char c = text[i];
            if (c != '-' && c != '.' && (c < '0' || c > '9')) {
                throw IllegalCharException(type_name<double>::value, c);
            }
        }

        // Run the text through C++'s default parser
        double result;
        try {
            result = std::stod(text);
        } catch (std::invalid_argument& e) {
            throw IllegalValueException(type_name<double>::value, text);
        } catch (std::out_of_range& e) {
            throw OutOfBoundsException(type_name<double>::value, std::to_string(std::numeric_limits<double>::max()), text);
        }

        // Done, return
        return std::any(result);
    }

    /* Parser for booleans (they can be either: "true", "1", "yes", "y" for true or "false", "0", "no", "n" for false (capitalization-independent)) */
    std::any parse_bool(std::vector<std::string> values) {
        // Try to parse the first value only
        if (values.size() < 1) { throw NotEnoughValuesException(type_name<bool>::value, 1, values.size()); }

        const char* text = values[0].c_str();
        int i = 0;
        char c;

        // Use a jump table structure to parse using an final-state automaton
start:
        c = text[i++];
        switch(c) {
            case ' ':
            case '\t':
            case '\n':
                goto start;
            case 't':
                goto t;
            case 'y':
                goto y;
            case '1':
                goto end_true;
            case 'f':
                goto f;
            case 'n':
                goto n;
            case '0':
                goto end_false;
            default:
                goto failure;
        }

t:
        c = text[i++];
        switch(c) {
            case 'r':
                goto tr;
            default:
                goto failure;
        }

tr:
        c = text[i++];
        switch(c) {
            case 'u':
                goto tru;
            default:
                goto failure;
        }

tru:
        c = text[i++];
        switch(c) {
            case 'e':
                goto end_true;
            default:
                goto failure;
        }

y:
        c = text[i++];
        switch(c) {
            case ' ':
            case '\t':
            case '\n':
                goto end_true;
            case 'e':
                goto ye;
            case '\0':
                // Done!
                return true;
            default:
                goto failure;
        }

ye:
        c = text[i++];
        switch(c) {
            case 's':
                goto end_true;
            default:
                goto failure;
        }

f:
        c = text[i++];
        switch(c) {
            case 'a':
                goto fa;
            default:
                goto failure;
        }

fa:
        c = text[i++];
        switch(c) {
            case 'l':
                goto fal;
            default:
                goto failure;
        }

fal:
        c = text[i++];
        switch(c) {
            case 's':
                goto fals;
            default:
                goto failure;
        }

fals:
        c = text[i++];
        switch(c) {
            case 'e':
                goto end_false;
            default:
                goto failure;
        }

n:
        c = text[i++];
        switch(c) {
            case ' ':
            case '\t':
            case '\n':
            case 'o':
                goto end_false;
            case '\0':
                // Done!
                return false;
            default:
                goto failure;
        }

end_true:
        c = text[i++];
        switch(c) {
            case ' ':
            case '\t':
            case '\n':
                goto end_true;
            case '\0':
                // Done!
                return std::any(true);
            default:
                goto failure;
        }

end_false:
        c = text[i++];
        switch(c) {
            case ' ':
            case '\t':
            case '\n':
                goto end_false;
            case '\0':
                // Done!
                return std::any(false);
            default:
                goto failure;
        }

failure:
        throw IllegalValueException(type_name<bool>::value, text);
    }

    /* Parser for single characters. Only errors if no character is given. */
    std::any parse_char(std::vector<std::string> values) {
        // Try to parse the first value only
        if (values.size() < 1) { throw NotEnoughValuesException(type_name<char>::value, 1, values.size()); }

        if (values[0].size() != 1) {
            throw IllegalValueException(type_name<char>::value, values[0]);
        }

        return std::any(values[0][0]);
    }

    /* Parser for multiple characters. Only errors if no character is given. */
    std::any parse_string(std::vector<std::string> values) {
        // Try to parse the first value only
        if (values.size() < 1) { throw NotEnoughValuesException(type_name<std::string>::value, 1, values.size()); }

        if (values[0].size() == 0) {
            throw IllegalValueException(type_name<std::string>::value, values[0]);
        }

        return std::any(values[0]);
    }





    /******************** DEFAULT TYPES ********************/

    /* Unsigned byte, which refer's to C's unsigned char. */
    using UByte = Type<unsigned char, 1, parse_uint<unsigned char>, STR_T("unsigned byte")>;
    /* Signed byte, which refer's to C's signed char. */
    using Byte = Type<signed char, 1, parse_int<signed char>, STR_T("byte")>;
    /* Unsigned short, which refer's to C's unsigned short. */
    using UShort = Type<unsigned short, 1, parse_uint<unsigned short>, STR_T("unsigned short")>;
    /* Signed short, which refer's to C's signed short. */
    using Short = Type<short, 1, parse_int<short>, STR_T("short")>;
    /* Unsigned int, which refer's to C's unsigned int. */
    using UInt = Type<unsigned int, 1, parse_uint<unsigned int>, STR_T("unsigned int")>;
    /* Signed int, which refer's to C's signed int. */
    using Int = Type<int, 1, parse_int<int>, STR_T("int")>;
    /* Unsigned long, which refer's to C's unsigned long. */
    using ULong = Type<unsigned long, 1, parse_uint<unsigned long>, STR_T("unsigned long")>;
    /* Signed long, which refer's to C's signed long. */
    using Long = Type<long, 1, parse_int<long>, STR_T("long")>;
    /* Unsigned long long, which refer's to C's unsigned long long. */
    using ULongLong = Type<unsigned long long, 1, parse_uint<unsigned long long>, STR_T("unsigned long long")>;
    /* Signed long long, which refer's to C's signed long long. */
    using LongLong = Type<long long, 1, parse_int<long long>, STR_T("long long")>;
    /* Float, which refers to C's float type. */
    using Float = Type<float, 1, parse_float, STR_T("float")>;
    /* Double, which refers to C's double type. */
    using Double = Type<double, 1, parse_double, STR_T("double")>;
    /* Boolean type, which can be used to interpret true/false, yes/no or 1/0. */
    using Bool = Type<bool, 1, parse_bool, STR_T("bool")>;
    /* Char type, which is used to interpret a single character */
    using Char = Type<char, 1, parse_char, STR_T("char")>;
    /* String type, which is used to interpret multiple characters. */
    using String = Type<std::string, 1, parse_string, STR_T("string")>;





    /******************** THE ARGUMENTS ********************/

    /* The Argument class, which forms the baseclass for all classes that count as command-line arguments. */
    class Argument {
    protected:
        /* The name of the Argument. Is used to identify the argument both behind the screens as on the CLI. */
        std::string name;
        /* The type of the Argument. */
        ArgumentType arg_type;

        /* Constructor for the Argument baseclass, which takes a type of the derived, calling Argument. */
        Argument(ArgumentType arg_type, const std::string& name) :
            arg_type(arg_type)
        {}
    
    public:
        /* Virtual destructor of the Argument class. */
        virtual ~Argument() = default;

        /* Returns the name of the AtomicArgument. */
        inline std::string get_name() const { return this->name; }
        /* Checks if this Argument listens to or contains an Argument that listens to the given name. */
        virtual bool has_name(const std::string& name) const = 0;
        /* Checks if this Argument listens to or contains an Argument that listens to the given shortlabel. Note that it throws if used on a Positional. */
        virtual bool has_shortlabel(char shortlabel) const = 0;
        /* Returns the type of the Argument. */
        inline ArgumentType get_arg_type() const { return this->arg_type; }
        /* Returns if this Argument is actually an Atomic argument. */
        inline bool is_atomic() const { return this->arg_type >= ArgumentType::multi_argument; };

        /* Allows ArgumentParser to let each Argument simply parse themselves. Receives a list of strings, one for each space-separated value, out of which the Argument cherry picks its own values and removes those from the list, ready to be parsed by the next one. Can throw ParseException derivates if it failed somehow, but returns the appropriate std::any value otherwise. */
        virtual std::any parse(std::vector<std::string>& raw_args) const = 0;

        /* Swap operator for the Argument class. */
        friend void swap(Argument& a1, Argument& a2);

        /* Allows derived classes of the Argument to copy themselves. */
        virtual Argument* copy() const = 0;

    };
    /* Swap operator for the Argument class. */
    void swap(Argument& a1, Argument& a2) {
        using std::swap;

        swap(a1.arg_type, a2.arg_type);
    }



    /* The AtomicArgument class: forms the baseclass of all elementary, no-children arguments. */
    class AtomicArgument : public Argument {
    protected:
        /* A pointer to the Type used in this AtomicArgument. */
        RuntimeType type;
        
        /* Determines whether or not the AtomicArgument is optional, i.e., the user can omit specifying it. */
        bool optional;
        /* Determines whether or not the AtomicArgument can only be defined once or not. */
        bool singleton;
        /* The description of the AtomicArgument. Will be used to describe the argument when generate_help() is used. Arguments without description will not be displayed there (but will be displayed using generate_usage()). */
        std::string description;
        /* The category of the AtomicArgument. Will be used to group arguments when using generate_help(). Is 'Miscellaneous' by default. */
        std::string category;
        /* Stores the default value set for the AtomicArgument if there is one. Otherwise, will be equal to std::any(nullptr). */
        std::any default_value;

        /* Constructor for the AtomicArgument baseclass, which takes the name of the derived class and its (argument) type. */
        AtomicArgument(ArgumentType arg_type, const std::string& name, const RuntimeType& type) :
            Argument(arg_type, name),
            type(type),
            optional(false),
            singleton(true),
            description(""),
            category("Miscellaneous")
        {
            // Check if the name is legal
            char valid = is_valid_name(this->name);
            if (valid != '\0') { throw IllegalNameException("AtomicArgument", name, valid); }
        }

        /* Constructor for the AtomicArgument baseclass which only takes the name of the derived class. Only used by Flags. */
        AtomicArgument(const std::string& name) :
            Argument(ArgumentType::flag, name),
            type(Bool::runtime()),
            optional(true),
            singleton(true),
            description(""),
            category("Miscellaneous"),
            default_value(false)
        {
            // Check if the name is legal
            char valid = is_valid_name(this->name);
            if (valid != '\0') { throw IllegalNameException("AtomicArgument", name, valid); }
        }

    public:
        /* Virtual destructor of the AtomicArgument class. */
        virtual ~AtomicArgument() = default;

        /* Checks if this AtomicArgument listens to the given name. */
        virtual bool has_name(const std::string& name) const { return this->name == name; }
        /* Returns the name of the type associated with this AtomicArgument. */
        inline RuntimeType get_type() const { return this->type; }

        /* Sets whether or not the AtomicArgument is optional, i.e., can be omitted by the user. Returns a reference to the AtomicArgument to allow chaining. */
        virtual AtomicArgument& set_optional(bool optional) {
            this->optional = optional;
            return *this;
        }
        /* Gets whether or not the AtomicArgument is optional, i.e., can be omitted by the user. */
        inline bool is_optional() const { return this->optional; }

        /* Sets whether or not the AtomicArgument may only be specified once. If allowed to be specified multiple times, the result must be retrieved with Arguments::getp() instead of Arguments::get() (see the ArgumentParser's Wiki for more information). Returns a reference to the AtomicArgument to allow chaining. */
        virtual AtomicArgument& set_singleton(bool is_singleton) {
            this->singleton = is_singleton;
            return *this;
        }
        /* Gets whether or not the AtomicArgument may only be specified once. If allowed to be specified multiple times, the result must be retrieved with Arguments::getp() instead of Arguments::get() (see the ArgumentParser's Wiki for more information). */
        inline bool is_singleton() const { return this->singleton; }

        /* Sets the description of the AtomicArgument. Returns a reference to the AtomicArgument to allow chaining. */
        virtual AtomicArgument& set_description(const std::string& description) {
            this->description = description;
            return *this;
        }
        /* Returns the description of the AtomicArgument. */
        inline std::string get_description() const { return this->description; }

        /* Sets the category of the AtomicArgument. Returns a reference to the AtomicArgument to allow chaining. */
        virtual AtomicArgument& set_category(const std::string& category) {
            this->category = category;
            return *this;
        }
        /* Returns the category of the AtomicArgument. */
        inline std::string get_category() const { return this->category; }

        /* Returns whether or not this AtomicArgument has a default value set. */
        inline bool has_default() const { return this->default_value.has_value(); }
        /* Clears the default value of the AtomicArgument if it had any. Returns a reference to the AtomicArgument to allow chaining. */
        virtual AtomicArgument& clear_default() {
            this->default_value.reset();
            return *this;
        }
        /* Sets the default value of this AtomicArgument. Returns a reference to the AtomicArgument to allow chaining. */
        virtual AtomicArgument& set_default(const std::any& default_value) {
            this->default_value = default_value;
            return *this;
        }
        /* Gets the default value of this AtomicArgument. Note that the returned any object may be unitialized if AtomicArgument::has_default() return false. */
        inline std::any get_default() const { return this->default_value; }

        /* Allows derived classes of the AtomicArgument to copy themselves. */
        virtual AtomicArgument* copy() const = 0;

    };

    /* The Positional class: this is an argument which is position-dependent, and only has a value (not an argument). */
    class Positional : public AtomicArgument {
    private:
        /* The index of the Positional. */
        size_t index;

        /* Constructor for a Positional, which takes the name, the index for it and its type as RuntimeType. */
        Positional(const std::string& name, size_t index, const RuntimeType& type) :
            AtomicArgument(ArgumentType::positional, name, type),
            index(index)
        {}

        /* Declare the ArgumentParser as friend so it can call the constructor. */
        friend class ArgumentParser;
        /* Declare the MultiArgument as friend so it can call the constructor. */
        friend class MultiArgument;
    
    public:
        /* Returns always false, as Positionals don't have shortlabels. If the DEBUG-macro is defined, prints a warning to stderr. */
        virtual bool has_shortlabel(char shortlabel) const {
            #ifdef DEBUG
            std::cerr << "[WARNING] Called Argument::has_shortlabel() on a Positional." << std::endl;
            #endif
            return false;
        };

        /* Sets whether or not the Positional is optional, i.e., can be omitted by the user. Returns a reference to the Positional to allow chaining. */
        virtual Positional& set_optional(bool optional) { return (Positional&) AtomicArgument::set_optional(optional); }
        /* Sets whether or not the Positional may only be specified once. If allowed to be specified multiple times, the result must be retrieved with Arguments::getp() instead of Arguments::get() (see the ArgumentParser's Wiki for more information). Returns a reference to the Positional to allow chaining. */
        virtual Positional& set_singleton(bool is_singleton) { return (Positional&) AtomicArgument::set_singleton(is_singleton); }
        /* Sets the description of the Positional. Returns a reference to the Positional to allow chaining. */
        virtual Positional& set_description(const std::string& description) { return (Positional&) AtomicArgument::set_description(description); }
        /* Sets the category of the Positional. Returns a reference to the Positional to allow chaining. */
        virtual Positional& set_category(const std::string& category) { return (Positional&) AtomicArgument::set_category(category); }

        /* Clears the default value of the Positional if it had any. Returns a reference to the Positional to allow chaining. */
        virtual Positional& clear_default() { return (Positional&) AtomicArgument::clear_default(); }
        /* Sets the default value of this Positional. Returns a reference to the Positional to allow chaining. */
        virtual Positional& set_default(const std::any& default_value) { return (Positional&) AtomicArgument::set_default(default_value); }

        /* Attempts to parse the appropriate Positional from the given list of CLI arguments. Doesn't it remove the value when found (in contradication to Options and Flags) to keep their index encoded. Throws an appropriate ParseException derivative if it wasn't successul. Note that we assume all Options & Flags have already been parsed, and only Positionals are left. */
        virtual std::any parse(std::vector<std::string>& raw_args) const {
            if (this->index >= raw_args.size()) {
                if (this->optional) {
                    // Return an empty any, to indicate we succeeded but still didn't parse anything
                    return std::any();
                } else {
                    // Throw an error to show this message cannot be missed
                    throw MissingMandatoryException(this->name);
                }
            }

            // Otherwise, try to parse
            std::any result;
            try {
                result = this->type.parse_func(std::vector<std::string>(raw_args.begin() + this->index, raw_args.end()));
            } catch (TypeParseException& e) {
                e.msg = e.generate_message(this->type.type_name, this->name, e.message);
                throw;
            }

            // Success!
            return result;
        }

        /* Allows the Positional to copy itself polymorphically. */
        virtual Positional* copy() const { return new Positional(*this); }

    };

    /* The Option class: an argument with a label and a value. */
    class Option : public AtomicArgument {
    private:
        /* Stores the shortlabel given to the Option. */
        char shortlabel;

        /* Constructor for the Option class, which takes the name, the shortlabel for it and its type as RuntimeType. */
        Option(const std::string& name, char shortlabel, const RuntimeType& type) :
            AtomicArgument(ArgumentType::option, name, type),
            shortlabel(shortlabel)
        {
            // Check if the shortlabel is legal
            if (!is_valid_shortlabel(shortlabel)) { throw IllegalShortlabelException("Option", shortlabel); }
        }

        /* Declare the ArgumentParser as friend so it can call the constructor. */
        friend class ArgumentParser;
        /* Declare the MultiArgument as friend so it can call the constructor. */
        friend class MultiArgument;
    
    public:
        /* Returns the shortlabel of the Option. */
        inline size_t get_shortlabel() const { return this->shortlabel; }
        /* Returns whether this Option has given shortlabel. */
        virtual bool has_shortlabel(char shortlabel) const {
            return this->shortlabel == shortlabel;
        };

        /* Sets whether or not the Option is optional, i.e., can be omitted by the user. Returns a reference to the Option to allow chaining. */
        virtual Option& set_optional(bool optional) { return (Option&) AtomicArgument::set_optional(optional); }
        /* Sets whether or not the Option may only be specified once. If allowed to be specified multiple times, the result must be retrieved with Arguments::getp() instead of Arguments::get() (see the ArgumentParser's Wiki for more information). Returns a reference to the Option to allow chaining. */
        virtual Option& set_singleton(bool is_singleton) { return (Option&) AtomicArgument::set_singleton(is_singleton); }
        /* Sets the description of the Option. Returns a reference to the Option to allow chaining. */
        virtual Option& set_description(const std::string& description) { return (Option&) AtomicArgument::set_description(description); }
        /* Sets the category of the Option. Returns a reference to the Option to allow chaining. */
        virtual Option& set_category(const std::string& category) { return (Option&) AtomicArgument::set_category(category); }

        /* Clears the default value of the Option if it had any. Returns a reference to the Option to allow chaining. */
        virtual Option& clear_default() { return (Option&) AtomicArgument::clear_default(); }
        /* Sets the default value of this Option. Returns a reference to the Option to allow chaining. */
        virtual Option& set_default(const std::any& default_value) { return (Option&) AtomicArgument::set_default(default_value); }

        /* Attempts to parse the appropriate Option from the given list of CLI arguments. Removes the opt flag & value when found. Throws an appropriate ParseException derivative if it wasn't successul. */
        virtual std::any parse(std::vector<std::string>& raw_args) const {
            // Loop through all raw arguments to find any matching ourselves
            for (size_t i = 0; i < raw_args.size(); i++) {
                std::string arg = raw_args.at(i);
                if ((arg.size() == 2 && arg[0] == '-' && arg[1] == this->shortlabel) || (arg == "--" + this->name)) {
                    // It's this option! See if there are any values
                    if (this->type.n_values > raw_args.size() - (i + 1)) {
                        throw NotEnoughValuesException(this->type.type_name, this->type.n_values, raw_args.size() - (i + 1), this->name, this->shortlabel);
                    }

                    // If there are, try to parse it
                    try {
                        return this->type.parse_func(std::vector<std::string>(raw_args.begin() + i + 1, raw_args.end()));
                    } catch (TypeParseException& e) {
                        e.msg = e.generate_message(this->type.type_name, this->name, e.message);
                        throw;
                    }
                }
            }
            
            // If not found, check if that's okay
            if (this->optional) {
                // Return empty to single it wasn't here
                return std::any();
            } else {
                // Throw a lean mean error
                throw MissingMandatoryException(this->name, this->shortlabel);
            }
        }

        /* Allows the Option to copy itself polymorphically. */
        virtual Option* copy() const { return new Option(*this); }

    };

    /* The Flag class: an argument with only a label, not a value. This can be used as a way to pass booleans. */
    class Flag : public AtomicArgument {
    private:
        /* Stores the shortlabel given to the Option. */
        char shortlabel;

        /* Constructor for the Flag class, which takes the name and the shortlabel of the Argument. */
        Flag(const std::string& name, char shortlabel) :
            AtomicArgument(name),
            shortlabel(shortlabel)
        {
            // Check if the shortlabel is legal
            if (!is_valid_shortlabel(shortlabel)) { throw IllegalShortlabelException("Flag", shortlabel); }
        }

        /* Declare the ArgumentParser as friend so it can call the constructor. */
        friend class ArgumentParser;
        /* Declare the MultiArgument as friend so it can call the constructor. */
        friend class MultiArgument;
    
    public:
        /* Returns the shortlabel of the Flag. */
        inline size_t get_shortlabel() const { return this->shortlabel; }
        /* Returns whether this Flag has given shortlabel. */
        virtual bool has_shortlabel(char shortlabel) const {
            return this->shortlabel == shortlabel;
        };

        /* This function is disabled for Flags, since they're always optional. Throws a ValueMismatchException when called. */
        virtual Flag& set_optional(bool optional) { throw ValueMismatchException("Flag::set_optional()", this->name); }
        /* This function is disabled for Flags, since it doesn't make sense to specify flags multiple times. Throws a ValueMismatchException when called. */
        virtual Flag& set_singleton(bool is_singleton) { throw ValueMismatchException("Flag::set_singleton()", this->name); }
        /* Sets the description of the Flag. Returns a reference to the Flag to allow chaining. */
        virtual Flag& set_description(const std::string& description) { return (Flag&) AtomicArgument::set_description(description); }
        /* Sets the category of the Flag. Returns a reference to the Flag to allow chaining. */
        virtual Flag& set_category(const std::string& category) { return (Flag&) AtomicArgument::set_category(category); }

        /* This function is disabled for Flags, since Flag's value must always be set to false or true. Throws a ValueMismatchException when called. */
        virtual Flag& clear_default() { throw ValueMismatchException("Flag::clear_default()", this->name); }
        /* Sets the Flag's value if the user doesn't specify it. Is true by default, but might make sense to set to false. Throws an Returns a reference to the Option to allow chaining. */
        virtual Flag& set_default(const std::any& default_value) {
            if (default_value.type() != typeid(bool) && default_value.type() != typeid(const bool)) { throw IllegalDefaultValueException("Flag::set_default()", this->name, default_value); }
            return (Flag&) AtomicArgument::set_default(default_value);
        }

        /* Attempts to parse the appropriate Flag from the given list of CLI arguments. Removes the opt flag when found. Throws an appropriate ParseException derivative if it wasn't successul. */
        virtual std::any parse(std::vector<std::string>& raw_args) const {
            // Loop through all raw arguments to find any matching ourselves
            for (size_t i = 0; i < raw_args.size(); i++) {
                std::string arg = raw_args.at(i);
                if ((arg.size() == 2 && arg[0] == '-' && arg[1] == this->shortlabel) || (arg == "--" + this->name)) {
                    // It's this Flag! Simply return true
                    return std::any(true);
                }
            }
            
            // If not found, return false
            std::any(false);
        }

        /* Allows the Flag to copy itself polymorphically. */
        virtual Flag* copy() const { return new Flag(*this); }

    };



    /* The MultiArgument class: forms the baseclass for all arguments which can have children or nested arguments. */
    class MultiArgument : public Argument {
    protected:
        /* Reference to the top-level MultiArgument, so that we can check for things before adding them. */
        MultiArgument* root;
        /* A list of all nested arguments. */
        std::vector<Argument*> args;

        /* Constructor for the MultiArgument which takes a name and an ArgumentType. */
        MultiArgument(ArgumentType arg_type, MultiArgument* root, const std::string& name) :
            Argument(arg_type, name),
            root(root)
        {}

    public:
        /* Copy constructor for the MultiArgument class. */
        MultiArgument(const MultiArgument& other) :
            Argument(other),
            root(root)
        {
            for (size_t i = 0; i < other.args.size(); i++) {
                this->args.push_back(other.args.at(i)->copy());
            }
        }
        /* Move constructor for the MultiArgument class. */
        MultiArgument(MultiArgument&& other) :
            Argument(other),
            root(root),
            args(other.args)
        {
            for (size_t i = 0; i < other.args.size(); i++) {
                other.args.at(i) = nullptr;
            }
        }
        /* Destructor for the MultiArgument class. */
        virtual ~MultiArgument() {
            for (size_t i = 0; i < this->args.size(); i++) {
                if (this->args.at(i) != nullptr) { delete this->args.at(i); }
            }
        }

        /* Returns if this argument contains an argument with the given name. Only used for AtomicArguments, so does not return true on any MultiArgument names. */
        virtual bool has_name(const std::string& name) const {
            for (size_t i = 0; i < this->args.size(); i++) {
                if (this->args.at(i)->has_name(name)) { return true; }
            }
            return false;
        }
        /* Returns if this argument contains an argument with the given shortlabel. */
        virtual bool has_shortlabel(char shortlabel) const {
            for (size_t i = 0; i < this->args.size(); i++) {
                if (this->args.at(i)->has_shortlabel(shortlabel)) { return true; }
            }
            return false;
        }

        /* Adds a new positional argument to the parser of Type T. Note that the order of defining the Positionals determines the order of parsing them. A reference to the new parameter is returned to change its properties. */
        template <typename T, typename = std::enable_if_t<std::is_base_of<BaseType, T>::value> >
        Positional& add_positional(const std::string& name) {
            const std::string context = "MultiArgument::add_positional()";

            // Check if n_values != 0
            if (T::n_values == 0) { throw IllegalNValuesException(context, T::n_values); }

            // Check if the name exists
            if (this->root->has_name(name)) {
                throw DuplicateNameException(context, name);
            }

            // Check how many Positionals there are in total (deep search from the root)
            size_t n_positionals = this->root->deepsearch<Positional>().size();

            // Create a new Positional object with the index = the number of n_positionals
            Positional* p = new Positional(name, n_positionals, T::runtime());
            this->args.push_back(p);
            
            // Return it
            return *p;
        }
        /* Adds a new optional argument to the parser (an argument with a label and a value) of DerivedType T. A reference to the new parameter is returned to change its properties. */
        template <typename T, typename = std::enable_if_t<std::is_base_of<BaseType, T>::value> >
        Option& add_option(char shortlabel, const std::string& name) {
            const std::string context = "MultiArgument::add_option()";

            // Check if n_values != 0
            if (T::n_values == 0) { throw IllegalNValuesException(context, T::n_values); }

            // Check if either the name or the shortlabel exists
            if (this->root->has_name(name)) {
                throw DuplicateNameException(context, name);
            } else if (this->root->has_shortlabel(shortlabel)) {
                throw DuplicateShortlabelException(context, shortlabel);
            }

            // Create a new Option object and add it
            Option* o = new Option(name, shortlabel, T::runtime());
            this->args.push_back(o);

            // Return it
            return *o;
        }
        /* Adds a new flag argument to the parser (an argument with a label, but no value). A reference to the new parameter is returned to change its properties. */
        Flag& add_flag(char shortlabel, const std::string& name) {
            const std::string context = "MultiArgument::add_flag()";

            // Check if either the name or the shortlabel exists
            if (this->root->has_name(name)) {
                throw DuplicateNameException(context, name);
            } else if (this->root->has_shortlabel(shortlabel)) {
                throw DuplicateShortlabelException(context, shortlabel);
            }

            // Create a new Flag object and add it
            Flag* f = new Flag(name, shortlabel);
            this->args.push_back(f);

            // Return it
            return *f;
        }
        /* Adds a new IncludedGroup to the parser (a collection of arguments for which holds that if one appears, all of them must appear). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. A reference to the new group is returned to change its properties and add children. */
        IncludedGroup& add_included(const std::string& name) {
            const std::string context = "MultiArgument::add_included()";

            // Check if the given name exists by getting a list of all MultiArguments (nested) from the root and checking their name
            for (MultiArgument* marg : this->root->deepsearch<MultiArgument>()) {
                if (marg->get_name() == name) {
                    throw DuplicateNameException(context, name);
                }
            }

            // Create a new IncludedGroup object
            IncludedGroup* ig = new IncludedGroup(this->root, name);
            this->args.push_back(ig);
            
            // Return it
            return *ig;
        }
        /* Adds a new ExcludedGroup to the parser (a collection of arguments for which holds that if one appears, none other can appear). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. A reference to the new group is returned to change its properties and add children. */
        ExcludedGroup& add_excluded(const std::string& name) {
            const std::string context = "MultiArgument::add_excluded()";

            // Check if the given name exists by getting a list of all MultiArguments (nested) from the root and checking their name
            for (MultiArgument* marg : this->root->deepsearch<MultiArgument>()) {
                if (marg->get_name() == name) {
                    throw DuplicateNameException(context, name);
                }
            }

            // Create a new ExcludedGroup object
            ExcludedGroup* eg = new ExcludedGroup(this->root, name);
            this->args.push_back(eg);
            
            // Return it
            return *eg;
        }
        /* Adds a new RequiredGroup to the parser (a collection of arguments for which holds that an argument can only appear if the previous appears). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. A reference to the new group is returned to change its properties and add children. */
        RequiredGroup& add_required(const std::string& name) {
            const std::string context = "MultiArgument::add_required()";

            // Check if the given name exists by getting a list of all MultiArguments (nested) from the root and checking their name
            for (MultiArgument* marg : this->root->deepsearch<MultiArgument>()) {
                if (marg->get_name() == name) {
                    throw DuplicateNameException(context, name);
                }
            }

            // Create a new ExcludedGroup object
            RequiredGroup* rg = new RequiredGroup(this->root, name);
            this->args.push_back(rg);
            
            // Return it
            return *rg;
        }

        /* Returns a reference to an AtomicArgument with the given name. The return type can optionally be specified to make working with this a little easier. Throws an UnknownNameException if no such argument exists, and an ArgTypeMismatchException if the given type does not match that of the found argument. */
        template <class T = AtomicArgument, typename = std::enable_if_t<std::is_base_of<AtomicArgument, T>::value> >
        T& get_atomic(const std::string& name) const {
            const std::string context = "MultiArgument::get_atomic";

            for (size_t i = 0; i < this->args.size(); i++) {
                Argument* arg = this->args.at(i);
                if (dynamic_cast<AtomicArgument*>(arg) && arg->has_name(name)) {
                    // It's an atomic argument
                    if (!dynamic_cast<T*>(arg)) { throw ArgTypeMismatchException(context, name, arg->get_arg_type(), T::type_name); }
                    return (T&) (*arg);
                } else if (dynamic_cast<MultiArgument*>(arg) && arg->has_name(name)) {
                    // The MultiArgument contains our target, so search that one
                    MultiArgument* marg = (MultiArgument*) arg;
                    return marg->get_atomic<T>(name);
                }
            }

            // If not found, throw the appropraite exception
            throw UnknownNameException(context, name);
        }
        /* Returns a reference to a MultiArgument with the given name. The return type can optionally be specified to make working with this a little easier. Throws an UnknownNameException if no such argument exists, and a ArgTypeMismatchException if the given type does not match that of the found argument. */
        template <class T = AtomicArgument, typename = std::enable_if_t<std::is_base_of<MultiArgument, T>::value> >
        T& get_multi(const std::string& name) const {
            const std::string context = "MultiArgument::get_multi";

            for (size_t i = 0; i < this->args.size(); i++) {
                Argument* arg = this->args.at(i);
                // Skip all non-MultiArguments
                if (!dynamic_cast<MultiArgument*>(arg)) { continue; }

                // Otherwise, check if this arg is the one
                if (arg->get_name() == name) {
                    // It is, now check if the type is correct
                    if (!dynamic_cast<T*>(arg)) { throw ArgTypeMismatchException(context, name, arg->get_arg_type(), T::type_name); }
                    return (T&) (*arg);
                }

                // Otherwise, check if this arg has the one
                MultiArgument* marg = (MultiArgument*) arg;
                try {
                    return marg->get_multi<T>(name);
                } catch (UnknownNameException& e) {
                    // Wasn't it, so simply skip
                }
            }

            // If not found, throw the appropraite exception
            throw UnknownNameException(context, name);
        }
        /* Returns a vector containing all Arguments of the specified type (or derivatives thereof) in this MultiArgument and any nested ones (deep search). Use T = Argument to simply get a list of ALL Arguments instead of filtering others out. This vector can then be iterated over, if desired. */
        template <class T = Argument, typename = std::enable_if_t<std::is_base_of<Argument, T>::value> >
        std::vector<T*> deepsearch() const {
            std::vector<T*> result;

            // Set the reversed list as our search space
            std::vector<Argument*> to_do = std::vector<Argument*>(this->args.rbegin(), this->args.rend());
            while (to_do.size() > 0) {
                // Get the Argument we want to investigate
                Argument* tail = to_do.at(to_do.size() - 1);
                to_do.pop_back();

                // If the Argument is of the expected type, add it to the list
                if (dynamic_cast<T*>(tail)) {
                    result.push_back((T*) tail);
                }

                // Regardless, if the Argument is a MultiArgument (or derived), add it's children to the list of to-do (depth-first)
                if (dynamic_cast<MultiArgument*>(tail)) {
                    std::vector<T*> nested_result = ((MultiArgument*) tail)->deepsearch<T>();
                    to_do.insert(to_do.end(), nested_result.rbegin(), nested_result.rend());
                }
            }

            // Result should now contain a list of all Arguments that derive of type T, so return it!
            return result;
        }

        /* Returns the number of arguments stored in this MultiArgument. */
        inline size_t size() const { return this->args.size(); }

        /* Returns an iterator pointing to the beginning of the internal vector object. */
        inline std::vector<Argument*>::const_iterator begin() const { return this->args.begin(); }
        /* Returns an iterator pointing to the beginning of the internal vector object. */
        inline std::vector<Argument*>::const_iterator cbegin() const { return this->args.cbegin(); }
        /* Returns an iterator pointing to the end of the internal vector object. */
        inline std::vector<Argument*>::const_iterator end() const { return this->args.end(); }
        /* Returns an iterator pointing to the end of the internal vector object. */
        inline std::vector<Argument*>::const_iterator cend() const { return this->args.cend(); }

        /* Copy assignment operator for the MultiArgument class. */
        virtual MultiArgument& operator=(const MultiArgument& other) { return *this = MultiArgument(other); }
        /* Move assignment operator for the MultiArgument class. */
        virtual MultiArgument& operator=(MultiArgument&& other) {
            if (this != &other) { swap(*this, other); }
            return *this;
        }
        /* Swap operator for the MultiArgument class. */
        friend void swap(MultiArgument& ma1, MultiArgument& ma2);

        /* Allows derived classes of the AtomicArgument to copy themselves. */
        virtual MultiArgument* copy() const { return new MultiArgument(*this); };

    };
    /* Swap operator for the MultiArgument class. */
    void swap(MultiArgument& ma1, MultiArgument& ma2) {
        using std::swap;

        swap((Argument&) ma1, (Argument&) ma2);
        swap(ma1.root, ma2.root);
        swap(ma1.args, ma2.args);
    }

    /* The IncludedGroup class: when the user specifies one of this classes' children, he or she must specify all of them. */
    class IncludedGroup : public MultiArgument {

    };

    /* The ExcludedGroup class: when the user specifies one of this classes' children, he or she cannot specify any other. */
    class ExcludedGroup : public MultiArgument {

    };

    /* The RequiredGroup class: the user must specify the preceding argument before the following argument can be given. */
    class RequiredGroup : public MultiArgument {

    };





    /******************** THE RESULTING ARGUMENTS DICT ********************/

    /* The Arguments class, which will be used to let the user interact with all the parsed elements. */
    class Arguments {
    private:
        /* Maps all arguments to true if they exist. */
        std::unordered_map<std::string, Argument*> exists_map;
        /* Maps all arguments to a value if they have one. */
        std::unordered_map<std::string, std::any> value_map;
        /* Specifies for all arguments whether they are given or not. */
        std::unordered_map<std::string, bool> given_map;

        /* Adds an argument with a value. */
        template <class T>
        void add_arg(const T& arg, const std::any& value, bool given) {
            // Register the argument in the exists_map if we didn't already
            if (this->exists_map.find(arg.get_name()) == this->exists_map.end()) {
                this->exists_map.insert(std::make_pair(arg.get_name(), arg.copy()));
                this->given_map.insert(std::make_pair(arg.get_name(), given));

                // Then, add either as-is or wrapped in a vector, depending on if the argument allows to be specified multiple times
                if (!arg.get_any_number()) {
                    this->value_map.insert(std::make_pair(arg.get_name(), value));
                } else {
                    this->value_map.insert(std::make_pair(arg.get_name(), std::any(std::vector<std::any>({ value }))));
                }
            } else if (arg.get_any_number()) {
                // Append the value in the value map
                std::vector<std::any> new_vec = std::vector<std::any>(std::any_cast<std::vector<std::any>>(this->value_map.at(arg.get_name())));
                new_vec.push_back(value);
                this->value_map.at(arg.get_name()) = std::any(new_vec);
            } else {
                // The argument already occurred, let's throw an exception (note that this can only occur for Options)
                throw DuplicateArgumentException(arg.get_name(), ((const Option&) arg).get_shortlabel());
            }
        }

        /* Declare the ArgumentParser as friend. */
        friend class ArgumentParser;

    public:
        /* Default constructor for the Arguments class. Only available from the ArgumentParser. */
        Arguments()
        {}
        /* Copy constructor for the Arguments class. */
        Arguments(const Arguments& other) :
            value_map(other.value_map),
            given_map(other.given_map)
        {
            // Copy everything from the exists_map
            for (std::pair<std::string, Argument*> p : other.exists_map) {
                this->exists_map.insert(p);
            }
        }
        /* Move constructor for the Arguments class. */
        Arguments(Arguments&& other) :
            value_map(std::move(other.value_map)),
            given_map(std::move(other.given_map))
        {
            // Steal everything from the exists_map
            for (std::pair<std::string, Argument*> p : other.exists_map) {
                this->exists_map.insert(p);
                p.second = nullptr;
            }
        }
        /* Destructor for the Arguments class. */
        ~Arguments() {
            for (std::pair<std::string, Argument*> p : this->exists_map) {
                // Only delete if not stolen
                if (p.second != nullptr) { delete p.second; }
            }
        }

        /* Returns true if this object contains an argument with the given name. */
        inline bool contains(std::string name) const {
            return this->exists_map.find(name) != this->exists_map.end();
        }
        /* Returns true if the user explicitly specified the argument. Otherwise, the argument does not exist or was a default argument. */
        bool is_given(std::string name) const {
            std::unordered_map<std::string, bool>::const_iterator iter = this->given_map.find(name);
            return iter != this->given_map.end() && (*iter).second;
        }

        /* Returns the argument with given name as the given type. Will throw an TypeMismatchException if the type entered here does not match the type entered when registring the argument. */
        template<class T>
        T get(std::string name) const {
            // The context for this function
            const std::string context = "Arguments::get<" + std::string(type_name<T>::value) + ">() const";

            // Try to find the uid in the value map
            std::unordered_map<std::string, std::any>::const_iterator iter = this->value_map.find(name);
            if (iter == this->value_map.end()) {
                // Check if it's because of it not being registered or it not having a value
                if (this->exists_map.find(name) == this->exists_map.end()) {
                    throw UnknownNameException(context, name);
                } else {
                    throw ValueMismatchException(context, name);
                }
            }

            // Check if the type is correct
            std::any value = (*iter).second;
            Argument* arg = this->exists_map.at(name);
            if (dynamic_cast<Positional*>(arg)) {
                // Check as Positional
                Positional* pos = (Positional*) arg;

                // Check if pos' type checks out
                if (pos->get_type_hash() != typeid(T).hash_code()) {
                    throw TypeMismatchException(context, pos->get_type_name(), type_name<T>::value);
                }

                // If the Positional can occur multiple times, error since they must use the other get()
                if (pos->get_any_number()) {
                    throw AnyNumberMismatchException(context, pos->get_name());
                }
            } else {
                // Check as Option
                Option* opt = (Option*) arg;

                // Check if opt's type checks out
                if (opt->get_type_hash() != typeid(T).hash_code()) {
                    throw TypeMismatchException(context, opt->get_type_name(), type_name<T>::value);
                }
            }

            // If the casted version happens to be a char array (constant or not), and T is a string, then return as string
            if (typeid(T).hash_code() == typeid(std::string).hash_code()) {
                if (value.type().hash_code() == typeid(char*).hash_code()) {
                    value = std::any(std::string(std::any_cast<char*>(value)));
                } else if (value.type().hash_code() == typeid(const char*).hash_code()) {
                    value = std::any(std::string(std::any_cast<const char*>(value)));
                }
            }

            // Everything checks out, so return the any casted version
            return std::any_cast<T>(value);
        }
        /* Retrusn the argument with given name as the given type, but assumes that the argument that we request can be given more than once. The value is therefore repeated as a vector of given type, where each element is one of the times it was specified (in order). */
        template <class T>
        std::vector<T> getp(std::string name) const {
            // The context for this function
            const std::string context = "Arguments::getp<" + std::string(type_name<T>::value) + ">() const";

            // Try to find the uid in the value map
            std::unordered_map<std::string, std::any>::const_iterator iter = this->value_map.find(name);
            if (iter == this->value_map.end()) {
                // Check if it's because of it not being registered or it not having a value
                if (this->exists_map.find(name) == this->exists_map.end()) {
                    throw UnknownNameException(context, name);
                } else {
                    throw ValueMismatchException(context, name);
                }
            }

            // Check if the type is correct
            std::any value = (*iter).second;
            Argument* arg = this->exists_map.at(name);
            if (dynamic_cast<Positional*>(arg)) {
                // Check as Positional
                Positional* pos = (Positional*) arg;

                // Check if pos' type checks out
                if (pos->get_type_hash() != typeid(T).hash_code()) {
                    throw TypeMismatchException(context, pos->get_type_name(), type_name<T>::value);
                }

                // If pos does not repeat, wrap the value in a vector and return
                if (!pos->get_any_number()) {
                    return std::vector<T>({ std::any_cast<T>(value) });
                }
            } else {
                // Check as Option
                Option* opt = (Option*) arg;

                // Check if opt's type checks out
                if (opt->get_type_hash() != typeid(T).hash_code()) {
                    throw TypeMismatchException(context, opt->get_type_name(), type_name<T>::value);
                }
            }

            // Everything checks out, so return the any casted version
            std::vector<std::any> any_values = std::any_cast<std::vector<std::any>>(value);
            std::vector<T> values;
            values.resize(any_values.size());
            for (size_t i = 0; i < any_values.size(); i++) {
                std::any val = any_values.at(i);

                // If the casted version happens to be a char array (constant or not), and T is a string, then return as string
                if (typeid(T).hash_code() == typeid(std::string).hash_code()) {
                    if (val.type().hash_code() == typeid(char*).hash_code()) {
                        val = std::any(std::string(std::any_cast<char*>(val)));
                    } else if (val.type().hash_code() == typeid(const char*).hash_code()) {
                        val = std::any(std::string(std::any_cast<const char*>(val)));
                    }
                }

                values.at(i) = std::any_cast<T>(val);
            }
            return values;
        } 

        /* Returns the index of given argument if it is a Positional. Throws PositionalMismatchException otherwise. */
        size_t get_index(std::string name) const {
            const std::string context = "Arguments::get_index() const";

            // Try to find uid
            std::unordered_map<std::string, Argument*>::const_iterator iter = this->exists_map.find(name);
            if (iter == this->exists_map.end()) {
                throw UnknownNameException(context, name);
            }

            // Check if it has an index, i.e., is a Positional
            if (!dynamic_cast<Positional*>((*iter).second)) { throw PositionalMismatchException(context, name); }

            // Return the index
            return ((Positional*) (*iter).second)->get_index();
        }
        /* Returns the shortlabel of given argument if it is an Option or a Flag. Throws OptionMismatchException otherwise. */
        char get_shortlabel(std::string name) const {
            const std::string context = "Arguments::get_shortlabel() const";

            // Try to find uid
            std::unordered_map<std::string, Argument*>::const_iterator iter = this->exists_map.find(name);
            if (iter == this->exists_map.end()) {
                throw UnknownNameException(context, name);
            }

            // Check if it has an index, i.e., is a Flag (or derived)
            if (!dynamic_cast<Flag*>((*iter).second)) { throw FlagMismatchException(context, name); }

            // Return the index
            return ((Flag*) (*iter).second)->get_shortlabel();
        }

        /* Copy assignment operator for the Arguments class. */
        inline Arguments& operator=(const Arguments& other) { return *this = Arguments(other); }
        /* Move assignment operator for the Arguments class. */
        Arguments& operator=(Arguments&& other) {
            if (this != &other) {
                swap(*this, other);
            }
            return *this;
        }
        /* Swap operator for the the Arguments class. */
        friend void swap(Arguments& a1, Arguments& a2);
    };

    /* Swap operator for the the Arguments class. */
    void swap(Arguments& a1, Arguments& a2) {
        using std::swap;

        swap(a1.exists_map, a2.exists_map);
        swap(a1.value_map, a2.value_map);
        swap(a1.given_map, a2.given_map);
    }
    /* Adds an argument without value, aka, a Flag (full specialization for the add_arg member). */
    template <>
    void Arguments::add_arg<Flag>(const Flag& arg, const std::any&, bool given) {
        // If it was already given, throw
        if (this->exists_map.find(arg.get_name()) != this->exists_map.end()) {
            throw DuplicateArgumentException(arg.get_name(), arg.get_shortlabel());
        }

        // The argument always occurs if it's registered
        this->exists_map.insert(std::make_pair(arg.get_name(), arg.copy()));
        this->given_map.insert(std::make_pair(arg.get_name(), given));
    }





    /******************** THE ARGUMENTPARSER CLASS ITSELF ********************/

    /* The ArgumentParser class itself. */
    class ArgumentParser {
    private:
        /* Returns whether given string is an option or not. */
        inline static bool is_option(const char* text) {
            return (text[0] == '-' && text[1] == '-' && text[2] != '\0') || (text[0] == '-' && text[1] != '\0');
        }

        /* Registers a parsed Argument. Based on it's type, it is either registered with value or without. Also note that this resolves dependencies, and so may throw dependency-related exceptions. */
        template <typename T>
        static void register_arg(Arguments& args, std::vector<Argument*>& mandatory, std::vector<Argument*>& excluded, T* arg, const std::any& value) {
            // First, check if it is allowed to occur
            if (std::find(excluded.begin(), excluded.end(), (Argument*) arg) != excluded.end()) {
                throw ExcludedArgumentException(arg->get_name(), ((Flag*) arg)->get_shortlabel());
            }

            // Add the value
            args.add_arg(*arg, value, true);

            // Then, process include relationships
            for (Argument* a : arg->relations.at(ArgumentRelation::include)) {
                // If the argument doesn't occur in args yet and not in mandatory, add it in mandatory
                if (!args.contains(a->get_name()) && std::find(mandatory.begin(), mandatory.end(), a) == mandatory.end()) {
                    mandatory.push_back(a);
                }
            }

            // Process exclude relationships
            for (Argument* a : arg->relations.at(ArgumentRelation::exclude)) {
                // If the argument doesn't occur in args yet and not in mandatory, add it in mandatory
                if (!args.contains(a->get_name()) && std::find(excluded.begin(), excluded.end(), a) == excluded.end()) {
                    excluded.push_back(a);
                }
            }

            // Finally, process require relationships
            /* TBD */

            // Done
        }

        /* Stores all Positionals in the parser. */
        MultiArgument args;

    public:
        /* Stores whether or not to automatically handle help. */
        bool auto_help;

        /* Adds a new positional argument to the parser of Type T. Note that the order of defining the Positionals determines the order of parsing them. A reference to the new parameter is returned to change its properties. */
        template <typename T, typename = std::enable_if_t<std::is_base_of<BaseType, T>::value> >
        Positional& add_positional(const std::string& name) {
            const std::string context = "ArgumentParser::add_positional()";

            // Check if n_values != 0 to avoid having confusing contexts
            if (T::n_values == 0) { throw IllegalNValuesException(context, T::n_values); }

            // Simply call the function of the nested MultiArgument
            return args.add_positional<T>(name);
        }
        /* Adds a new optional argument to the parser (an argument with a label and a value) of DerivedType T. A reference to the new parameter is returned to change its properties. */
        template <typename T, typename = std::enable_if_t<std::is_base_of<BaseType, T>::value> >
        Option& add_option(char shortlabel, const std::string& name) {
            const std::string context = "ArgumentParser::add_option()";

            // Check if n_values != 0 to avoid having confusing contexts
            if (T::n_values == 0) { throw IllegalNValuesException(context, T::n_values); }

            // Simply call the function of the nested MultiArgument
            return args.add_option<T>(shortlabel, name);
        }
        /* Adds a new flag argument to the parser (an argument with a label, but no value). A reference to the new parameter is returned to change its properties. */
        inline Flag& add_flag(char shortlabel, const std::string& name) { return args.add_flag(shortlabel, name); }
        /* Adds a new IncludedGroup to the parser (a collection of arguments for which holds that if one appears, all of them must appear). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. A reference to the new group is returned to change its properties and add children. */
        inline IncludedGroup& add_included(const std::string& name) { return args.add_included(name); }
        /* Adds a new ExcludedGroup to the parser (a collection of arguments for which holds that if one appears, none other can appear). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. A reference to the new group is returned to change its properties and add children. */
        inline ExcludedGroup& add_excluded(const std::string& name) { return args.add_excluded(name); }
        /* Adds a new RequiredGroup to the parser (a collection of arguments for which holds that an argument can only appear if the previous appears). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. A reference to the new group is returned to change its properties and add children. */
        inline RequiredGroup& add_required(const std::string& name) { return args.add_required(name); }

        /* Returns a reference to an AtomicArgument with the given name. The return type can optionally be specified to make working with this a little easier. Throws an UnknownNameException if no such argument exists, and a TypeMismatchException if the given type does not match that of the found argument. */
        template <class T = AtomicArgument, typename = std::enable_if_t<std::is_base_of<AtomicArgument, T>::value> >
        inline T& get_atomic(const std::string& name) const { return this->args.get_atomic<T>(name); }
        /* Returns a reference to a MultiArgument with the given name. The return type can optionally be specified to make working with this a little easier. Throws an UnknownNameException if no such argument exists, and a TypeMismatchException if the given type does not match that of the found argument. */
        template <class T = AtomicArgument, typename = std::enable_if_t<std::is_base_of<MultiArgument, T>::value> >
        inline T& get_multi(const std::string& name) const { return this->args.get_multi<T>(name); }

        /* Checks if all arguments are valid, i.e., resolves relational dependencies, optional check for positionals, etc. Throws an appropriate ProgrammingError when not. */
        void validate(const std::string& context = "ArgumentParser::validate() const") const {
            // First, check if the optional positionals are all in the back and, if there's a Positional with any_number, it's the only one and at the back.
            bool optional = false;
            size_t pos_i = 0;
            for (Argument* a : this->args) {
                if (dynamic_cast<Positional*>(a)) {
                    Positional* p = (Positional*) a;

                    // Check if the optional order is correct
                    if (!optional) {
                        if (p->optional) {
                            optional = true;
                        }
                    } else {
                        if (!p->optional) {
                            throw OptionalPositionalException(context, p->get_name(), p->get_index());
                        }
                    }

                    // Check if the any_number is correct
                    if (++pos_i != this->n_positionals && p->any_number) {
                        throw AnyNumberPositionalException(context, p->get_name(), p->get_index());
                    }
                }
            }
        }
        /* Parses the command line. Note that it does not parse any values to other types - that is left for the Arguments class. */
        Arguments parse(size_t argc, const char** argv) const {
            const std::string context = "ArgumentParser::parse() const";

            /* Phase 0: Prepare for parsing by extracting the executable. */

            // Extract the executable name
            const char* executable = argv[0];
            ++argv;
            --argc;

            /* Phase 1: Check the validity of the given arguments. */
            this->validate(context);

            /* Phase 2: Parse all arguments, one-by-one */
            size_t positional_i = 0;
            Arguments args;
            while (argc > 0) {
                const char* arg = argv[0];
                if (ArgumentParser::is_option(arg)) {
                    // Treat it as an Option or a Flag; try to find a match
                    bool found = false;
                    for (Argument* a : this->args) {
                        // Skip all Positionals
                        if (dynamic_cast<Positional*>(a)) { continue; }

                        // Check if the value matches with this Flag / Option
                        Flag* f = (Flag*) a;                        
                        if ((arg[2] == '\0' && arg[1] == f->get_shortlabel()) ||
                            (arg[1] == '-' && std::string(arg + 2) == f->get_name())) {
                            // Parse it as this Option / Flag
                            found = true;
                            Option* opt = dynamic_cast<Option*>(f);
                            if (opt) {
                                // Parse as Option

                                // Advance argc and argv to dodge the option itself
                                ++argv;
                                --argc;

                                // Try to collect the required number of values, but check if we don't go out-of-bounds in the meantime
                                std::vector<std::string> values;
                                values.resize(opt->n_values);
                                bool success = true;
                                for (size_t i = 0; i < opt->n_values; i++) {
                                    // Check if there are any left
                                    if (i >= argc) {
                                        // If the Option has a default value, then we can register it nonetheless
                                        if (i == 0 && opt->has_default_value()) {
                                            // Register the argument and then break from the parsing
                                            args.add_arg(*opt, opt->get_default_value(), true);
                                            success = false;
                                            break;
                                        }
                                        // Otherwise, throw exception
                                        throw NotEnoughValuesException(opt->type_name, opt->n_values, i, opt->get_name(), opt->get_shortlabel());
                                    }

                                    // There are, so grab the next word
                                    const char* next_arg = argv[0];

                                    // If it's an option, we were tricked
                                    if (i >= argc || ArgumentParser::is_option(next_arg)) {
                                        // If the Option has a default value, then we can register it nonetheless
                                        if (i == 0 && opt->has_default_value()) {
                                            // Simply break, after setting values to the default input
                                            args.add_arg(*opt, opt->get_default_value(), true);
                                            success = false;
                                            break;
                                        }
                                        throw NotEnoughValuesException(opt->type_name, opt->n_values, i, opt->get_name(), opt->get_shortlabel());
                                    }

                                    // Otherwise, add the value
                                    values.at(i) = next_arg;

                                    // Advance argv & argc
                                    ++argv;
                                    --argc;
                                }

                                // Register the argument if we were successful in collecting the values
                                if (success) {
                                    args.add_arg(*opt, opt->type_parser(values), true);
                                }

                            } else {
                                // Otherwise, parse as Flag
                                args.add_arg(*f, nullptr, true);

                                // Advance argc & argv once
                                ++argv;
                                --argc;
                            }
                        }
                    }
                    if (!found) {
                        if (arg[1] == '-') {
                            throw UnknownLonglabelException(arg + 2);
                        } else {
                            throw UnknownShortlabelException(arg[1]);
                        }
                    }
                } else {
                    // Find the correct Positional
                    Positional* pos = nullptr;
                    size_t p_i = 0;
                    for (Argument* a : this->args) {
                        if (dynamic_cast<Positional*>(a) && p_i++ == positional_i) {
                            pos = (Positional*) a;
                            // Only advance the positional_i if this positional can occur multiple times
                            if (!pos->get_any_number()) { ++positional_i; }
                            break;
                        }
                    }
                    // If it wasn't found, ignore the argument (since we've run out of Positionals)
                    if (pos == nullptr) { continue; }
                    
                    // Parse the next n_values parameters as part of this positional
                    std::vector<std::string> values;
                    values.resize(pos->n_values);
                    for (size_t i = 0; i < pos->n_values; i++) {
                        // Check if there are any left
                        if (i >= argc) {
                            throw NotEnoughValuesException(pos->type_name, pos->n_values, i, pos->get_name(), pos->get_index());
                        }

                        // There are, so grab the next word
                        const char* next_arg = argv[0];

                        // If it's an option, we were tricked
                        if (i >= argc || ArgumentParser::is_option(next_arg)) {
                            throw NotEnoughValuesException(pos->type_name, pos->n_values, i, pos->get_name(), pos->get_index());
                        }

                        // Otherwise, add the value
                        values.at(i) = next_arg;

                        // Advance argv & argc
                        ++argv;
                        --argc;
                    }

                    // Register the argument by parsing the input values
                    args.add_arg(*pos, pos->type_parser(values), true);
                }
            }

            /* Phase 3: Automatically handle help. */
            if (this->auto_help) {
                if (args.contains("help")) {
                    std::stringstream msg;
                    msg << std::endl << this->generate_usage(executable) << std::endl << std::endl << this->generate_help();
                    throw HelpHandledException(msg.str());
                }
            }

            /* Phase 4: Check mandatory. */
            for (Argument* a : this->args) {
                // Do not add if it already exists
                if (args.contains(a->get_name())) { continue; }

                // Switch to the correct value (but ignore Flags)
                if (dynamic_cast<Positional*>(a)) {
                    Positional* p = (Positional*) a;

                    // Only add it if it's optional and has a default value
                    if (p->get_optional() && p->has_default_value()) {
                        args.add_arg(*p, p->get_default_value(), false);
                    } else if (!p->get_optional()) {
                        // Throw because it's missing
                        throw MissingMandatoryException(p->get_name(), p->get_index());
                    }
                } else if (dynamic_cast<Option*>(a)) {
                    Option* o = (Option*) a;

                    // Only add it if it's optional and has a default value
                    if (o->get_optional() && o->has_default_value()) {
                        args.add_arg(*o, o->get_default_value(), false);
                    } else if (!o->get_optional()) {
                        // Throw because it's missing
                        throw MissingMandatoryException(o->get_name(), o->get_shortlabel());
                    }
                }
            }

            return args;
        }

        /* Generates a usage message based on all given arguments. The given argument is the executable that was used to run the parser. */
        std::string generate_usage(const char* exec) const {
            std::stringstream sstr;
            sstr << "Usage: " << exec;

            /* Phase 1: Print the flags */
            {
                // Step 1.1: Print the flags, as flags do not know include relationships
                bool first = false;
                for (Argument* a : this->args) {
                    if (dynamic_cast<Flag*>(a) && !dynamic_cast<Option*>(a)) {
                        if (!first) {
                            sstr << " -";
                            first = true;
                        }
                        sstr << ((Flag*) a)->get_shortlabel();
                    }
                }
            }

            /* Phase 2: Print the Options. */
            {
                // Step 2.1: Sort the Options into groups based on their include dependencies. This is done by seeing for each argument if it or it's dependencies occur in an already existing group, add them to that group if so or add them as a new group otherwise.
                std::vector<std::vector<Argument*>> option_groups;
                option_groups.reserve(this->n_options);
                for (Argument* a : this->args) {
                    // Skip all non-Options
                    if (!dynamic_cast<Option*>(a)) { continue; }

                    std::vector<Argument*> group = a->relations.at(ArgumentRelation::include);
                    group.push_back(a);
                    std::vector<Argument*>* target_group = nullptr;
                    for (Argument* dependency : group) {
                        // Try to find a group with one of the included elements in it
                        for (std::vector<Argument*>& group : option_groups) {
                            if (std::find(group.begin(), group.end(), dependency) != group.end()) {
                                target_group = &group;
                                break;
                            }
                        }
                        if (target_group) { break; }
                    }
                    // Either add the elements in this vector to the found group or add the group itself
                    if (target_group) {
                        // Make sure to only add if not yet added
                        for (Argument* dependency : group) {
                            if (std::find(target_group->begin(), target_group->end(), dependency) == target_group->end()) {
                                target_group->push_back(dependency);
                            }
                        }
                    } else {
                        option_groups.push_back(group);
                    }
                }

                // Step 2.2: Print the groups nicely
                for (std::vector<Argument*>& group : option_groups) {
                    sstr << ' ';

                    // If the first Option is optional, we can assume all of them will be.
                    Option* first = (Option*) group.at(0);
                    if (first->get_optional()) {
                        sstr << '[';
                    }
                    for (size_t i = 0; i < group.size(); i++) {
                        Option* o = (Option*) group.at(i);
                        if (i > 0) { sstr << ' '; }
                        sstr << '-' << o->get_shortlabel() << ' ';
                        if (o->has_default_value()) { sstr << "["; }
                        sstr << o->get_placeholder();
                        if (o->has_default_value()) { sstr << "]"; }
                    }
                    if (first->get_optional()) {
                        sstr << ']';
                    }
                }
            }

            /* Phase 3: Print the Positionals. */
            {
                // Step 3.1: Sort the Positionals into groups based on their include dependencies. This is done by seeing for each argument if it or it's dependencies occur in an already existing group, add them to that group if so or add them as a new group otherwise.
                std::vector<std::vector<Argument*>> positional_groups;
                positional_groups.reserve(this->n_options);
                for (Argument* a : this->args) {
                    // Skip all non-Positionals
                    if (!dynamic_cast<Positional*>(a)) { continue; }

                    std::vector<Argument*> group = a->relations.at(ArgumentRelation::include);
                    group.push_back(a);
                    std::vector<Argument*>* target_group = nullptr;
                    for (Argument* dependency : group) {
                        // Try to find a group with one of the included elements in it
                        for (std::vector<Argument*>& group : positional_groups) {
                            if (std::find(group.begin(), group.end(), dependency) != group.end()) {
                                target_group = &group;
                                break;
                            }
                        }
                        if (target_group) { break; }
                    }
                    // Either add the elements in this vector to the found group or add the group itself
                    if (target_group) {
                        // Make sure to only add if not yet added
                        for (Argument* dependency : group) {
                            if (std::find(target_group->begin(), target_group->end(), dependency) == target_group->end()) {
                                target_group->push_back(dependency);
                            }
                        }
                    } else {
                        positional_groups.push_back(group);
                    }
                }

                // Step 3.2: Print the Positional groups
                size_t n_optional_groups = 0;
                for (std::vector<Argument*>& group : positional_groups) {
                    // If the first in the group is optional, we assume all of them are
                    Positional* first = (Positional*) group.at(0);
                    if (first->get_optional()) {
                        sstr << '[';
                        ++n_optional_groups;
                    }
                    for (Argument* a : group) {
                        Positional* p = (Positional*) a;
                        sstr << " <" << p->get_name() << '>';
                        if (p->get_any_number()) { sstr << "..."; }
                    }
                }

                // Step 3.3: Print the closing brackets
                for (size_t i = 0; i < n_optional_groups; i++) {
                    sstr << ']';
                }
            }

            // Done, return
            return sstr.str();
        }
        /* Generates the help message based on all given arguments. Only displays arguments with a defined description. */
        std::string generate_help() const {
            std::vector<std::string> categories;
            std::unordered_map<std::string, std::vector<Argument*>> args;

            // First, sort by category
            for (Argument* a : this->args) {
                // Be sure to skip all values without description
                if (a->get_description().empty()) { continue; }

                std::string cat = a->get_category();
                categories.push_back(cat);
                args[cat].push_back(a);
            }
            
            // Then, extract the unique categories from the categories list
            size_t n_unique = 0;
            for (size_t i = 0; i < categories.size(); i++) {
                std::string& elem = categories[i];

                // Search the unique values (at the start of the vector) to see if the new one is unique
                bool found = false;
                for (size_t j = 0; j < n_unique; j++) {
                    if (elem == categories[j]) {
                        found = true;
                        break;
                    }
                }

                // If it is unique, add it to the end of the unique list
                if (!found) {
                    categories[n_unique++] = elem;
                }
            }

            // Print each category, in the order of flags - options - positionals
            std::stringstream sstr;
            for (size_t i = 0; i < n_unique; i++) {
                std::vector<Argument*>& cat_args = args[categories[i]];
                sstr << categories[i] << std::endl;

                // First pass: print all flags
                for (size_t j = 0; j < cat_args.size(); j++) {
                    Argument* arg = cat_args[j];

                    if (dynamic_cast<Flag*>(arg) && !dynamic_cast<Option*>(arg)) {
                        Flag* flg = (Flag*) arg;

                        // Write the human-recognisable name
                        sstr << "   -" << flg->get_shortlabel() << ", --" << flg->get_name();

                        // Either pad with zeroes or hit 'em with enter
                        if (flg->get_name().size() < 16) {
                            for (size_t k = 0; k < 16 - flg->get_name().size(); k++) { sstr << ' '; }
                        } else {
                            sstr << std::endl;
                            for (size_t k = 0; k < 25; k++) { sstr << ' '; }
                        }

                        // Next, print the description - with linewraps
                        std::vector<std::string> lines = linewrap(flg->get_description(), 55);
                        for (size_t k = 0; k < lines.size(); k++) {
                            if (k > 0) {
                                for (size_t l = 0; l < 25; l++) { sstr << ' '; }
                            }
                            sstr << lines[k] << std::endl;
                        }
                    }
                }

                // Second pass: print all options
                for (size_t j = 0; j < cat_args.size(); j++) {
                    Argument* arg = cat_args[j];

                    if (dynamic_cast<Option*>(arg)) {
                        Option* opt = (Option*) arg;

                        // Write the human-recognisable name
                        std::string name = "   " + (std::string("-") += opt->get_shortlabel()) + " ";
                        if (opt->has_default_value()) { name += "["; }
                        name += opt->get_placeholder();
                        if (opt->has_default_value()) { name += "]"; }
                        name += ", --" + opt->get_name() + " ";
                        if (opt->has_default_value()) { name += "["; }
                        name += opt->get_placeholder();
                        if (opt->has_default_value()) { name += "]"; }
                        sstr << name;

                        // Either pad with zeroes or hit 'em with enter
                        if (name.size() < 25) {
                            for (size_t k = 0; k < 25 - name.size(); k++) { sstr << ' '; }
                        } else {
                            sstr << std::endl;
                            for (size_t k = 0; k < 25; k++) { sstr << ' '; }
                        }

                        // Next, print the description - with linewraps
                        std::vector<std::string> lines = linewrap(opt->get_description(), 55);
                        for (size_t k = 0; k < lines.size(); k++) {
                            if (k > 0) {
                                for (size_t l = 0; l < 25; l++) { sstr << ' '; }
                            }
                            sstr << lines[k] << std::endl;
                        }
                    }
                }

                // Third pass: print all positionals
                for (size_t j = 0; j < cat_args.size(); j++)  {
                    Argument* arg = cat_args[j];

                    if (dynamic_cast<Positional*>(arg)) {
                        Positional* pos = (Positional*) arg;

                        // Write the human-readable name
                        std::string name = pos->get_name();
                        if (pos->optional) {
                            name = "   [<" + name + ">" + (pos->get_any_number() ? "..." : "") + "]";
                        } else {
                            name = "   <" + name + ">" + (pos->get_any_number() ? "..." : "");
                        }
                        sstr << name;

                        // Either pad with zeroes or hit 'em with enter
                        if (name.size() < 25) {
                            for (size_t k = 0; k < 25 - name.size(); k++) {
                                sstr << ' ';
                            }
                        } else {
                            sstr << std::endl;
                            for (size_t k = 0; k < 25; k++) { sstr << ' '; }
                        }

                        // Next, print the description - with linewraps
                        std::vector<std::string> lines = linewrap(pos->get_description(), 55);
                        for (size_t k = 0; k < lines.size(); k++) {
                            if (k > 0) {
                                for (size_t l = 0; l < 25; l++) { sstr << ' '; }
                            }
                            sstr << lines[k] << std::endl;
                        }
                    }
                }

                // Use a final newline for this category
                sstr << std::endl;
            }
            return sstr.str();
        }
    };
}

#endif
