/* ARGUMENT PARSER.hpp
 *   by Lut99
 *
 * Created:
 *   6/4/2020, 12:51:55 PM
 * Last edited:
 *   16/10/2020, 18:22:43
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
        positional = 1,
        option = 2,
        flag = 3,
        multi_argument = 4,
        included_group = 5,
        excluded_group = 6,
        required_group = 7
    };
    enum class MemberType {
        any = 0,
        positional = 1,
        option = 2,
        flag = 3
    };
    const static std::string argtype_name_map[] = {
        "Any",
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
    template<class T> struct type_name { static constexpr const char* value = "???"; };
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
    /* Exception that isn't really an exception, and used when --help is automatically handled by the parser. */
    class HelpHandledException : public ArgumentParserException {
    private:
        /* The message that was generated by the auto help implementation. */
        std::string help_message;

    public:
        /* The constructor for the HelpHandledException, which takes the help message generated. */
        HelpHandledException(const std::string& help_message) :
            ArgumentParserException("Automatically handled the --help flag."),
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

    /* Exception for when a Token is added on the Tokenizer while there's already an executable at it's head. */
    class DuplicateExecutableTokenException: public ProgrammingException {
    private:
        /* The value of the Executable token that was already on the stream. */
        std::string existing;
        /* The value of the token that was attempted to be added. */
        std::string given;
    
    public:
        /* Constructor for the ProgrammingException that takes a programming context, the value of the existing token already on the stream and the value of the newly given token. */
        DuplicateExecutableTokenException(const std::string& context, const std::string& existing_token, const std::string& given_token) :
            ProgrammingException(context, generate_message(context, "Cannot put token '" + given_token + "' on the stream when Executable token '" + existing_token + "' is at its head (no token has been taken).")),
            existing(existing_token),
            given(given_token)
        {}

        /* Returns the value of the already existing, executable token on the stream. */
        inline std::string get_existing() const { return this->existing; }
        /* Returns the value of the newly given token. */
        inline std::string get_given() const { return this->given; }

        /* Allows the DuplicateExecutableTokenException to be copied polymorphically. */
        virtual DuplicateExecutableTokenException* copy() const {
            return new DuplicateExecutableTokenException(*this);
        }

    };

    /* Exception for when we expected an argument to be non-variadic (e.g., not a variable number of arguments) but it was. */
    class SingletonMismatchException: public ProgrammingException {
    private:
        /* The name of the Argument that was mismatched. */
        std::string name;
    
    public:
        /* Constructor for the SingletonMismatchException class, which takes a context and the name of the variadic exception. */
        SingletonMismatchException(const std::string& context, const std::string& name) :
            ProgrammingException(context, generate_message(context, "Argument '" + name + "' can accept any number of values, which we expected it to only accept its values once.")),
            name(name)
        {}

        /* Returns the name of the mismatching argument. */
        inline std::string get_name() const { return this->name; }

        /* Allows the SingletonMismatchException to be copied polymorphically. */
        virtual SingletonMismatchException* copy() const {
            return new SingletonMismatchException(*this);
        }

    };
    /* Exception for when we mismatched the value type of an Argument. */
    class TypeMismatchException: public ProgrammingException {
    private:
        /* The name of the argument with the unexpected value type. */
        std::string arg_name;
        /* The name of the type that we expected the argument to have. */
        std::string expected_name;
        /* The name of the type that the argument actually had. */
        std::string given_name;
    
    public:
        /* Constructor for the TypeMismatchException class, which takes a context, the name of the conflicting argument, the name of the type we expected the argument to have and the name of the type it actually had. */
        TypeMismatchException(const std::string& context, const std::string& arg_name, const std::string& expected_type_name, const std::string& given_type_name) :
            ProgrammingException(context, generate_message(context, "")),
            arg_name(arg_name),
            expected_name(expected_type_name),
            given_name(given_type_name)
        {}

        /* Returns the name of the mismatching argument. */
        inline std::string get_arg_name() const { return this->arg_name; }
        /* Returns the name of the type we expected the mismatching argument to have. */
        inline std::string get_expected_type_name() const { return this->expected_name; }
        /* Returns the name of the type that the mismatching argument actually had. */
        inline std::string get_given_type_name() const { return this->given_name; }

        /* Allows the TypeMismatchException to be copied polymorphically. */
        virtual TypeMismatchException* copy() const {
            return new TypeMismatchException(*this);
        }

    };

    /* Baseclass exception for when a given name or shortlabel was illegal. */
    class IllegalIDException: public ProgrammingException {
    public:
        /* Constructor for the IllegalCharException which takes a context and optionally a message. */
        IllegalIDException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows the IllegalIDException to be copied polymorphically. */
        virtual IllegalIDException* copy() const {
            return new IllegalIDException(*this);
        }

    };
    /* Exception for when a given shortlabel ID is invalid. */
    class IllegalShortlabelException: public IllegalIDException {
    private:
        /* The shortlabel that was illegal. */
        char shortlabel;

    public:
        /* Constructor for the IllegalShortlabelCharException which takes a context and the illegal shortlabel. */
        IllegalShortlabelException(const std::string& context, const char illegal_shortlabel) :
            IllegalIDException(context, generate_message(context, (std::string("Got illegal shortlabel '") += illegal_shortlabel)  + "'.")),
            shortlabel(illegal_shortlabel)
        {}

        /* Returns the shortlabel that was illegal. */
        inline char get_shortlabel() const { return this->shortlabel; }

        /* Allows the IllegalShortlabelException to be copied polymorphically. */
        virtual IllegalShortlabelException* copy() const {
            return new IllegalShortlabelException(*this);
        }

    };
    /* Exception for when a given name ID is invalid. */
    class IllegalNameException: public IllegalIDException {
    private:
        /* The illegal name. */
        std::string name;

    public:
        /* Constructor for the IllegalNameException which takes the illegal character and the string where it occurred. */
        IllegalNameException(const std::string& context, const std::string& name) :
            IllegalIDException(context, generate_message(context, "Got illegal name '" + name + "'.")),
            name(name)
        {}

        /* Returns the name that was somehow illegal. */
        inline std::string get_name() const { return this->name; }

        /* Allows the IllegalNameException to be copied polymorphically. */
        virtual IllegalNameException* copy() const {
            return new IllegalNameException(*this);
        }

    };

    /* Baseclass exception for when a given name or shortlabel is already in use. */
    class DuplicateIDException: public ProgrammingException {
    public:
        /* Constructor for the DuplicateIDException class, which takes a programming context and optionally a message. */
        DuplicateIDException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows the DuplicateIDException to be copied polymorphically. */
        virtual DuplicateIDException* copy() const {
            return new DuplicateIDException(*this);
        }

    };
    /* Exception for when a name is already in use by another argument. */
    class DuplicateNameException: public DuplicateIDException {
    private:
        /* The name that was duplicate. */
        std::string name;
    
    public:
        /* Constructor for the DuplicateNameException class which takes a programming context and the name that was already in use. */
        DuplicateNameException(const std::string& context, const std::string& name) :
            DuplicateIDException(context, generate_message(context, "An argument with the name '" + name + "' already exists.")),
            name(name)
        {}

        /* Returns the name that was already in use. */
        inline std::string get_name() const { return this->name; }

        /* Allows the DuplicateNameException to be copied polymorphically. */
        virtual DuplicateNameException* copy() const {
            return new DuplicateNameException(*this);
        }

    };
    /* Exception for when a shortlabel is already in use by another argument. */
    class DuplicateShortlabelException: public DuplicateIDException {
    private:
        /* The shortlabel that was duplicate. */
        char shortlabel;
    
    public:
        /* Constructor for the DuplicateShortlabelException class which takes a programming context and the shortlabel that was already in use. */
        DuplicateShortlabelException(const std::string& context, const char shortlabel) :
            DuplicateIDException(context, generate_message(context, (std::string("An argument with the shortlabel '") += shortlabel) + "' already exists.")),
            shortlabel(shortlabel)
        {}

        /* Returns the shortlabel that was already in use. */
        inline char get_shortlabel() const { return this->shortlabel; }

        /* Allows the DuplicateShortlabelException to be copied polymorphically. */
        virtual DuplicateShortlabelException* copy() const {
            return new DuplicateShortlabelException(*this);
        }

    };

    /* Baseclass exception for when a given name or shortlabel does not exist. */
    class UnknownIDException: public ProgrammingException {
    public:
        /* Constructor for the UnknownIDException which takes a programming context and optionally a message. */
        UnknownIDException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows the UnknownIDException to be copied polymorphically. */
        virtual UnknownIDException* copy() const {
            return new UnknownIDException(*this);
        }

    };
    /* Exception for when a given name does not exist. */
    class UnknownNameException: public UnknownIDException {
    private:
        /* The name that was unknown. */
        std::string name;
    
    public:
        /* Constructor for the UnknownNameException class which takes a programming context and the name that was unknown. */
        UnknownNameException(const std::string& context, const std::string& name) :
            UnknownIDException(context, generate_message(context, "Could not find Argument with name '" + name + "'.")),
            name(name)
        {}

        /* Returns the name that was unknown. */
        inline std::string get_name() const { return this->name; }

        /* Allows the UnknownNameException to be copied polymorphically. */
        virtual UnknownNameException* copy() const {
            return new UnknownNameException(*this);
        }

    };
    /* Exception for when a given shortlabel does not exist. */
    class UnknownShortlabelException: public UnknownIDException {
    private:
        /* The shortlabel that was unknown. */
        char shortlabel;
    
    public:
        /* Constructor for the UnknownShortlabelException class which takes a programming context and the shortlabel that was unknown. */
        UnknownShortlabelException(const std::string& context, const char shortlabel) :
            UnknownIDException(context, generate_message(context, (std::string("Could not find Argument with shortlabel '") += shortlabel) + "'.")),
            shortlabel(shortlabel)
        {}

        /* Returns the shortlabel that was unknown. */
        inline char get_shortlabel() const { return this->shortlabel; }

        /* Allows the UnknownShortlabelException to be copied polymorphically. */
        virtual UnknownShortlabelException* copy() const {
            return new UnknownShortlabelException(*this);
        }

    };

    /* Exception for when we have mismatched the argument's type. */
    class ArgumentTypeException: public ProgrammingException {
    protected:
        /* The name of the argument that was mismatched. */
        std::string name;
        /* The ArgumentType we expected the argument to have. */
        ArgumentType expected;
        /* The ArgumentType the argument actually had. */
        ArgumentType given;

    public:
        /* Constructor for the ArgumentTypeException that takes a context, the name of the argument that was mismatched, the expected type and the given type. */
        ArgumentTypeException(const std::string& context, const std::string& name, const ArgumentType expected_type, const ArgumentType given_type) :
            ProgrammingException(context, generate_message(context, "Expected argument '" + name + "' to be of type " + argtype_name_map[(int) expected_type] + ", but is of type " + argtype_name_map[(int) given_type] + ".")),
            name(name),
            expected(expected_type),
            given(given_type)
        {}

        /* Returns the name of the argument that we mismatched its type of. */
        inline std::string get_name() const { return this->name; }
        /* Returns the ArgumentType that we expected the argument to have. */
        inline ArgumentType get_expected_argtype() const { return this->expected; }
        /* Returns the ArgumentType that the argument actually had. */
        inline ArgumentType get_given_argtype() const { return this->given; }

        /* Allows the ArgumentTypeException to be copied polymorphically. */
        virtual ArgumentTypeException* copy() const {
            return new ArgumentTypeException(*this);
        }

    };
    /* Specialized exception for when an Argument is assumed to have a value (e.g., a Positional or an Option, it assumes Option) but it didn't (it was a Flag). */
    class ValueTypeMismatchException: public ArgumentTypeException {
    public:
        /* The constructor of the ValueTypeMismatchException, that takes a context and the name of the argument that was mismatched. */
        ValueTypeMismatchException(const std::string& context, const std::string& name) :
            ArgumentTypeException(context, name, ArgumentType::option, ArgumentType::flag)
        {
            this->msg = generate_message(context, "Expected argument '" + name + "' to have a value, but it didn't (it was a Flag).");
        }

        /* Allows the ValueTypeMismatchException to be copied polymorphically. */
        virtual ValueTypeMismatchException* copy() const {
            return new ValueTypeMismatchException(*this);
        }

    };

    /* Baseclass exception for when a certain argument is illegally added to a MultiGroup argument. */
    class MultiGroupTypeException: public ProgrammingException {
    protected:
        /* The name of the group to which we tried to add an argument. */
        std::string group_name;
        /* The type of the group to which we tried to add an argument. */
        ArgumentType group_type;
        /* The name of the argument we tried to add to the group. */
        std::string arg_name;
        /* The type of the argument we tried to add to the group. */
        ArgumentType arg_type;

        /* Returns the message of a derived class plus whatever the parent classes wants to add. */
        static inline std::string generate_message(const std::string& context, const std::string& group_name, const ArgumentType group_type, const std::string& arg_name, const ArgumentType arg_type, const std::string& message) {
            return ProgrammingException::generate_message(context, "Could not add " + argtype_name_map[(int) arg_type] + " '" + arg_name + "' to " + argtype_name_map[(int) group_type] + " '" + group_name + "'" + (message.empty() ? "" : ": " + message));
        }

    public:
        /* Constructor for the MultiGroupTypeException, which takes a context, the name of the group, the type of the group, the name of the argument, the type of the argument and optionally a message. */
        MultiGroupTypeException(const std::string& context, const std::string& group_name, const ArgumentType group_type, const std::string& arg_name, const ArgumentType arg_type, const std::string& message = "") :
            ProgrammingException(context, message),
            group_name(group_name),
            group_type(group_type),
            arg_name(arg_name),
            arg_type(arg_type)
        {}

        /* Returns the name of the MultiArgument to which we tried to add another argument. */
        inline std::string get_group_name() const { return this->group_name; }
        /* Returns the type of the MultiArgument to which we tried to add another argument. */
        inline ArgumentType get_group_type() const { return this->group_type; }
        /* Returns the name of the argument which we tried to illegaly add to the MultiArgument. */
        inline std::string get_arg_name() const { return this->arg_name; }
        /* Returns the type of the argument which we tried to illegaly add to the MultiArgument. */
        inline ArgumentType get_arg_type() const { return this->arg_type; }

        /* Allows the MultiGroupTypeException to be copied polymorphically. */
        virtual MultiGroupTypeException* copy() const {
            return new MultiGroupTypeException(*this);
        }

    };
    /* Exception for when a custom MemberType was attempted to be given to a MultiGroup that already had a member type. */
    class MemberTypeConflictException: public MultiGroupTypeException {
    public:
        /* Constructor for the MemberTypeConflictException which takes a context, the name of the parent group we tried to add a nested group to, the parent group's type, the name of the nested group we tried to add and the nested group's type. */
        MemberTypeConflictException(const std::string& context, const std::string& parent_name, const ArgumentType parent_type, const std::string& nested_name, const ArgumentType nested_type) :
            MultiGroupTypeException(context, parent_name, parent_type, nested_name, nested_type,
                                    generate_message(context, parent_name, parent_type, nested_name, nested_type, "Cannot custumize the member type of a group that will be added to a parent group that already has a member type defined."))
        {}

        /* Allows the MemberTypeConflictException to be copied polymorphically. */
        virtual MemberTypeConflictException* copy() const {
            return new MemberTypeConflictException(*this);
        }

    };
    /* Exception for when an argument was attempted to be added to a derivative of the MultiArgument, but that group was already populated by arguments of a different type. */
    class MemberTypeMismatchException: public MultiGroupTypeException {
    private:
        /* The type bound to the group we tried to add a member to. */
        MemberType member_type;

    public:
        /* Constructor for the MemberTypeMismatchException class, which takes a context, the name of the group we tried to add a member to, the argument type of the group, the member type of the group, the name of the argument we tried to add and the type of the argument. */
        MemberTypeMismatchException(const std::string& context, const std::string& group_name, const ArgumentType group_type, const MemberType member_type, const std::string& arg_name, const ArgumentType arg_type) :
            MultiGroupTypeException(context, group_name, group_type, arg_name, arg_type,
                                    generate_message(context, group_name, group_type, arg_name, arg_type, "Members in group have incompatible type " + argtype_name_map[(int) member_type])),
            member_type(member_type)
        {}

        /* Returns the ArgumentType of the members in the group we tried to add an argument to. */
        inline MemberType get_member_type() const { return this->member_type; }

        /* Allows the MemberTypeMismatchException to be copied polymorphically. */
        virtual MemberTypeMismatchException* copy() const {
            return new MemberTypeMismatchException(*this);
        }

    };
    /* Exception for when an argument was attempted to be added to a derivative of the MultiArgument, but that type is always illegal for that type of relation. */
    class IllegalGroupTypeException: public MultiGroupTypeException {
    public:
        /* Constructor for the IllegalGroupTypeException class, which takes a context, the name of the group we tried to add a member to, the type of the group, the name of the argument we tried to add and that argument's type. */
        IllegalGroupTypeException(const std::string& context, const std::string& group_name, const ArgumentType group_type, const std::string& arg_name, const ArgumentType arg_type) :
            MultiGroupTypeException(context, group_name, group_type, arg_name, arg_type,
                                    generate_message(context, group_name, group_type, arg_name, arg_type, argtype_name_map[(int) group_type] + " doesn't accept arguments of type " + argtype_name_map[(int) arg_type]))
        {}

        /* Allows the IllegalGroupTypeException to be copied polymorphically. */
        virtual IllegalGroupTypeException* copy() const {
            return new IllegalGroupTypeException(*this);
        }

    };
    /* Exception for when a Positional was added to a MultiGroup that wasn't the direct neighbour of the last Positional in that group. */
    class NonSequentialPositionalException: public MultiGroupTypeException {
    private:
        /* Index of the last Positional in the group. */
        size_t group_index;
        /* Index of the Positional we tried to add to the group. */
        size_t arg_index;

    public:
        /* Constructor for the NonSequentialPositionalException class, which takes a context, the name of the group we tried to add a Positional to, that group's type, the index of the last Positional in that group, the name of the Positional we tried to add to that group and that Positional's index. */
        NonSequentialPositionalException(const std::string& context, const std::string& group_name, const ArgumentType group_type, const size_t group_index, const std::string& arg_name, const size_t arg_index) :
            MultiGroupTypeException(context, group_name, group_type, arg_name, ArgumentType::positional,
                                    generate_message(context, group_name, group_type, arg_name, ArgumentType::positional, "Given Positional (at index " + std::to_string(arg_index) + ") is not a direct neighbour of the last Positional in the group (at index " + std::to_string(group_index) + ")")),
            group_index(group_index),
            arg_index(arg_index)
        {}

        /* Returns the index of the last Positional in the group we tried to add a new Positional to. */
        inline size_t get_group_index() const { return this->group_index; }
        /* Returns the index of the Positional we tried to add to a MultiArgument. */
        inline size_t get_arg_index() const { return this->arg_index; }

        /* Allows the NonSequentialPositionalException to be copied polymorphically. */
        virtual NonSequentialPositionalException* copy() const {
            return new NonSequentialPositionalException(*this);
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
    
    /* Exception for when no input is given to the parser. */
    class NoInputException: public ParseException {
    public:
        /* Constructor for the NoInputException which takes... nothing! */
        NoInputException() :
            ParseException(generate_message("No input given (not even an executable!)"))
        {}

        /* Allows the NoInputException to be copied polymorphically. */
        virtual NoInputException* copy() const {
            return new NoInputException(*this);
        }

    };

    /* Exception for when a non-optional Positional was defined after optional Positionals. */
    class OptionalPositionalException: public ParseException {
    private:
        /* The name of the violating Positional. */
        std::string name;
        /* The index of the violating Positional. */
        size_t index;
    
    public:
        /* Constructor for the OptionalPositionalException class, which takes the name and the index of the Positional that was mandatory but declared after optional ones. */
        OptionalPositionalException(const std::string& name, const size_t index) :
            ParseException(generate_message("Mandatory Positional '" + name + "' at index " + std::to_string(index) + " was declared after Optional Positionals have been declared.")),
            name(name),
            index(index)
        {}

        /* Returns the name of the violating, mandatory Positional. */
        inline std::string get_name() const { return this->name; }
        /* Returns the CLI-relevant index of the violating, mandatory Positional. */
        inline size_t get_index() const { return this->index; }

        /* Allows the OptionalPositionalException to be copied polymorphically. */
        virtual OptionalPositionalException* copy() const {
            return new OptionalPositionalException(*this);
        }

    };
    /* Exception for when any Positional other than the last one is declared to be variadic. */
    class VariadicPositionalException: public ParseException {
    private:
        /* The name of the violating Positional. */
        std::string name;
        /* The index of the violating Positional. */
        size_t index;
    
    public:
        /* Constructor for the VariadicPositionalException class, which takes the name and the index of the Positional that was declared variadic but not the last one. */
        VariadicPositionalException(const std::string& name, const size_t index) :
            ParseException(generate_message("Positional '" + name + "' at index " + std::to_string(index) + " was declared to be variadic, but was not the last Positional.")),
            name(name),
            index(index)
        {}

        /* Returns the name of the violating, variadic Positional. */
        inline std::string get_name() const { return this->name; }
        /* Returns the CLI-relevant index of the violating, variadic Positional. */
        inline size_t get_index() const { return this->index; }

        /* Allows the VariadicPositionalException to be copied polymorphically. */
        virtual VariadicPositionalException* copy() const {
            return new VariadicPositionalException(*this);
        }

    };

    /* Baseclass exception for when an illegal character was encountered during name or shortlabel parsing. */
    class IllegalIDCharException: public ParseException {
    private:
        /* The character that was illegal. */
        char illegal;
    
    public:
        /* Constructor for the IllegalCharException which takes the illegal character parsed and optionally a message. */
        IllegalIDCharException(const char illegal_char, const std::string& message = "") :
            ParseException(message),
            illegal(illegal_char)
        {}

        /* Returns the illegal character that was encountered. */
        inline char get_illegal() const { return this->illegal; }

        /* Allows the IllegalIDCharException to be copied polymorphically. */
        virtual IllegalIDCharException* copy() const {
            return new IllegalIDCharException(*this);
        }

    };
    /* Exception for when we wanted to parse a shortlabel, but we encountered an illegal character instead. */
    class IllegalShortlabelCharException: public IllegalIDCharException {
    public:
        /* Constructor for the IllegalShortlabelCharException which takes the illegal character. */
        IllegalShortlabelCharException(const char illegal_char) :
            IllegalIDCharException(illegal_char, generate_message((std::string("Encountered illegal shortlabel '") += illegal_char)  + "'."))
        {}

        /* Allows the IllegalShortlabelCharException to be copied polymorphically. */
        virtual IllegalShortlabelCharException* copy() const {
            return new IllegalShortlabelCharException(*this);
        }

    };
    /* Exception for when we wanted to parse a name, but we encountered an illegal character instead. */
    class IllegalNameCharException: public IllegalIDCharException {
    private:
        /* The name where the illegal character occurred. */
        std::string name;

    public:
        /* Constructor for the IllegalNameCharException which takes the illegal character and the string where it occurred. */
        IllegalNameCharException(const char illegal_char, const std::string& name) :
            IllegalIDCharException(illegal_char, generate_message((std::string("Encountered illegal character '") += illegal_char)  + "' in the name '" + name + "'.")),
            name(name)
        {}

        /* Returns the name where the illegal character occurred. */
        inline std::string get_name() const { return this->name; }

        /* Allows the IllegalNameCharException to be copied polymorphically. */
        virtual IllegalNameCharException* copy() const {
            return new IllegalNameCharException(*this);
        }

    };

    /* Exception for when an Argument should have been specified, but wasn't. */
    class MissingMandatoryException: public ParseException {
    private:
        /* The name of the missing argument. */
        std::string name;
    
    public:
        /* Constructor for the MissingMandatoryException class that takes the name of the argument that was missing. */
        MissingMandatoryException(const std::string& name) :
            ParseException(generate_message("Missing mandatory argument '" + name + "'.")),
            name(name)
        {}

        /* Returns the name of the mandatory argument that was missing. */
        inline std::string get_name() const { return this->name; }

        /* Allows the MissingMandatoryException to be copied polymorphically. */
        virtual MissingMandatoryException* copy() const {
            return new MissingMandatoryException(*this);
        }

    };
    /* Exception for when an Argument was specified multiple times when it wasn't allowed to be. */
    class DuplicateArgumentException: public ParseException {
    private:
        /* The name of the duplicate argument. */
        std::string name;
        /* The shortlabel of the duplicate argument. */
        char shortlabel;
    
    public:
        /* Constructor for the MissingMandatoryException class that takes the name of the argument that was specified more than once and its shortlabel. */
        DuplicateArgumentException(const std::string& name, const char shortabel) :
            ParseException(generate_message("Duplicate argument '" + name + "'" + (shortlabel == '\0' ? "" : (std::string(" -(") += shortlabel) + ")") + ".")),
            name(name),
            shortlabel(shortlabel)
        {}

        /* Returns the name of the mandatory argument that was missing. */
        inline std::string get_name() const { return this->name; }
        /* Returns the shortlabel of the mandatory argument that was missing. If the argument had no shortlabel, returns '\0'. */
        inline char get_shortlabel() const { return this->shortlabel; }

        /* Allows the DuplicateArgumentException to be copied polymorphically. */
        virtual DuplicateArgumentException* copy() const {
            return new DuplicateArgumentException(*this);
        }

    };

    /* Baseclass exception for all dependency-related issues. */
    class DependencyException: public ParseException {
    private:
        /* The name of the violating argument. */
        std::string name;
        /* The name of the argument whos presence made or broke this deal. */
        std::string peer_name;

    public:
        /* Constructor for the DependencyException class that takes the name of the violating argument and optionally a message. */
        DependencyException(const std::string& name, const std::string& peer_name, const std::string& message = "") :
            ParseException(message),
            name(name),
            peer_name(peer_name)
        {}

        /* Returns the name of the violating argument. */
        inline std::string get_name() const { return this->name; }
        /* Returns the name of the argument who caused this issue by being or not being there. */
        inline std::string get_peer_name() const { return this->peer_name; }
        
        /* Allows the DependencyException to be copied polymorphically. */
        virtual DependencyException* copy() const {
            return new DependencyException(*this);
        }

    };
    /* Exception for when an argument part of an IncludedGroup is not specified, while at least one of the other arguments in that group is. */
    class IncludedDependencyException: public DependencyException {
    public:
        /* Constructor for the IncludedDependencyException class that takes the name of the violating arguments and one of its peer arguments. */
        IncludedDependencyException(const std::string& name, const std::string& peer_name) :
            DependencyException(name, generate_message("Missing argument '" + name + "' since argument '" + peer_name + "' is given."))
        {}
        
        /* Allows the IncludedDependencyException to be copied polymorphically. */
        virtual IncludedDependencyException* copy() const {
            return new IncludedDependencyException(*this);
        }

    };
    /* Exception for when an argument part of an ExcludedGroup is specified, while at least one of the other arguments in that group already was. */
    class ExcludedDependencyException: public DependencyException {
    public:
        /* Constructor for the ExcludedDependencyException class that takes the name of the violating arguments and one of its peer arguments. */
        ExcludedDependencyException(const std::string& name, const std::string& peer_name) :
            DependencyException(name, generate_message("Argument '" + name + "' specified while conflicting argument '" + peer_name + "' was already given."))
        {}
        
        /* Allows the ExcludedDependencyException to be copied polymorphically. */
        virtual ExcludedDependencyException* copy() const {
            return new ExcludedDependencyException(*this);
        }

    };
    /* Exception for when an argument part of an RequiredGroup is specified, but it's previous argument wasn't. */
    class RequiredDependencyException: public DependencyException {
    public:
        /* Constructor for the RequiredDependencyException class that takes the name of the violating arguments and one of its peer arguments. */
        RequiredDependencyException(const std::string& name, const std::string& peer_name) :
            DependencyException(name, generate_message("Argument '" + name + "' specified without specifying required argument '" + peer_name + "'."))
        {}
        
        /* Allows the RequiredDependencyException to be copied polymorphically. */
        virtual RequiredDependencyException* copy() const {
            return new RequiredDependencyException(*this);
        }

    };

    /* Baseclass exception for any error related to TypeParsing. */
    class TypeParseException: public ParseException {
    private:
        /* The message passed on to us by any derived class. */
        std::string derived_msg;
    
    protected:
        /* The name of the type we wanted to parse to. */
        std::string type_name;
        /* The name of the argument we wanted to parse. */
        std::string arg_name;
        /* The name of the shortlabel we wanted to parse. Is equal to: '\0' if there isn't any. */
        char shortlabel;

        /* Generates a message by calling the parent generator and adding the name of the argument and the type. */
        static inline std::string generate_message(const std::string& type_name, const std::string& name, const std::string& message) {
            return ParseException::generate_message("Could not parse value of Positional '" + name + "' as " + type_name + (message.empty() ? "" : ": " + message));
        }
        /* Generates a message by calling the parent generator and adding the name and the the shortlabel of the argument and the type. */
        static inline std::string generate_message(const std::string& type_name, const std::string& name, const char shortlabel, const std::string& message) {
            return ParseException::generate_message("Could not parse value of '--" + name + "'" + (shortlabel == '\0' ? "" : (std::string(" (-") += shortlabel) + ")") + " as " + type_name + (message.empty() ? "" : ": " + message));
        }

    public:
        /* Constructor for the TypeParseException class which only takes the name of the type and optionally a message. Other values, like the argument name and the shortlabel, will have to be injected later using 'inject'. */
        TypeParseException(const std::string& type_name, const std::string& message) :
            ParseException(),
            derived_msg(message),
            type_name(type_name)
        {}

        /* Returns the name of the type that we couldn't parse to. */
        inline std::string get_type_name() const { return this->type_name; }
        /* Returns the name of the argument that we attempted to parse. */
        inline std::string get_arg_name() const { return this->arg_name; }
        /* Returns the shortlabel of the argument that we attempted to parse to. If there wasn't any shortlabel, the return value is equal to '\0'. */
        inline char get_shortlabel() const { return this->shortlabel; }
        
        /* Inserts information about a Positional that is very useful to have but not available in the parsers. */
        void insert(const std::string& name) {
            this->msg = this->generate_message(this->type_name, name, this->derived_msg);
            this->arg_name = name;
            this->shortlabel = '\0';
        }
        /* Inserts information about an Option that is very useful to have but not available in the parsers. */
        void insert(const std::string& name, char shortlabel) {
            this->msg = this->generate_message(this->type_name, name, shortlabel, this->derived_msg);
            this->arg_name = name;
            this->shortlabel = shortlabel;
        }
    
        /* Allows the TypeParseException to be copied polymorphically. */
        virtual TypeParseException* copy() const {
            return new TypeParseException(*this);
        }

    };
    /* Exception for when a type does not have enough values to unpack for an argument. */
    class NotEnoughValuesException: public TypeParseException {
    private:
        /* The expected amount of values to unpack. */
        size_t expected;
        /* The given amount of values to unpack. */
        size_t given;
    
    public:
        /* Constructor for the NotEnoughValuesException class, which accepts the name of the type we wanted to parse to, the number of arguments for that type and the actual number of values. */
        NotEnoughValuesException(const std::string& type_name, const size_t expected_n_values, const size_t given_n_values) :
            TypeParseException(type_name, "Not enough values to unpack (expected at least " + std::to_string(expected_n_values) + ", got " + std::to_string(given_n_values) + ")."),
            expected(expected_n_values),
            given(given_n_values)
        {}

        /* Returns the minimum number of values required for this type. */
        inline size_t get_expected() const { return this->expected; }
        /* Returns the illegal amount of values got when we attempted to parse. */
        inline size_t get_given() const { return this->given; }

        /* Allows the NotEnoughValuesException to be copied polymorphically. */
        virtual NotEnoughValuesException* copy() const {
            return new NotEnoughValuesException(*this);
        }

    };
    /* Baseclass exception for all OutOfRangeExceptions. */
    class OutOfRangeException: public TypeParseException {
    protected:
        /* Constructor for the OutOfRangeException, which takes a type name and possibly a message. */
        OutOfRangeException(const std::string& type_name, const std::string& message = "") :
            TypeParseException(type_name, message)
        {}

    public:
        /* Allows the OutOfRangeException to be copied polymorphically. */
        virtual OutOfRangeException* copy() const {
            return new OutOfRangeException(*this);
        }

    };
    /* Templated exception for when a value is out of range for some type. The given type should be a numberical type that can accept the given range. */
    template <class T, typename = std::enable_if_t<std::is_arithmetic<T>::value> >
    class OutOfTRangeException: public OutOfRangeException {
    private:
        /* The minimum value that is allowed (inclusive). */
        T minimum;
        /* The maximum value that is allowed (inclusive). */
        T maximum;
        /* The value we actually got, as string (since it might not fit in T). */
        std::string given;
    
    public:
        /* Constructor for the OutOfTRangeException class, which accepts the type we attempted to parse to, that type's minimum value (inclusive), that type's maximum value (exclusive) and the given value as string. */
        OutOfTRangeException(const std::string& type_name, const T minimum, const T maximum, const std::string& given) :
            OutOfRangeException(type_name, "Value '" + given + "' is out of range [" + std::to_string(minimum) + ", " + std::to_string(maximum) + "]."),
            minimum(minimum),
            maximum(maximum),
            given(given)
        {}

        /* Returns the minimum value for this type (inclusive). */
        inline T get_minimum() const { return this->minimum; }
        /* Returns the maximum value for this type (inclusive). */
        inline T get_maximum() const { return this->maximum; }
        /* Returns the value that we were given (as string). */
        inline std::string get_given() const { return this->given; }

        /* Allows the OutOfTRangeException to be copied polymorphically. */
        virtual OutOfTRangeException* copy() const {
            return new OutOfTRangeException(*this);
        }

    };
    /* Exception for when an illegal character is encountered when parsing a value to a type. */
    class IllegalCharException: public TypeParseException {
    private:
        /* The message describing the range of allowed characters. */
        std::string allowed;
        /* The character that was illegal. */
        char illegal;
    
    public:
        /* Constructor for the IllegalCharException which takes the name of type we wanted to parse to, the illegal character and a string describing the allowed characters. See the Wiki for more details. */
        IllegalCharException(const std::string& type_name, const char illegal_char, const std::string& allowed_chars) :
            TypeParseException(type_name, "Encountered illegal characters (" + type_name + " only accepts " + allowed_chars + ")."),
            allowed(allowed_chars),
            illegal(illegal_char)
        {}

        /* Returns the illegal character that was encountered. */
        inline char get_illegal() const { return this->illegal; }
        /* Returns the string describing the valid characters. */
        inline std::string get_allowed() const { return this->allowed; }

        /* Allows the IllegalCharException to be copied polymorphically. */
        virtual IllegalCharException* copy() const {
            return new IllegalCharException(*this);
        }

    };
    /* Exception for when an illegal value is given for a type. */
    class IllegalValueException: public TypeParseException {
    private:
        /* The message describing the range of allowed characters. */
        std::string allowed;
        /* The value that was illegal. */
        std::string illegal;
    
    public:
        /* Constructor for the IllegalCharException which takes the name of type we wanted to parse to, the illegal character and a string describing the allowed characters. See the Wiki for more details. */
        IllegalValueException(const std::string& type_name, const std::string illegal_value, const std::string& allowed_values) :
            TypeParseException(type_name, "Encountered illegal characters (" + type_name + " only accepts " + allowed_values + ")."),
            allowed(allowed_values),
            illegal(illegal_value)
        {}

        /* Returns the illegal value that was encountered. */
        inline std::string get_illegal() const { return this->illegal; }
        /* Returns the string describing the valid characters. */
        inline std::string get_allowed() const { return this->allowed; }

        /* Allows the IllegalValueException to be copied polymorphically. */
        virtual IllegalValueException* copy() const {
            return new IllegalValueException(*this);
        }

    };
    /* Baseclass exception for when the given length of a string is incorrect. */
    class StringSizeException: public TypeParseException {
    protected:
        /* The string that had the incorrect length. If the string is empty, we'll hide it. */
        std::string given;
    
    public:
        /* Constructor for the StringSizeException class that takes the typename, the string that was incorrectly sized and optionally a message. */
        StringSizeException(const std::string& type_name, const std::string& given_string, const std::string& message = "") :
            TypeParseException(type_name, message),
            given(given_string)
        {}

        /* Returns the string that was incorrectly sized. */
        inline std::string get_given() const { return this->given; }
        /* Returns the size of the string that was incorrectly sized. */
        inline size_t get_given_size() const { return this->given.size(); }

        /* Allows the StringSizeException to be copied polymorphically. */
        virtual StringSizeException* copy() const {
            return new StringSizeException(*this);
        }
  
    };
    /* Exception for when a given string is too large. */
    class StringTooLargeException: public StringSizeException {
    protected:
        /* The maximum length of the string. */
        size_t max_size;
    
    public:
        /* Constructor for the StringTooLargeException class that takes the name of the type we wanted to parse to, the given string and the maximum string length. */
        StringTooLargeException(const std::string& type_name, const std::string& given, const size_t max_size) :
            StringSizeException(type_name, given, "Given string is too large (" + std::to_string(given.size()) + " > " + std::to_string(max_size) + ")."),
            max_size(max_size)
        {}

        /* Returns the maximum allowed string size. */
        inline size_t get_max_size() const { return this->max_size; }

        /* Allows the StringTooLargeException to be copied polymorphically. */
        virtual StringTooLargeException* copy() const {
            return new StringTooLargeException(*this);
        }

    };
    /* Exception for when a given string is too small. */
    class StringTooSmallException: public StringSizeException {
    protected:
        /* The minimum length of the string. */
        size_t min_size;
    
    public:
        /* Constructor for the StringTooSmallException class that takes the name of the type we wanted to parse to, the given string and the minimum string length. */
        StringTooSmallException(const std::string& type_name, const std::string& given, const size_t min_size) :
            StringSizeException(type_name, given, "Given string is too small (" + std::to_string(given.size()) + " < " + std::to_string(min_size) + ")."),
            min_size(min_size)
        {}

        /* Returns the minimum allowed string size. */
        inline size_t get_min_size() const { return this->min_size; }

        /* Allows the StringTooSmallException to be copied polymorphically. */
        virtual StringTooSmallException* copy() const {
            return new StringTooSmallException(*this);
        }

    };
    /* Exception for when a given string just has an incorrect size. */
    class StringSizeIncorrectException: public StringSizeException {
    protected:
        /* A string describing the allowed sizes. */
        std::string allowed;
    
    public:
        /* Constructor for the StringSizeIncorrectException class that takes the name of the type we wanted to parse to, the given string and a string describing the allowed string sizes. */
        StringSizeIncorrectException(const std::string& type_name, const std::string& given, const std::string& allowed_sizes) :
            StringSizeException(type_name, given, "Given string of length " + std::to_string(given.size()) + " has incorrect size (" + type_name + " only allows " + allowed_sizes + ")."),
            allowed(allowed_sizes)
        {}

        /* Returns a string describing all allowed sizes. */
        inline std::string get_allowed() const { return this->allowed; }

        /* Allows the StringSizeIncorrectException to be copied polymorphically. */
        virtual StringSizeIncorrectException* copy() const {
            return new StringSizeIncorrectException(*this);
        }

    };





    /******************** TOKENIZER ********************/

    /* Enum describing the type of a Token. Executable is simply the first argument, label is anything signalling a Flag or an Option and value is some value that will need to be parsed. */
    enum class TokenType {
        executable,
        label,
        value,
        empty
    };

    /* The Token struct, which represents a single CLI argument returned by the Tokenizer. */
    struct Token {
    public:
        /* The type of the Token. */
        TokenType type;
        /* The string value of the token (excluding the first '-' if Option or Flag (but including the second)). */
        std::string value;
    };

    /* The Tokenizer class, which acts as a stream returning one token at a time or allowing the user to put a token back on it. */
    class Tokenizer {
    private:
        /* The vector containing the input. */
        std::vector<std::string> input;
        /* Whether or not we have seen the executable yet. */
        bool executable_to_go;
        /* Determines whether we have seen '--' or not. */
        bool accepts_options;

    public:
        /* Constructor for the Tokenizer class, which takes the raw CLI-input. */
        Tokenizer(const int argc, const char** argv) :
            executable_to_go(true),
            accepts_options(true)
        {
            // If we get an empty stream, override executable_to_go to false
            if (argc == 0) {
                this->executable_to_go = false;
            }

            // Copy the argv strings to our own input vector. Note that we reverse it, as we treat this vector as a stack of sorts.
            this->input.resize((size_t) argc);
            for (int i = 0; i < argc; i++) {
                this->input.at(argc - 1 - i) = std::string(argv[i]);
            }
        }

        /* Returns and removed the first token on the stream in the given token object. If no more tokens are available, returns a Token with 'empty' TokenType. */
        Tokenizer& operator>>(Token& result) {
            // If there are no more tokens to get, give the token an empty value
            if (this->input.size() == 0) {
                result.type = TokenType::empty;
                result.value = "";
                return *this;
            }

            // Get the head of the vector in the Token
            result.value = this->input.at(this->input.size() - 1);
            this->input.pop_back();

            // Analyze its type
            if (this->executable_to_go) {
                // It's the executable!
                result.type = TokenType::executable;
                executable_to_go = false;
                return *this;
            } else if (this->accepts_options && result.value.size() >= 2 && result.value[0] == '-') {
                // It's a dash with at least something else; an Option
                result.type = TokenType::label;
                std::string label = result.value.substr(1);

                // Check if it's a shortlabel or a name and, if either, if it's valid
                if (label[0] != '-' && !is_valid_shortlabel(label[0])) {
                    throw IllegalShortlabelCharException(label[0]);
                } else if (label[0] == '-') {
                    // If we just see two '--', we mark it so and return the next token instead
                    if (label.size() == 1) {
                        this->accepts_options = false;
                        return *this >> result;
                    }

                    // Check if that name happens to be illegal
                    char result = is_valid_name(label.substr(1));
                    if (result != '\0') { throw IllegalNameCharException(result, label); }
                }

                // It's valid, so overwrite result.value with label and return
                result.value = label;
                return *this;
            } else {
                // We parse it as a normal value
                result.type = TokenType::value;
                return *this;
            }
        }
        /* Returns the first token on the stream as a new token object, but doesn't remove it from the stream. If no more tokens are available, returns a Token with 'empty' TokenType. */
        Token peek() const {
            Token result;

            // If there are no more tokens to get, give the token an empty value
            if (this->input.size() == 0) {
                result.type = TokenType::empty;
                result.value = "";
                return result;
            }

            // Get the head of the vector in the Token
            result.value = this->input.at(this->input.size() - 1);

            // Analyze its type
            if (this->executable_to_go) {
                // It's the executable!
                result.type = TokenType::executable;
                return result;
            } else if (this->accepts_options && result.value.size() >= 2 && result.value[0] == '-') {
                // It's a dash with at least something else; an Option
                result.type = TokenType::label;
                std::string label = result.value.substr(1);

                // Check if it's a shortlabel or a name and, if either, if it's valid
                if (label[0] != '-' && !is_valid_shortlabel(label[0])) {
                    throw IllegalShortlabelCharException(label[0]);
                } else if (label[0] == '-') {
                    // If we just see two '--', we return the next token instead
                    if (label.size() == 1) {
                        return this->peek();
                    }

                    // Check if that name happens to be illegal
                    char result = is_valid_name(label.substr(1));
                    if (result != '\0') { throw IllegalNameCharException(result, label); }
                }

                // It's valid, so overwrite result.value with label and return
                result.value = label;
                return result;
            } else {
                // We parse it as a normal value
                result.type = TokenType::value;
                return result;
            }
        }
        /* Only removes the first token on the stream, does not return it. Simply does nothing if no more tokens are available on the stream. */
        void pop() {
            if (this->input.size() > 0) {
                this->input.pop_back();
            }
        }

        /* Puts a given token back on the stream. */
        Tokenizer& operator<<(const Token& to_return) {
            const std::string context = "Tokenizer::operator<<(Token)";

            // If the type is empty, do nothing
            if (to_return.type == TokenType::empty) { return *this; }

            // If we have an executable lying on top, throw an error that we went too far
            if (this->executable_to_go) { throw DuplicateExecutableTokenException(context, this->input.at(this->input.size() - 1), to_return.value); }

            // Otherwise, be sure to possibly re-mark the executable flag if we're given one
            if (to_return.type == TokenType::executable) { this->executable_to_go = true; }

            // Get the to-be-added value & prepend it with a '-' if it's a Label
            std::string value = to_return.value;
            if (to_return.type == TokenType::label) {
                value = "-" + value;
            }

            // Push the value on the input stack
            this->input.push_back(value);

            // Done, return ourselves!
            return *this;
        }
        /* Puts a given string back on the stream as if it were a value token. */
        Tokenizer& operator<<(const std::string& to_return) {
            // If the value is equal to '--', do not add it but to set the accepts_options flag to true
            if (to_return == "--") {
                this->accepts_options = true;
                return *this;
            }

            // Otherwise, push the value to the input stack
            this->input.push_back(to_return);

            // Done, return ourselves!
            return *this;
        }

        /* If we've reached the end of the input, this returns true. */
        inline bool eof() const { return this->input.size() == 0; }
        /* Returns the number of tokens left in the stream. */
        inline size_t size() const { return this->input.size(); }

    };





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
        const std::any (*parse_func)(Tokenizer&);

        /* Constructor of the RuntimeType class which takes the name & hash of the target type, the assigned parser and the number of values. */
        RuntimeType(const char* type_name, const size_t type_hash, const std::any (*parse_func)(Tokenizer&)) :
            type_name(type_name),
            type_hash(type_hash),
            parse_func(parse_func)
        {}

        /* Returns true if this and the other type are the same. */
        inline bool operator==(const RuntimeType& other) const { return this->type_name == other.type_name && this->type_hash == other.type_hash && this->parse_func == other.parse_func; }
        /* Returns true if this and the other type are different. */
        inline bool operator!=(const RuntimeType& other) const { return this->type_name != other.type_name || this->type_hash != other.type_hash || this->parse_func != other.parse_func; }

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
    template <typename, std::any (*)(Tokenizer&), typename...>
    struct Type : public BaseType { };

    /* Template for types used in the ArgumentParser. Used to specify behaviour for a certain type. This specialization focusses on string literals, and should be used with STR_T(). */
    template <typename T, std::any (*PARSE_FUNC)(Tokenizer&), char... TYPENAME>
    struct Type<T, PARSE_FUNC, tstring<TYPENAME...>> : public BaseType {
        /* References the chosen type. */
        using type = T;
        /* The name / uid of the type. */
        static constexpr const char type_name[] = { TYPENAME..., '\0' };
        /* The parser for this type. */
        static constexpr const std::any (*parse_func)(Tokenizer&) = PARSE_FUNC;

        /* Generates a RuntimeType from this specific DerivedType. */
        static RuntimeType runtime() {
            return RuntimeType(this->type_name, typeid(this->type).hash_code(), this->parse_func);
        }
    };





    /******************** PARSERS ********************/

    /* Parser for signed integer types (signed char, short, int, long, long long). */
    template <class T, typename = std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value> >
    std::any parse_int(Tokenizer& input) {
        // Get the first token
        Token token;
        input >> token;
        if (token.type != TokenType::value) { throw NotEnoughValuesException(type_name<T>::value, 1, 0); }

        // Get an easy-to-use reference to the parsed string
        std::string& text = token.value;

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
                        throw OutOfTRangeException<T>(type_name<T>::value, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), text);
                    }
                } else {
                    // Negative check
                    if (result < std::numeric_limits<T>::min() / 10 || result * 10 < std::numeric_limits<T>::min() + value) {
                        throw OutOfTRangeException<T>(type_name<T>::value, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), "-" + text);
                    }
                }

                // Increment the result
                result *= 10;
                // Add the value
                result += modifier * value;
            } else {
                throw IllegalCharException(type_name<T>::value, c, "0-9 and '-'");
            }
        }

        // Return the number, modified
        return std::any(result);
    }

    /* Parser for unsigned integer types (unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long) */
    template <class T, typename = std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value> >
    std::any parse_uint(Tokenizer& input) {
        // Get the first token
        Token token;
        input >> token;
        if (token.type != TokenType::value) { throw NotEnoughValuesException(type_name<T>::value, 1, 0); }

        // Get an easy-to-use reference to the parsed string
        std::string& text = token.value;

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
                    throw OutOfTRangeException<T>(type_name<T>::value, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), text);
                }

                // Increment the result
                result *= 10;
                // Add the value
                result += value;
            } else {
                throw IllegalCharException(type_name<T>::value, c, "0-9");
            }
        }

        // Return the number, modified
        return std::any(result);
    }

    /* Parser for single-precision floating-point types. */
    std::any parse_float(Tokenizer& input) {
        // Get the first token
        Token token;
        input >> token;
        if (token.type != TokenType::value) { throw NotEnoughValuesException(type_name<float>::value, 1, 0); }

        // Get an easy-to-use reference to the parsed string
        std::string& text = token.value;

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
                throw IllegalCharException(type_name<float>::value, c, "0-9, '.' and '-'");
            }
        }

        // Run the text through C++'s default parser
        float result;
        try {
            result = std::stof(text);
        } catch (std::invalid_argument& e) {
            throw IllegalValueException(type_name<bool>::value, text, "floating-point numbers");
        } catch  (std::out_of_range& e) {
            throw OutOfTRangeException<float>(type_name<float>::value, std::numeric_limits<float>::min(), std::numeric_limits<float>::max(), text);
        }

        // Done, return
        return std::any(result);
    }

    /* Parser for double-precision floating-point types. */
    std::any parse_double(Tokenizer& input) {
        // Get the first token
        Token token;
        input >> token;
        if (token.type != TokenType::value) { throw NotEnoughValuesException(type_name<double>::value, 1, 0); }

        // Get an easy-to-use reference to the parsed string
        std::string& text = token.value;

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
                throw IllegalCharException(type_name<double>::value, c, "0-9, '.' and '-'");
            }
        }

        // Run the text through C++'s default parser
        double result;
        try {
            result = std::stod(text);
        } catch (std::invalid_argument& e) {
            throw IllegalValueException(type_name<bool>::value, text, "floating-point numbers");
        } catch (std::out_of_range& e) {
            throw OutOfTRangeException<double>(type_name<double>::value, std::numeric_limits<double>::min(), std::numeric_limits<double>::max(), text);
        }

        // Done, return
        return std::any(result);
    }

    /* Parser for booleans (they can be either: "true", "1", "yes", "y" for true or "false", "0", "no", "n" for false (capitalization-independent)) */
    std::any parse_bool(Tokenizer& input) {
        // Get the first token
        Token token;
        input >> token;
        if (token.type != TokenType::value) { throw NotEnoughValuesException(type_name<double>::value, 1, 0); }

        const char* text = token.value.c_str();
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
        throw IllegalValueException(type_name<bool>::value, text, "'true', 'false', 'y', 'yes', 'n', 'no', '1' or '0'");
    }

    /* Parser for single characters. Only errors if no character is given. */
    std::any parse_char(Tokenizer& input) {
        // Get the first token
        Token token;
        input >> token;
        if (token.type != TokenType::value) { throw NotEnoughValuesException(type_name<double>::value, 1, 0); }

        // Get an easy-to-use reference to the parsed string
        std::string& text = token.value;

        if (text.size() != 1) {
            throw StringSizeIncorrectException(type_name<char>::value, text, "single characters");
        }

        return std::any(text[0]);
    }

    /* Parser for multiple characters. Only errors if no character is given. */
    std::any parse_string(Tokenizer& input) {
        // Get the first token
        Token token;
        input >> token;
        if (token.type != TokenType::value) { throw NotEnoughValuesException(type_name<double>::value, 1, 0); }

        // Get an easy-to-use reference to the parsed string
        std::string& text = token.value;

        if (text.size() == 0) {
            throw StringTooSmallException(type_name<std::string>::value, text, 1);
        }

        return std::any(text);
    }





    /******************** DEFAULT TYPES ********************/

    /* Unsigned byte, which refer's to C's unsigned char. */
    using UByte = Type<unsigned char, parse_uint<unsigned char>, STR_T("unsigned byte")>;
    /* Signed byte, which refer's to C's signed char. */
    using Byte = Type<signed char, parse_int<signed char>, STR_T("byte")>;
    /* Unsigned short, which refer's to C's unsigned short. */
    using UShort = Type<unsigned short, parse_uint<unsigned short>, STR_T("unsigned short")>;
    /* Signed short, which refer's to C's signed short. */
    using Short = Type<short, parse_int<short>, STR_T("short")>;
    /* Unsigned int, which refer's to C's unsigned int. */
    using UInt = Type<unsigned int, parse_uint<unsigned int>, STR_T("unsigned int")>;
    /* Signed int, which refer's to C's signed int. */
    using Int = Type<int, parse_int<int>, STR_T("int")>;
    /* Unsigned long, which refer's to C's unsigned long. */
    using ULong = Type<unsigned long, parse_uint<unsigned long>, STR_T("unsigned long")>;
    /* Signed long, which refer's to C's signed long. */
    using Long = Type<long, parse_int<long>, STR_T("long")>;
    /* Unsigned long long, which refer's to C's unsigned long long. */
    using ULongLong = Type<unsigned long long, parse_uint<unsigned long long>, STR_T("unsigned long long")>;
    /* Signed long long, which refer's to C's signed long long. */
    using LongLong = Type<long long, parse_int<long long>, STR_T("long long")>;
    /* Float, which refers to C's float type. */
    using Float = Type<float, parse_float, STR_T("float")>;
    /* Double, which refers to C's double type. */
    using Double = Type<double, parse_double, STR_T("double")>;
    /* Boolean type, which can be used to interpret true/false, yes/no or 1/0. */
    using Bool = Type<bool, parse_bool, STR_T("bool")>;
    /* Char type, which is used to interpret a single character */
    using Char = Type<char, parse_char, STR_T("char")>;
    /* String type, which is used to interpret multiple characters. */
    using String = Type<std::string, parse_string, STR_T("string")>;





    /******************** THE RESULTING ARGUMENTS DICT ********************/

    /* The Arguments class, which will be used to let the user interact with all the parsed elements. */
    class Arguments {
    private:
        /* The ParsedArgument struct, which is used to store parsed arguments. */
        struct ParsedArgument {
            /* The name of the argument. */
            std::string name;
            /* The shortlabel of the argument. */
            char shortlabel;
            /* The RuntimeType of the Argument. */
            RuntimeType type;
            /* Whether the ParsedArgument can accept any number of arguments or not (both is_variadic and is_repeatable). */
            bool repeatable;

            /* The parsed value of the Argument, as vector to accomodate variadic arguments. */
            std::vector<std::any> values;

            /* Whether the ParsedArgument is explicitly given by the user (true) or if it's a default value (false). */
            bool is_given;

            /* Useful constructor for the ArgumentParser side of things. */
            ParsedArgument(const std::string& name, char shortlabel, const RuntimeType& type, bool repeatable, const std::any& value, bool is_given) :
                name(name),
                shortlabel(shortlabel),
                type(type),
                repeatable(repeatable),
                values(std::vector<std::any>({ value })),
                is_given(is_given)
            {}
        };

        /* Lists all items, sorted by their name. */
        std::unordered_map<std::string, ParsedArgument*> args;
        /* Allows arguments to be found by their shortname, if they have one. */
        std::unordered_map<char, ParsedArgument*> short_args;

        /* Returns the value from the given ParsedArgument, making sure it's non-variadic and performing some other checks. */
        template <class T>
        T _get(const std::string& context, ParsedArgument* arg) {
            // If the argument is variadic, let's error, as they must use getp
            if (arg->repeatable) { throw SingletonMismatchException(context, name); }

            // Try to return the type as the given type
            try {
                return std::any_cast<T>(arg->values[0]);
            } catch (std::bad_any_cast& e) {
                // Re-throw as TypeMismatchException
                std::string raw_name = typeid(T).name();
                throw TypeMismatchException(context, name, raw_name != "???" ? type_name<T>::value : raw_name, arg->type.type_name);
            }
        }
        /* Returns the value from the given ParsedArgument as if it was variadic. Also performs some other checks. */
        template <class T>
        std::vector<T> _getv(const std::string& context, ParsedArgument* arg) {
            // Try to cast each element in the vector, throwing TypeMismatchs errors if we couldn't.
            std::vector<T> result;
            result.reserve(arg->values.size());
            try {
                for (size_t i = 0; i < arg->values.size(); i++) {
                    result.push_back(std::any_cast<T>(arg->values[i]));
                }
            } catch (std::bad_any_cast& e) {
                // Re-throw as TypeMismatchException
                std::string raw_name = typeid(T).name();
                throw TypeMismatchException(context, name, raw_name != "???" ? type_name<T>::value : raw_name, arg->type.type_name);
            }
            
            // If we were successful, return the result
            return result;
        }

        /* Adds an argument. */
        void add_arg(const ParsedArgument& arg) {
            // First, check if we already have this argument (based on its name)
            std::unordered_map<std::string, ParsedArgument*>::const_iterator iter = this->args.find(arg.name);
            if (iter != this->args.end()) {
                if (arg.repeatable) {
                    // Append to the existing list of values, and then return as we're done
                    (*iter).second->values.insert((*iter).second->values.end(), arg.values.begin(), arg.values.end());
                    return;
                } else {
                    // Shouldn't be allowed!
                    throw DuplicateArgumentException(arg.name, arg.shortlabel);
                }
            }

            // Otherwise, add it as new, so create a locally managed copy of the ParsedArgument
            ParsedArgument* copy = new ParsedArgument(arg);

            // Insert it into the exists map (since it always exists if given)
            this->args.insert(std::make_pair(copy->name, copy));

            // Also insert it into the shortlabel map if it has a shortlabel
            if (arg.shortlabel != '\0') {
                this->short_args.insert(std::make_pair(copy->shortlabel, copy));
            }
        }

        /* Declare the ArgumentParser as friend. */
        friend class ArgumentParser;

    public:
        /* Default constructor for the Arguments class. Only available from the ArgumentParser. */
        Arguments()
        {}
        /* Copy constructor for the Arguments class. */
        Arguments(const Arguments& other) {
            // Copy everything from the args map using this->add_arg, since that does the trick really
            for (std::pair<std::string, ParsedArgument*> p : other.args) {
                this->add_arg(*(p.second));
            }
        }
        /* Move constructor for the Arguments class. */
        Arguments(Arguments&& other) :
            args(std::move(other.args)),
            short_args(std::move(other.short_args))
        {
            // Simply clear out the other's maps
            other.args.clear();
            other.short_args.clear();
        }
        /* Destructor for the Arguments class. */
        ~Arguments() {
            // Delete immediate as stealing doesn't leave any nullptrs
            for (std::pair<std::string, ParsedArgument*> p : this->args) {
                delete p.second;
            }
        }

        /* Returns true if this object contains an argument with the given name. */
        inline bool contains(const std::string& name) const { return this->args.find(name) != this->args.end(); }
        /* Returns true if this object contains an argument with the given shortlabel. */
        inline bool contains(char shortlabel) const { return this->short_args.find(shortlabel) != this->short_args.end(); }
        /* Returns true if the user explicitly specified the argument. Otherwise, the argument does not exist or was a default argument. */
        bool is_given(const std::string& name) const {
            std::unordered_map<std::string, ParsedArgument*>::const_iterator iter = this->args.find(name);
            return iter != this->args.end() && (*iter).second->is_given;
        }
        /* Returns true if the user explicitly specified the argument. Otherwise, the argument does not exist or was a default argument. */
        bool is_given(char shortlabel) const {
            std::unordered_map<char, ParsedArgument*>::const_iterator iter = this->short_args.find(shortlabel);
            return iter != this->short_args.end() && (*iter).second->is_given;
        }

        /* Returns the argument with given name as the given type. Will throw an TypeMismatchException if the type entered here does not match the type entered when registring the argument. */
        template<class T>
        T get(const std::string& name) const {
            // The context for this function
            const std::string context = "Arguments::get<" + std::string(type_name<T>::value) + ">(name) const";

            // Try to find the argument with that name
            std::unordered_map<std::string, ParsedArgument*>::const_iterator iter = this->args.find(name);
            if (iter == this->args.end()) { throw UnknownNameException(context, name); }

            // Return using the internal extraction method
            return this->_get(context, (*iter).second);
        }
        /* Returns the argument with given shortlabel as the given type. Will throw an TypeMismatchException if the type entered here does not match the type entered when registring the argument. */
        template<class T>
        T get(char shortlabel) const {
            // The context for this function
            const std::string context = "Arguments::get<" + std::string(type_name<T>::value) + ">(shortlabel) const";

            // Try to find the argument with that name
            std::unordered_map<char, ParsedArgument*>::const_iterator iter = this->short_args.find(shortlabel);
            if (iter == this->short_args.end()) { throw UnknownShortlabelException(context, shortlabel); }

            // Return using the internal extraction method
            return this->_get(context, (*iter).second);
        }
        /* Returns the argument with given name as the given type, but assumes that the argument that we request can be given more than once / can accept more than one value. The value is therefore repeated as a vector of given type, where each element is one of the times it was specified (in order). */
        template <class T>
        std::vector<T> getv(const std::string& name) const {
            // The context for this function
            const std::string context = "Arguments::getv<" + std::string(type_name<T>::value) + ">(name) const";

            // Try to find the argument with that name
            std::unordered_map<std::string, ParsedArgument*>::const_iterator iter = this->args.find(name);
            if (iter == this->args.end()) { throw UnknownNameException(context, name); }

            // Return using the internal extraction method
            return this->_getv(context, (*iter).second);
        }
        /* Returns the argument with given shortlabel as the given type, but assumes that the argument that we request can be given more than once / can accept more than one value. The value is therefore repeated as a vector of given type, where each element is one of the times it was specified (in order). */
        template <class T>
        std::vector<T> getv(char shortlabel) const {
            // The context for this function
            const std::string context = "Arguments::getv<" + std::string(type_name<T>::value) + ">(shortlabel) const";

            // Try to find the argument with that name
            std::unordered_map<std::string, ParsedArgument*>::const_iterator iter = this->short_args.find(shortlabel);
            if (iter == this->short_args.end()) { throw UnknownShortlabelException(context, shortlabel); }

            // Return using the internal extraction method
            return this->_getv(context, (*iter).second);
        }

        /* Returns the shortlabel of given argument if it has one. Returns '\0' if it doesn't. */
        char get_shortlabel(const std::string& name) const {
            const std::string context = "Arguments::get_shortlabel()";

            // Try to find argument
            std::unordered_map<std::string, ParsedArgument*>::const_iterator iter = this->args.find(name);
            if (iter == this->args.end()) {
                throw UnknownNameException(context, name);
            }

            // Return it
            return (*iter).second->shortlabel;
        }
        /* Returns the name of the argument with the given shortlabel. */
        std::string get_name(char shortlabel) const {
            const std::string context = "Arguments::get_name()";

            // Try to find argument
            std::unordered_map<char, ParsedArgument*>::const_iterator iter = this->short_args.find(shortlabel);
            if (iter == this->short_args.end()) {
                throw UnknownShortlabelException(context, shortlabel);
            }

            // Return its name
            return (*iter).second->name;
        }
        /* Returns whether or not given argument is repeatable, i.e., it can accept any number of arguments or can be specified more than once. */
        bool is_repeatable(const std::string& name) const {
            const std::string context = "Arguments::is_variadic(name)";

            // Try to find uid
            std::unordered_map<std::string, ParsedArgument*>::const_iterator iter = this->args.find(name);
            if (iter == this->args.end()) {
                throw UnknownNameException(context, name);
            }

            // Return it
            return (*iter).second->repeatable;
        }
        /* Returns whether or not given argument is repeatable, i.e., it can accept any number of arguments or can be specified more than once. */
        bool is_repeatable(char shortlabel) const {
            const std::string context = "Arguments::is_variadic(shortlabel)";

            // Try to find uid
            std::unordered_map<char, ParsedArgument*>::const_iterator iter = this->short_args.find(shortlabel);
            if (iter == this->short_args.end()) {
                throw UnknownShortlabelException(context, shortlabel);
            }

            // Return it
            return (*iter).second->repeatable;
        }
        /* Returns a copy of the RuntimeType of the argument, which can be used to learn its wrapped Type. */
        RuntimeType get_type(const std::string& name) const {
            const std::string context = "Arguments::get_type(name)";

            // Try to find uid
            std::unordered_map<std::string, ParsedArgument*>::const_iterator iter = this->args.find(name);
            if (iter == this->args.end()) {
                throw UnknownNameException(context, name);
            }

            // Return it
            return (*iter).second->type;
        }
        /* Returns a copy of the RuntimeType of the argument, which can be used to learn its wrapped Type. */
        RuntimeType get_type(char shortlabel) const {
            const std::string context = "Arguments::get_type(shortlabel)";

            // Try to find uid
            std::unordered_map<char, ParsedArgument*>::const_iterator iter = this->short_args.find(shortlabel);
            if (iter == this->short_args.end()) {
                throw UnknownShortlabelException(context, shortlabel);
            }

            // Return it
            return (*iter).second->type;
        }

        /* Copy assignment operator for the Arguments class. */
        inline Arguments& operator=(const Arguments& other) { return *this = Arguments(other); }
        /* Move assignment operator for the Arguments class. */
        Arguments& operator=(Arguments&& other) {
            if (this != &other) { swap(*this, other); }
            return *this;
        }
        /* Swap operator for the the Arguments class. */
        friend void swap(Arguments& a1, Arguments& a2);
    };

    /* Swap operator for the the Arguments class. */
    void swap(Arguments& a1, Arguments& a2) {
        using std::swap;

        swap(a1.args, a2.args);
        swap(a1.short_args, a2.short_args);
    }





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

        /* Allows derived classes to create the correct string for the usage string. */
        virtual std::string usage() const = 0;
        
        /* Returns the type of the Argument. */
        inline ArgumentType get_arg_type() const { return this->arg_type; }
        /* Returns if this Argument is actually an Atomic argument. */
        inline bool is_atomic() const { return this->arg_type >= ArgumentType::multi_argument; };

        /* Swap operator for the Argument class. */
        friend void swap(Argument& a1, Argument& a2);

        /* Allows derived classes of the Argument to copy themselves. */
        virtual Argument* copy() const = 0;

    };
    /* Swap operator for the Argument class. */
    void swap(Argument& a1, Argument& a2) {
        using std::swap;

        swap(a1.name, a2.name);
        swap(a1.arg_type, a2.arg_type);
    }



    /* The AtomicArgument class: forms the baseclass of all elementary, no-children arguments. */
    class AtomicArgument : public Argument {
    protected:
        /* The shortlabel of this Argument. This is optional for Options and Flags, and disabled for Positionals. */
        char shortlabel;
        /* A pointer to the Type used in this AtomicArgument. */
        RuntimeType type;
        
        /* Determines whether or not the AtomicArgument is optional, i.e., the user can omit specifying it. */
        bool optional;
        /* Determines whether of not the AtomicArgument can have any number of values following it (true) or just a single set (false). */
        bool variadic;
        /* The description of the AtomicArgument. Will be used to describe the argument when generate_help() is used. Arguments without description will not be displayed there (but will be displayed using generate_usage()). */
        std::string description;
        /* The category of the AtomicArgument. Will be used to group arguments when using generate_help(). Is 'Miscellaneous' by default. */
        std::string category;
        /* Stores the default value set for the AtomicArgument if there is one. Otherwise, will be equal to std::any(nullptr). */
        std::any default_value;

        /* Constructor for the AtomicArgument baseclass, which takes the name of the derived class and its (argument) type. */
        AtomicArgument(ArgumentType arg_type, const std::string& name, const char shortlabel, const RuntimeType& type) :
            Argument(arg_type, name),
            shortlabel(shortlabel),
            type(type),
            optional(false),
            variadic(false),
            description(""),
            category("Miscellaneous"),
            default_value(false)
        {
            const std::string context = "AtomicArgument()";

            // Check if the name is legal
            char valid = is_valid_name(this->name);
            if (valid != '\0') { throw IllegalNameException(context, name); }
            
            // Check if the shortlabel is legal
            if (!is_valid_shortlabel(shortlabel)) { throw IllegalShortlabelException(context, shortlabel); }
        }

        /* Constructor for the AtomicArgument baseclass which only takes the name of the derived class. Only used by Flags. */
        AtomicArgument(const std::string& name, const char shortlabel) :
            Argument(ArgumentType::flag, name),
            shortlabel(shortlabel),
            type(Bool::runtime()),
            optional(true),
            variadic(false),
            description(""),
            category("Miscellaneous"),
            default_value(false)
        {
            const std::string context = "AtomicArgument(name, shortlabel)";

            // Check if the name is legal
            char valid = is_valid_name(this->name);
            if (valid != '\0') { throw IllegalNameException(context, name); }
            
            // Check if the shortlabel is legal
            if (!is_valid_shortlabel(shortlabel)) { throw IllegalShortlabelException(context, shortlabel); }
        }

    public:
        /* Virtual destructor of the AtomicArgument class. */
        virtual ~AtomicArgument() = default;

        /* Checks if this AtomicArgument listens to the given name. */
        virtual bool has_name(const std::string& name) const { return this->name == name; }
        /* Returns whether or not this AtomicArgument has the given shortlabel. */
        virtual bool has_shortlabel(char shortlabel) const { return this->shortlabel == shortlabel; };

        /* Returns the shortlabel of this AtomicArgument. If the Argument has no shortlabel, '\0' is returned. */
        inline char get_shortlabel() const { return this->shortlabel; };
        /* Returns the name of the type associated with this AtomicArgument. */
        inline RuntimeType get_type() const { return this->type; }

        /* Sets whether or not the AtomicArgument is optional, i.e., can be omitted by the user. Returns a reference to the AtomicArgument to allow chaining. */
        virtual AtomicArgument& set_optional(bool optional) {
            this->optional = optional;
            return *this;
        }
        /* Gets whether or not the AtomicArgument is optional, i.e., can be omitted by the user. */
        inline bool is_optional() const { return this->optional; }

        /* Sets whether or not a single AtomicArgument can have any number of values following it (separated by spaces). */
        virtual AtomicArgument& set_variadic(bool is_variadic) {
            this->variadic = is_variadic;
            return *this;
        }
        /* Gets whether or not a single AtomicArgument can have any number of values following it. */
        inline bool is_variadic() const { return this->variadic; }

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

        /* Allows the ArgumentParser to delegate parsing to the correct AtomicArgument. Returns the value parsed for this AtomicArgument or, if the value wasn't for us, returns an empty std::any object. Suitable ParseExceptions may be thrown if the value was invalid somehow. */
        virtual std::any parse(size_t& n_positionals, Tokenizer& input) const = 0;

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
            AtomicArgument(ArgumentType::positional, name, '\0', type),
            index(index)
        {}

        /* Declare the ArgumentParser as friend so it can call the constructor. */
        friend class ArgumentParser;
        /* Declare the MultiArgument as friend so it can call the constructor. */
        friend class MultiArgument;
    
    public:
        /* Sets whether or not the Positional is optional, i.e., can be omitted by the user. Returns a reference to the Positional to allow chaining. */
        virtual Positional& set_optional(bool optional) { return (Positional&) AtomicArgument::set_optional(optional); }
        /* Sets whether or not a single Positional can have any number of values following it (separated by spaces). Do not that only the last Positional can have this feature enabled. */
        virtual Positional& set_variadic(bool is_variadic) { return (Positional&) AtomicArgument::set_variadic(is_variadic); }
        /* Sets the description of the Positional. Returns a reference to the Positional to allow chaining. */
        virtual Positional& set_description(const std::string& description) { return (Positional&) AtomicArgument::set_description(description); }
        /* Sets the category of the Positional. Returns a reference to the Positional to allow chaining. */
        virtual Positional& set_category(const std::string& category) { return (Positional&) AtomicArgument::set_category(category); }

        /* Allows derived classes to create the correct string for the usage string. */
        virtual std::string usage() const {
            // Simply return our own name
            return this->name;
        }

        /* Clears the default value of the Positional if it had any. Returns a reference to the Positional to allow chaining. */
        virtual Positional& clear_default() { return (Positional&) AtomicArgument::clear_default(); }
        /* Sets the default value of this Positional. Returns a reference to the Positional to allow chaining. */
        virtual Positional& set_default(const std::any& default_value) { return (Positional&) AtomicArgument::set_default(default_value); }

        /* Gets the CLI-relevant index of the Positional. */
        inline size_t get_index() const { return this->index; }

        /* Allows the ArgumentParser to delegate parsing to this Positional. Returns the value parsed for this AtomicArgument or, if the value wasn't for us, returns an empty std::any object. Suitable ParseExceptions may be thrown if the value was invalid somehow. */
        virtual std::any parse(size_t& n_positionals, Tokenizer& input) const {
            // First, check if we could be looking at a Positional
            Token token = input.peek();
            if (token.type != TokenType::value) {
                // Not for us, so return the empty any
                return std::any();
            }

            // Otherwise, check if it's our turn as a Positional
            if (n_positionals != this->index) {
                // Not for us, so return the empty any
                return std::any();
            }

            // If it is a value, however, it's for us, so try to parse it using our internal parser!
            ++n_positionals;
            try {
                return this->type.parse_func(input);
            } catch (TypeParseException& e) {
                e.insert(this->name);
                throw;
            }
        }

        /* Allows the Positional to copy itself polymorphically. */
        virtual Positional* copy() const { return new Positional(*this); }

    };

    /* The Option class: an argument with a label and a value. */
    class Option : public AtomicArgument {
    private:
        /* Determines whether or not the same Option can be specified multiple times. Output resembles is_variadic, and if used in conjunction then one big (in-order) vector of values is returned. */
        bool repeatable;

        /* Constructor for the Option class, which takes the name, the shortlabel for it and its type as RuntimeType. */
        Option(const std::string& name, char shortlabel, const RuntimeType& type) :
            AtomicArgument(ArgumentType::option, name, shortlabel, type),
            repeatable(false)
        {}

        /* Declare the ArgumentParser as friend so it can call the constructor. */
        friend class ArgumentParser;
        /* Declare the MultiArgument as friend so it can call the constructor. */
        friend class MultiArgument;
    
    public:
        /* Sets whether or not the Option is optional, i.e., can be omitted by the user. Returns a reference to the Option to allow chaining. */
        virtual Option& set_optional(bool optional) { return (Option&) AtomicArgument::set_optional(optional); }
        /* Sets whether or not a single Option can have any number of values following it (separated by spaces). */
        virtual Option& set_variadic(bool is_variadic) { return (Option&) AtomicArgument::set_variadic(is_variadic); }
        /* Sets the description of the Option. Returns a reference to the Option to allow chaining. */
        virtual Option& set_description(const std::string& description) { return (Option&) AtomicArgument::set_description(description); }
        /* Sets the category of the Option. Returns a reference to the Option to allow chaining. */
        virtual Option& set_category(const std::string& category) { return (Option&) AtomicArgument::set_category(category); }

        /* Allows the Positional to correctly describe its own name. */
        virtual std::string usage() const {
            std::stringstream sstr;

            // Return our shortlabel first if we have one, otherwise do the long one
            if (this->shortlabel != '\0') {
                sstr << '-' << this->shortlabel << ' ';
            } else {
                sstr << "--" << this->name << ' ';
            }

            // Then, append the placeholder (in caps)
            sstr << upperify(this->type.type_name);

            // Done, so return the build string
            return sstr.str();
        }

        /* Clears the default value of the Option if it had any. Returns a reference to the Option to allow chaining. */
        virtual Option& clear_default() { return (Option&) AtomicArgument::clear_default(); }
        /* Sets the default value of this Option. Returns a reference to the Option to allow chaining. */
        virtual Option& set_default(const std::any& default_value) { return (Option&) AtomicArgument::set_default(default_value); }

        /* Sets whether or not the Option can be specified multiple times. */
        virtual Option& set_repeatable(bool is_repeatable) {
            this->repeatable = is_repeatable;
            return *this;
        }
        /* Gets whether or not the Option can be specified multiple times. */
        inline bool is_repeatable() const { return this->repeatable; }

        /* Allows the ArgumentParser to delegate parsing to this Option. Returns the value parsed for this AtomicArgument or, if the value wasn't for us, returns an empty std::any object. Suitable ParseExceptions may be thrown if the value was invalid somehow. */
        virtual std::any parse(size_t&, Tokenizer& input) const {
            // First, check if we could be looking at an Option
            Token token = input.peek();
            if (token.type != TokenType::label) {
                // Not for us, so return the empty any
                return std::any();
            }

            // Then, check if the label belongs to us
            if (  (token.value.size() == 1 && token.value[0] != this->shortlabel) || 
                  (token.value.size() > 1 && token.value != "-" + this->name)) {
                // Not for us, so return the empty any
                return std::any();
            }

            // Otherwise, remove the label from the stream and try to parse the rest as values using our internal parser
            input.pop();
            try {
                return this->type.parse_func(input);
            } catch (TypeParseException& e) {
                e.insert(this->name, this->shortlabel);
                throw;
            }
        }

        /* Allows the Option to copy itself polymorphically. */
        virtual Option* copy() const { return new Option(*this); }

    };

    /* The Flag class: an argument with only a label, not a value. This can be used as a way to pass booleans. */
    class Flag : public AtomicArgument {
    private:
        /* Constructor for the Flag class, which takes the name and the shortlabel of the Argument. */
        Flag(const std::string& name, char shortlabel) :
            AtomicArgument(name, shortlabel)
        {}

        /* Declare the ArgumentParser as friend so it can call the constructor. */
        friend class ArgumentParser;
        /* Declare the MultiArgument as friend so it can call the constructor. */
        friend class MultiArgument;
    
    public:
        /* This function is disabled for Flags, since they're always optional. Throws a ValueTypeMismatchException when called. */
        virtual Flag& set_optional(bool) { throw ValueTypeMismatchException("Flag::set_optional()", this->name); }
        /* This function is disabled for Flags, since flags don't have any value. Throws a ValueTypeMismatchException when called. */
        virtual Flag& set_variadic(bool) { throw ValueTypeMismatchException("Flag::set_variadic()", this->name); }
        /* Sets the description of the Flag. Returns a reference to the Flag to allow chaining. */
        virtual Flag& set_description(const std::string& description) { return (Flag&) AtomicArgument::set_description(description); }
        /* Sets the category of the Flag. Returns a reference to the Flag to allow chaining. */
        virtual Flag& set_category(const std::string& category) { return (Flag&) AtomicArgument::set_category(category); }

        /* Allows the Positional to correctly describe its own name. */
        virtual std::string usage() const {
            std::stringstream sstr;

            // Return our shortlabel first if we have one, otherwise do the long one
            if (this->shortlabel != '\0') {
                sstr << '-' << this->shortlabel;
            } else {
                sstr << "--" << this->name;
            }

            // Done, so return the build string
            return sstr.str();
        }

        /* This function is disabled for Flags, since Flag's default value must always be set to true. Throws a ValueTypeMismatchException when called. */
        virtual Flag& clear_default() { throw ValueTypeMismatchException("Flag::clear_default()", this->name); }
        /* This function is disabled for Flags, since Flag's default value must always be set to true. Throws a ValueTypeMismatchException when called. */
        virtual Flag& set_default(const std::any&) { throw ValueTypeMismatchException("Flag::set_default()", this->name); }

        /* Allows the ArgumentParser to delegate parsing to this Flag. Returns the value parsed for this AtomicArgument or, if the value wasn't for us, returns an empty std::any object. Suitable ParseExceptions may be thrown if the value was invalid somehow. */
        virtual std::any parse(size_t&, Tokenizer& input) const {
            // First, check if we could be looking at a Flag
            Token token = input.peek();
            if (token.type != TokenType::label) {
                // Not for us, so return the empty any
                return std::any();
            }

            // Then, check if the label belongs to us
            if (  (token.value.size() == 1 && token.value[0] != this->shortlabel) || 
                  (token.value.size() > 1 && token.value != "-" + this->name)) {
                // Not for us, so return the empty any
                return std::any();
            }

            // Otherwise, we did find it!
            return std::any(true);
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
        /* Keeps track of the type of AtomicArgument stored in this MultiArgument. */
        MemberType member_type;

        /* Function that validates if a given Argument can be added based on the specific MultiArguments' rules, and then adds it. Throws errors if an illegal element is added this way. */
        virtual void _add_validated(const std::string&, Argument* arg) {
            // The MultiArgument is very inclusive, and accepts everything
            this->args.push_back(arg);
        }

        /* Constructor for the MultiArgument which takes a name and an ArgumentType. */
        MultiArgument(ArgumentType arg_type, MultiArgument* root, const std::string& name, MemberType member_type) :
            Argument(arg_type, name),
            root(root),
            member_type(member_type)
        {}

        friend class IncludedGroup;
        friend class ExcludedGroup;
        friend class RequiredGroup;

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

            // Check if the name exists
            if (this->root->has_name(name)) {
                throw DuplicateNameException(context, name);
            }

            // Check how many Positionals there are in total (deep search from the root)
            size_t n_positionals = this->root->deepsearch<Positional>().size();

            // Create a new Positional object with the index = the number of n_positionals
            Positional* p = new Positional(name, n_positionals, T::runtime());

            // Add it using the class' virtualized _add_validated() function
            this->_add_validated(context, (Argument*) p);
            
            // Return it
            return *p;
        }
        /* Adds a new optional argument to the parser (an argument with a label and a value) of DerivedType T. A reference to the new parameter is returned to change its properties. */
        template <typename T, typename = std::enable_if_t<std::is_base_of<BaseType, T>::value> >
        Option& add_option(char shortlabel, const std::string& name) {
            const std::string context = "MultiArgument::add_option()";
            
            // Check if either the name or the shortlabel exists
            if (this->root->has_name(name)) {
                throw DuplicateNameException(context, name);
            } else if (this->root->has_shortlabel(shortlabel)) {
                throw DuplicateShortlabelException(context, shortlabel);
            }

            // Create a new Option object and add it
            Option* o = new Option(name, shortlabel, T::runtime());

            // Add it using the class' virtualized _add_validated() function
            this->_add_validated(context, (Argument*) o);
            
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

            // Add it using the class' virtualized _add_validated() function
            this->_add_validated(context, (Argument*) f);

            // Return it
            return *f;
        }
        /* Adds a new IncludedGroup to the parser (a collection of arguments for which holds that if one appears, all of them must appear). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. The given member_type is the type that is allowed in the group's list of members and must be one of the three AtomicArgument types. A reference to the new group is returned to change its properties and add children. */
        IncludedGroup& add_included(const std::string& name, MemberType member_type) {
            const std::string context = "MultiArgument::add_included()";

            // Check if the given name exists by getting a list of all MultiArguments (nested) from the root and checking their name
            for (MultiArgument* marg : this->root->deepsearch<MultiArgument>()) {
                if (marg->get_name() == name) {
                    throw DuplicateNameException(context, name);
                }
            }

            // Create a new IncludedGroup object
            IncludedGroup* ig = new IncludedGroup(this->root, name, member_type);

            // Add it using the class' virtualized _add_validated() function
            this->_add_validated(context, (Argument*) ig);
            
            // Return it
            return *ig;
        }
        /* Adds a new ExcludedGroup to the parser (a collection of arguments for which holds that if one appears, none other can appear). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. The given member_type is the type that is allowed in the group's list of members and must be one of the three AtomicArgument types. A reference to the new group is returned to change its properties and add children. */
        ExcludedGroup& add_excluded(const std::string& name, MemberType member_type) {
            const std::string context = "MultiArgument::add_excluded()";

            // Check if the given name exists by getting a list of all MultiArguments (nested) from the root and checking their name
            for (MultiArgument* marg : this->root->deepsearch<MultiArgument>()) {
                if (marg->get_name() == name) {
                    throw DuplicateNameException(context, name);
                }
            }

            // Create a new ExcludedGroup object
            ExcludedGroup* eg = new ExcludedGroup(this->root, name, member_type);

            // Add it using the class' virtualized _add_validated() function
            this->_add_validated(context, (Argument*) eg);
            
            // Return it
            return *eg;
        }
        /* Adds a new RequiredGroup to the parser (a collection of arguments for which holds that an argument can only appear if the previous appears). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. The given member_type is the type that is allowed in the group's list of members and must be one of the three AtomicArgument types. A reference to the new group is returned to change its properties and add children. */
        RequiredGroup& add_required(const std::string& name, MemberType member_type) {
            const std::string context = "MultiArgument::add_required()";

            // Check if the given name exists by getting a list of all MultiArguments (nested) from the root and checking their name
            for (MultiArgument* marg : this->root->deepsearch<MultiArgument>()) {
                if (marg->get_name() == name) {
                    throw DuplicateNameException(context, name);
                }
            }

            // Create a new ExcludedGroup object
            RequiredGroup* rg = new RequiredGroup(this->root, name, member_type);

            // Add it using the class' virtualized _add_validated() function
            this->_add_validated(context, (Argument*) rg);
            
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
                    if (!dynamic_cast<T*>(arg)) { throw ArgumentTypeException(context, name, T::type_name, arg->get_arg_type()); }
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
                    if (!dynamic_cast<T*>(arg)) { throw ArgumentTypeException(context, name, T::type_name, arg->get_arg_type()); }
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

        /* Returns whether or not anything from this MultiArgument is given based on the given Arguments dict. */
        virtual bool is_given(const Arguments& parsed_args) const {
            for (size_t i = 0; i < this->args.size(); i++) {
                Argument* arg = this->args[i];
                if (arg->is_atomic()) {
                    if (parsed_args.is_given(arg->get_name())) { return true; }
                } else {
                    // Recursively call is_given
                    if (((MultiArgument*) arg)->is_given(parsed_args)) { return true; }
                }
            }
        }
        /* Lets the child classes validate whether their specific dependency rule is met, based on the given resulting Arguments dict. If the validation failed, and appropriate exception is thrown. */
        virtual void validate(const Arguments& parsed_args) const { /* Being only a group of arguments, we always allow! */ }

        /* Allows the nested elements to correctly describe their names. */
        virtual std::string usage() const {
            std::stringstream posses;
            std::stringstream opts;
            std::stringstream flags;

            // First, print all elements in this MultiArgument, in a per-argument_type sort of way
            for (size_t i = 0; i < this->args.size(); i++) {
                Argument* arg = this->args[i];

                // Write the argument to the correct stringstream
                if (arg->get_arg_type() == ArgumentType::positional || (!arg->is_atomic() && ((MultiArgument*) arg)->get_member_type() == MemberType::positional)) {
                    posses << ' ' << (arg->usage());
                } else if (arg->get_arg_type() == ArgumentType::option || (!arg->is_atomic() && ((MultiArgument*) arg)->get_member_type() == MemberType::option)) {
                    opts << ' ' << (arg->usage());
                } else if (arg->get_arg_type() == ArgumentType::option || (!arg->is_atomic() && ((MultiArgument*) arg)->get_member_type() == MemberType::option)) {
                    // If we've already written something, prefix it with a space
                    if (flags.tellp() != 0) { flags << ' '; }
                    flags << (arg->usage());
                }
            }

            // Done, now tie all three strings together and return
            std::string result = flags.str();
            if (opts.tellp() != 0) { result += opts.str(); }
            if (posses.tellp() != 0) { result += posses.str(); }
            return result;
        }

        /* Returns the type of the argument stored in this MultiArgument. */
        inline MemberType get_member_type() const { return this->member_type; }

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
    protected:
        /* Function that validates if a given Argument can be added based on the specific MultiArguments' rules, and then adds it. Throws errors if an illegal element is added this way. */
        virtual void _add_validated(const std::string& context, Argument* arg) {
            // The IncludedGroup only accepts Positionals with adjacent indices and Options, but not flags and not intermixed
            if (arg->is_atomic()) {
                AtomicArgument* aarg = (AtomicArgument*) arg;

                // Filter out specific types
                if (aarg->get_arg_type() == ArgumentType::flag) {
                    // We never accept flags as an IncludedGroup
                    throw IllegalGroupTypeException(context, this->name, this->arg_type, aarg->get_name(), aarg->get_arg_type());
                } else if (this->args.size() > 0 && aarg->get_arg_type() == ArgumentType::positional) {
                    Positional* pos = (Positional*) aarg;

                    // Get the maximum index from all (nested) positionals
                    size_t max = std::numeric_limits<size_t>::min();
                    for (Positional* pos2 : this->deepsearch<Positional>()) {
                        if (pos2->get_index() > max) { max = pos2->get_index(); }
                    }

                    // We only accept Positionals if they are coming sequentially after the Positionals in here
                    if (pos->get_index() > max + 1) {
                        throw NonSequentialPositionalException(context, this->name, this->arg_type, max, pos->get_name(), pos->get_index());
                    }
                }

                // Make sure our member_type matches
                if (this->member_type != (MemberType) aarg->get_arg_type()) {
                    throw MemberTypeMismatchException(context, this->name, this->arg_type, this->member_type, aarg->get_name(), aarg->get_arg_type());
                }
            }

            // If we made it this far, it all makes sense, so add the argument to the group!
            this->args.push_back(arg);
        }

    public:
        /* Constructor for the IncludedGroup class, which takes the root MultiArgument and a name. */
        IncludedGroup(MultiArgument* root, const std::string& name, MemberType member_type) :
            MultiArgument(ArgumentType::included_group, root, name, member_type)
        {}

        /* The add_included function that allows one to specify a custom member_type cannot be used in the IncludedGroup, as any nested MultiGroups must have the same member type as the parent group. */
        inline IncludedGroup& add_included(const std::string& name, MemberType member_type) { throw MemberTypeConflictException("IncludedGroup::add_included()", this->name, this->arg_type, name, ArgumentType::included_group); }
        /* The add_excluded function that allows one to specify a custom member_type cannot be used in the IncludedGroup, as any nested MultiGroups must have the same member type as the parent group. */
        inline ExcludedGroup& add_excluded(const std::string& name, MemberType member_type) { throw MemberTypeConflictException("IncludedGroup::add_excluded()", this->name, this->arg_type, name, ArgumentType::excluded_group); }
        /* The add_required function that allows one to specify a custom member_type cannot be used in the IncludedGroup, as any nested MultiGroups must have the same member type as the parent group. */
        inline RequiredGroup& add_required(const std::string& name, MemberType member_type) { throw MemberTypeConflictException("IncludedGroup::add_required()", this->name, this->arg_type, name, ArgumentType::required_group); }
        
        /* Adds a new IncludedGroup to the parser (a collection of arguments for which holds that if one appears, all of them must appear). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. A reference to the new group is returned to change its properties and add children. */
        inline IncludedGroup& add_included(const std::string& name) { return MultiArgument::add_included(name, this->member_type); }
        /* Adds a new ExcludedGroup to the parser (a collection of arguments for which holds that if one appears, none other can appear). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. The given member_type is the type that is allowed in the group's list of members and must be one of the three AtomicArgument types. A reference to the new group is returned to change its properties and add children. */
        inline ExcludedGroup& add_excluded(const std::string& name) { return MultiArgument::add_excluded(name, this->member_type); }
        /* Adds a new RequiredGroup to the parser (a collection of arguments for which holds that an argument can only appear if the previous appears). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. The given member_type is the type that is allowed in the group's list of members and must be one of the three AtomicArgument types. A reference to the new group is returned to change its properties and add children. */
        inline RequiredGroup& add_required(const std::string& name) { return MultiArgument::add_required(name, this->member_type); }
        
        /* Allows the nested elements to correctly describe their names. */
        virtual std::string usage() const {
            std::stringstream sstr;

            // Return all elements wrapped in a single '[]'
            sstr << '[';
            for (size_t i = 0; i < this->args.size(); i++) {
                if (i > 0) { sstr << ' '; }
                sstr << (this->args[i]->usage());
            }
            sstr << ']';

            // Done, so return the build string
            return sstr.str();
        }

        /* Lets this IncludedGroup validate whether all of their arguments are given, based on the given resulting Arguments dict. If the validation failed, and appropriate exception is thrown. */
        virtual void validate(const Arguments& parsed_args) const {
            std::string peer;
            for (size_t i = 0; i < this->args.size(); i++) {
                Argument* arg = this->args[i];
                
                // Check if it occurs, either directly as AtomicArgument or indirectly using a recursive-like is_given
                if (    (arg->is_atomic() && parsed_args.is_given(arg->get_name())) ||
                        (!arg->is_atomic() && ((MultiArgument*) arg)->is_given(parsed_args))) {
                    peer = arg->get_name();
                } else if (!peer.empty()) {
                    // One given, but also at least one is missing! Throw the appropriate exception
                    throw IncludedDependencyException(arg->get_name(), peer);
                }
            }
        }

    };

    /* The ExcludedGroup class: when the user specifies one of this classes' children, he or she cannot specify any other. */
    class ExcludedGroup : public MultiArgument {
    protected:
        /* Function that validates if a given Argument can be added based on the specific MultiArguments' rules, and then adds it. Throws errors if an illegal element is added this way. */
        virtual void _add_validated(const std::string& context, Argument* arg) {
            // The ExcludedGroup only accepts Options and flags, but no Positionals (as we couldn't distinguish those) and not intermixed
            if (arg->is_atomic()) {
                AtomicArgument* aarg = (AtomicArgument*) arg;

                // Filter out specific types
                if (aarg->get_arg_type() == ArgumentType::positional) {
                    // We never accept flags as an IncludedGroup
                    throw IllegalGroupTypeException(context, this->name, this->arg_type, aarg->get_name(), aarg->get_arg_type());
                }

                // Make sure our member_type matches
                if (this->member_type != (MemberType) aarg->get_arg_type()) {
                    throw MemberTypeMismatchException(context, this->name, this->arg_type, this->member_type, aarg->get_name(), aarg->get_arg_type());
                }
            }

            // If we made it this far, it all makes sense, so add the argument to the group!
            this->args.push_back(arg);
        }

    public:
        /* Constructor for the ExcludedGroup class, which takes the root MultiArgument and a name. */
        ExcludedGroup(MultiArgument* root, const std::string& name, MemberType member_type) :
            MultiArgument(ArgumentType::included_group, root, name, member_type)
        {}

        /* The add_included function that allows one to specify a custom member_type cannot be used in the ExcludedGroup, as any nested MultiGroups must have the same member type as the parent group. */
        inline IncludedGroup& add_included(const std::string& name, MemberType member_type) { throw MemberTypeConflictException("ExcludedGroup::add_included()", this->name, this->arg_type, name, ArgumentType::included_group); }
        /* The add_excluded function that allows one to specify a custom member_type cannot be used in the ExcludedGroup, as any nested MultiGroups must have the same member type as the parent group. */
        inline ExcludedGroup& add_excluded(const std::string& name, MemberType member_type) { throw MemberTypeConflictException("ExcludedGroup::add_excluded()", this->name, this->arg_type, name, ArgumentType::excluded_group); }
        /* The add_required function that allows one to specify a custom member_type cannot be used in the ExcludedGroup, as any nested MultiGroups must have the same member type as the parent group. */
        inline RequiredGroup& add_required(const std::string& name, MemberType member_type) { throw MemberTypeConflictException("ExcludedGroup::add_required()", this->name, this->arg_type, name, ArgumentType::required_group); }
        
        /* Adds a new IncludedGroup to the parser (a collection of arguments for which holds that if one appears, all of them must appear). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. A reference to the new group is returned to change its properties and add children. */
        inline IncludedGroup& add_included(const std::string& name) { return MultiArgument::add_included(name, this->member_type); }
        /* Adds a new ExcludedGroup to the parser (a collection of arguments for which holds that if one appears, none other can appear). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. The given member_type is the type that is allowed in the group's list of members and must be one of the three AtomicArgument types. A reference to the new group is returned to change its properties and add children. */
        inline ExcludedGroup& add_excluded(const std::string& name) { return MultiArgument::add_excluded(name, this->member_type); }
        /* Adds a new RequiredGroup to the parser (a collection of arguments for which holds that an argument can only appear if the previous appears). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. The given member_type is the type that is allowed in the group's list of members and must be one of the three AtomicArgument types. A reference to the new group is returned to change its properties and add children. */
        inline RequiredGroup& add_required(const std::string& name) { return MultiArgument::add_required(name, this->member_type); }
        
        /* Allows the nested elements to correctly describe their names. */
        virtual std::string usage() const {
            std::stringstream sstr;

            // Return all elements wrapped in a single '[]', but with '|' to indicate their exclusiveness
            sstr << '[';
            for (size_t i = 0; i < this->args.size(); i++) {
                if (i > 0) { sstr << " | "; }
                sstr << (this->args[i]->usage());
            }
            sstr << ']';

            // Done, so return the build string
            return sstr.str();
        }

        /* Lets this ExcludedGroup validate whether all of their arguments are given, based on the given resulting Arguments dict. If the validation failed, and appropriate exception is thrown. */
        virtual void validate(const Arguments& parsed_args) const {
            std::string peer;
            for (size_t i = 0; i < this->args.size(); i++) {
                Argument* arg = this->args.at(i);
                
                // Check if it occurs, either directly as AtomicArgument or indirectly using a recursive-like is_given
                if (    (arg->is_atomic() && parsed_args.is_given(arg->get_name())) ||
                        (!arg->is_atomic() && ((MultiArgument*) arg)->is_given(parsed_args))) {
                    if (peer.empty()) {
                        peer = arg->get_name();
                    } else {
                        // More than one given! Throw the appropriate exception
                        throw ExcludedDependencyException(arg->get_name(), peer);
                    }
                }
            }
        }

    };

    /* The RequiredGroup class: the user must specify the preceding argument before the following argument can be given. */
    class RequiredGroup : public MultiArgument {
    protected:
        /* Function that validates if a given Argument can be added based on the specific MultiArguments' rules, and then adds it. Throws errors if an illegal element is added this way. */
        virtual void _add_validated(const std::string& context, Argument* arg) {
            // The RequiredGroup only accepts Options and Flags, but no Positionals (as they are implicitly required already) and not intermixed
            if (arg->is_atomic()) {
                AtomicArgument* aarg = (AtomicArgument*) arg;

                // Filter out specific types
                if (aarg->get_arg_type() == ArgumentType::positional) {
                    // We never accept flags as an IncludedGroup
                    throw IllegalGroupTypeException(context, this->name, this->arg_type, aarg->get_name(), aarg->get_arg_type());
                }

                // Make sure our member_type matches
                if (this->member_type != (MemberType) aarg->get_arg_type()) {
                    throw MemberTypeMismatchException(context, this->name, this->arg_type, this->member_type, aarg->get_name(), aarg->get_arg_type());
                }
            }

            // If we made it this far, it all makes sense, so add the argument to the group!
            this->args.push_back(arg);
        }

    public:
        /* Constructor for the RequiredGroup class, which takes the root MultiArgument and a name. */
        RequiredGroup(MultiArgument* root, const std::string& name, MemberType member_type) :
            MultiArgument(ArgumentType::included_group, root, name, member_type)
        {}

        /* The add_included function that allows one to specify a custom member_type cannot be used in the RequiredGroup, as any nested MultiGroups must have the same member type as the parent group. */
        inline IncludedGroup& add_included(const std::string& name, MemberType member_type) { throw MemberTypeConflictException("RequiredGroup::add_included()", this->name, this->arg_type, name, ArgumentType::included_group); }
        /* The add_excluded function that allows one to specify a custom member_type cannot be used in the RequiredGroup, as any nested MultiGroups must have the same member type as the parent group. */
        inline ExcludedGroup& add_excluded(const std::string& name, MemberType member_type) { throw MemberTypeConflictException("RequiredGroup::add_excluded()", this->name, this->arg_type, name, ArgumentType::excluded_group); }
        /* The add_required function that allows one to specify a custom member_type cannot be used in the RequiredGroup, as any nested MultiGroups must have the same member type as the parent group. */
        inline RequiredGroup& add_required(const std::string& name, MemberType member_type) { throw MemberTypeConflictException("RequiredGroup::add_required()", this->name, this->arg_type, name, ArgumentType::required_group); }
        
        /* Adds a new IncludedGroup to the parser (a collection of arguments for which holds that if one appears, all of them must appear). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. A reference to the new group is returned to change its properties and add children. */
        inline IncludedGroup& add_included(const std::string& name) { return MultiArgument::add_included(name, this->member_type); }
        /* Adds a new ExcludedGroup to the parser (a collection of arguments for which holds that if one appears, none other can appear). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. The given member_type is the type that is allowed in the group's list of members and must be one of the three AtomicArgument types. A reference to the new group is returned to change its properties and add children. */
        inline ExcludedGroup& add_excluded(const std::string& name) { return MultiArgument::add_excluded(name, this->member_type); }
        /* Adds a new RequiredGroup to the parser (a collection of arguments for which holds that an argument can only appear if the previous appears). The given name must be unique among all groups, but can be a duplicate of an AtomicArgument. The given member_type is the type that is allowed in the group's list of members and must be one of the three AtomicArgument types. A reference to the new group is returned to change its properties and add children. */
        inline RequiredGroup& add_required(const std::string& name) { return MultiArgument::add_required(name, this->member_type); }
        
        /* Allows the nested elements to correctly describe their names. */
        virtual std::string usage() const {
            std::stringstream sstr;

            // Return all elements in their own '[]', but in such a way that it indicates their requiredness
            /* TBD */
            sstr << '[';
            for (size_t i = 0; i < this->args.size(); i++) {
                if (i > 0) { sstr << " | "; }
                sstr << (this->args[i]->usage());
            }
            sstr << ']';

            // Done, so return the build string
            return sstr.str();
        }

        /* Lets this RequiredGroup validate whether all of their arguments are given, based on the given resulting Arguments dict. If the validation failed, and appropriate exception is thrown. */
        virtual void validate(const Arguments& parsed_args) const {
            std::string last_arg;
            std::string peer;
            for (size_t i = 0; i < this->args.size(); i++) {
                Argument* arg = this->args.at(i);
                
                // Check if it occurs, either directly as AtomicArgument or indirectly using a recursive-like is_given
                if (    (arg->is_atomic() && parsed_args.is_given(arg->get_name())) ||
                        (!arg->is_atomic() && ((MultiArgument*) arg)->is_given(parsed_args))) {
                    if (last_arg == peer) {
                        // The last one is still on track, so allow this one and set it as peer
                        peer = arg->get_name();
                    } else {
                        // Not on track! Throw the appropriate exception
                        throw RequiredDependencyException(arg->name, peer);
                    }
                }
                last_arg = arg->get_name();
            }
        }

    };





    /******************** THE ARGUMENTPARSER CLASS ITSELF ********************/

    /* The ArgumentParser class, which can be used to parse arguments in a high-level, easy-to-use way. */
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
        inline Positional& add_positional(const std::string& name) { return args.add_positional<T>(name); }
        /* Adds a new optional argument to the parser (an argument with a label and a value) of DerivedType T. A reference to the new parameter is returned to change its properties. */
        template <typename T, typename = std::enable_if_t<std::is_base_of<BaseType, T>::value> >
        inline Option& add_option(char shortlabel, const std::string& name) { return args.add_option<T>(shortlabel, name); }
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

        /* Validates if all Positionals have been correctly defined. Specifically, checks if there aren't any mandatory Positionals following Optional ones and that only the last Positional can be variadic. */
        void validate_positionals() const {
            // First, check if the optional positionals are all in the back and, if there's a Positional with any_number, it's the only one and at the back.
            bool optional = false;
            bool seen_last = false;
            for (Positional* pos : this->args.deepsearch<Positional>()) {
                // Check if we have seen a variadic argument
                if (seen_last) {
                    // It's illegal for any Positionals to follow!
                    throw VariadicPositionalException(pos->get_name(), pos->get_index());
                }

                // Check if the optionality is still correct
                if (pos->is_optional()) {
                    optional = true;
                } else if (optional) {
                    // That's illegal!
                    throw OptionalPositionalException(pos->get_name(), pos->get_index());
                }

                // If it's variadic, make sure that it's the last one
                if (pos->is_variadic()) {
                    // Seen the last one
                    seen_last = true;
                }
            }
        }
        /* Parses the command line. Note that it does not parse any values to other types - that is left for the Arguments class. */
        Arguments parse(size_t argc, const char** argv) const {
            const std::string context = "ArgumentParser::parse()";

            /* STEP 1: Check the Positional's validity. */
            this->validate_positionals();

            /* STEP 2: Prepare parsing by creating the Tokenizer and extracting the executable. */
            Tokenizer input(argc, argv);
            Token executable;
            input >> executable;
            if (executable.type == TokenType::empty) {
                // Got nothing!
                throw NoInputException();
            }

            /* STEP 3: Loop through the AtomicArguments and find if they want to parse this. */
            Arguments args;
            size_t positional_index = 0;
            while (!input.eof()) {
                for (AtomicArgument* aarg : this->args.deepsearch<AtomicArgument>()) {
                    std::any result = aarg->parse(positional_index, input);
                    if (!result.has_value()) {
                        // Parsed as such! Register the argument with this value and then start again
                        bool is_repeatable = (dynamic_cast<Positional*>(aarg) && ((Positional*) aarg)->is_variadic()) ||
                                             (dynamic_cast<Option*>(aarg) && (((Option*) aarg)->is_variadic() || ((Option*) aarg)->is_repeatable()));
                        args.add_arg(Arguments::ParsedArgument(
                            aarg->get_name(),
                            aarg->get_shortlabel(),
                            aarg->get_type(),
                            is_repeatable,
                            result,
                            true
                        ));
                        break;
                    }
                }
            }

            /* STEP 4: Check if all MultiArguments were given correctly. */
            for (MultiArgument* marg : this->args.deepsearch<MultiArgument*>()) {
                // Just call validate. It will throw appropriate exceptions if needed
                marg->validate(args);
            }

            /* STEP 5: Check if all mandatory objects were present and give default values otherwise. */
            for (AtomicArgument* aarg : this->args.deepsearch<AtomicArgument>()) {
                if (!args.contains(aarg)) {
                    if (!aarg->is_optional()) { throw MissingMandatoryException(aarg->get_name()); }
                    else if (aarg->has_default()) {
                        // Add it as a default value
                        bool is_repeatable = (dynamic_cast<Positional*>(aarg) && ((Positional*) aarg)->is_variadic()) ||
                                             (dynamic_cast<Option*>(aarg) && (((Option*) aarg)->is_variadic() || ((Option*) aarg)->is_repeatable()));
                        args.add_arg(Arguments::ParsedArgument(
                            aarg->get_name(),
                            aarg->get_shortlabel(),
                            aarg->get_type(),
                            is_repeatable,
                            aarg->get_default(),
                            false
                        ));
                    }
                }
            }

            /* STEP 6: Done! Return the arguments dict. */
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
