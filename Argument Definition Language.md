# Argument Definition Language - Specification

## 1. Introduction
In this file, we will list the specification for the Argument Definition Language (ADL), which can be used to define the rules for parsing arguments in a high-level and (hopefully) intuitive way.

NOTE: If you are not looking to implement a parser for the ADL yourself but instead want to use it in the context of the ArgumentParser, please refer to the online [wiki](https://github.com/Lut99/ArgumentParser/wiki) of the ArgumentParser itself.

In this file, we first list the type of tokens that can be written in an ADL- file, after which we define the grammar rules that are used to parse it.

### Conventions
In this file, we will regularly list regex-expressions that recognize tokens. These expressions are wrapped in quotes (''). These can be assigned to certain identifiers, which are the names of the tokens they parse (if applicable).

## 2. Tokens
In ADL, we define a couple of tokens that are both used to parse the file and that are used to make up the grammar rules. From a usage perspective, the tokens are interesting to know as they define the basic syntax blocks that make up the language itself.

### Identifiers
Arguably one of the most important tokens are the identifiers. They are used to write unique names, such as the name of certain objects. Specifically, identifiers are used to define argument names (Positional and Options), define or reference type names and define or reference configuration settings for an agument.

For Positionals and configuration keywords, the identifier (referred to as ```ID```) must start with any alphabetical character (a-z & A-Z), and can then be a sequence of arbitrary length of any alphanumerical character, an underscore or a dash. Specifically, the regex expression for this type of identifiers would be:
```
ID = '[A-Za-z][A-Za-z0-9_-]*'
```
Note that any non-recognised configuration keywords may be expelled by the parser. For a list of acceptable keywords in the default parser, check \<TBD\>.

For Options, there are two different identifiers that are used. The first is the so-called longlabel identifier (```LLABEL```). This is an identifier of arbitrary length, which must always start with a double dash (--). Then, it can be followed by any sequence of alphanumerical characters, an underscore or more dashes, as long as it's at least one of those. Note, however, that to disambiguate from numbers, the first character cannot be another hyphen. The regex experession for this identifier would be:
```
LLABEL = '--[A-Za-z0-9_][A-Za-z0-9_-]'*
```

The other identifier used for Options, the shortlabel (```SLABEL```), exists only of a dash and any alphanumerical characters, plus a question mark. The regex-expression would be:
```
SLABEL = '-[A-Za-z0-9?]'
```

Types also have their own identifiers (```TYPE```), which are defined as follows. The name is wrapped in greater-than and less-than signs, and the identifier itself can exist out of at least one alphanumerical, underscore or dash character. In regex:
```
TYPE = '<[A-Za-z0-9_-]+>'
```

The final identifier occurring in the ADL are those for used to identify and set additional properties of arguments and types: the configuration identifiers (```CONFIG```). These identifiers are any alphanumerical character, underscore or dash, as long as its proceeded by a dot. In regex:
```
CONFIG = `.[A-Za-z0-9_-]+`
```

### Values
The ADL features four different values: plain strings, regex-expressions, integers and float-points. Unlike identifiers can values be repeated.

The first value type, strings (```STR```), are any sequence of characters wrapped in strings - including whitespaces (except for newlines). The regex-expression describing this would look like:
```
STR = '"([^"\\\n]|\\.)*"'
```
Note that we did some extra fuzziness to make sure to escape any character preceded by a backslash, including quotes.

A more special type of strings are regex-expressions (```REGEX```), which can be used when defining types so that the compiler knows what kind of input your parse function expects. These expressions start with an r and then the value, wrapped in quotes (e.g., 'r"hello"'). For the rest, the value is parsed exactly as a normal string, so that it can later be parsed as a real regex-expression.
The regex-expessions to match regex-experessions is:
```
REGEX = 'r"([^"\\\n]|\\.)*"'
```

The third value type, integers (```NUM```), are simply numbers; this means that they are identified by any number of numerical characters. Note that the parser does not support double negation, and that negative numbers have to begin with three dashes to distinguish them from Option long- or shortlabels. In regex:
```
NUM = '(---)?[0-9]+'
```

Similar to integers are floating-point numbers (```DECIMAL```), with their only difference being that they must have a dot somewhere but not at the start. If a dot is given at the end, the parser will interpret that as '.0'. In regex:
```
DECIMAL = '(---)?[0-9]+\.[0-9]*'
```

As a simple, 1-or-0 value, the ADL also includes booleans. A boolean is simply either 'true' or 'false', lowercase, but wrapped in brackets s.t. the parser can easily disambiguate it from identifiers. In regex:
```
BOOL = '(\(true\))|(\(false\))'
```

Aside from just values, users can also specify inline snippets of C++-code, for easy implementation of type parsers. Each such block is wrapped in matching curly brackets '++{' and '}++'. Then, in the snippet, one can use '++\<X\>++' to reference the X'th value matched by the type's given pattern. Note that the parser respects C++ comments, meaning that both the special brackets and references can be used in comments without messing up parsing.  
In regex, the token can be described as:
```
SNIPPET = '\+\+{([^/]|(\/\*([^*]|\*+[^/])*\*+\/)|(\/\/.*\n)|\/)*?}\+\+'
```
The expression may look hidious (and it is), but a careful observer should see that it is made up of five parts:
- The outer '++{' and '}++'
- A first rule that matches anything but forward slashes
- A second rule that matches multi-line comments (see below)
- A third rule that matches single-line comments (see below)
- A fourth rule matching only forward slashes that aren't part of commas.

In this way, the expression respects C++-style comments.

The final type of value that the ADL supports is not strictly a value, but rather a shotcut that is added for organisational purposes: a so-called reference (```REFERENCE```). These references are any (existing) Positional (using its identifier), Option (using its shortlabel or longlabel) or type definition (using its type identifier), followed by a dot and then the identifier part of a configuration parameter. In regex:
```
REFERENCE = '((ID)|(SLABEL)|(LABEL)|(TYPE))\.[A-Za-z0-9_-]+'
```
where ```ID```, ```SLABEL```, ```LLABEL``` and ```TYPE``` can be replaced with their respective regex-expressions.  
Note that the special toplevel META-namespace can also be used to reference constant values from.

### Special tokens
Aside from identifiers and values, ADL also features a couple of special tokens that are used to derive structure from the text. There are the following:
- left square bracket (```LSQUARE = '\['```)
- right square bracket (```RSQUARE = '\]'```)
- left curly bracket (```LCURLY = '{'```)
- right curly bracket (```RCURLY = '}'```)
- semicolon (```SEMICOLON = ';'```)
- the triple dot (```TDOT = '\.\.\.'```)

Additionally, the Tokenizer is also used to identify preprocessor macros (see section 4):
```
MACRO = '#[A-Za-z0-9-_]'
```

Finally, users can also control when and how the parser / compiler will treat warnings and errors. In particular, three tokens are defined to control that: a token that suppresses warnings (```SUPPRESS```), a token that raises them (```WARNING```) and a token that will raise a particular error (```ERROR```). In regex, these expressions look like this:
```
SUPPRESS = '@suppress'
WARNING  = '@warning'
ERROR  = '@error'
```
Following the tokens may either be an identifier, which is used to recognize a particular warning type (for ```SUPPRESS``` and ```WARNING```) or a direct string for custom warnings or errors (for ```WARNING``` and ```ERROR```). See section 3 for more information on how the tokens are parsed.

### Comments
Finally, the ADL also supports the use of comments. Although these aren't passed to the parser, the comments are matched by the Tokenizer and are therefore mentioned here.

The comments are C-style, both single-line and multi-line comments. The first type, single-line, starts at a double '//' and continues until the end of the line. In regex:
```
'\/\/.*\n'
```

Multi-line comments are slightly more complicated. They start at the occurence of a '/\*', and continue until a '\*/' has been reached. Moreover, they are supposed to support nested multi-line comments, and so the regex becomes:
```
'\/\*([^*]|\*+[^/])*\*+\/'
```
Credits to [this blog](http://blogs.perl.org/users/ben_bullock/2017/08/c-comments-and-regular-expressions.html) for the regex expression for multi-line comments.

## 3. Grammar
The grammar in the ADL defines how the tokens can be used, and what semantic meaning that particular sentence should have. In this section, we will describe the grammar of the ADL in terms of the Token described in section 2.

The top-level grammar rule is defined as follows:
```
file = file meta
     = file positional
     = file option
     = file typedef
     = meta
     = positional
     = option
     = typedef
```

Warnings may also occur in the toplevel of the file, which have their own rules to be parsed. For ```@suppress```-modifiers, the rule is:
```
suppress = SUPPRESS ID
```
where the ID refers to a particular build-in warning. Semantically, the ```@suppress``` token will suppress the all occurences of the given warning in either a definition (if it's defined directly before it) or a property if it's defined before that. Note that it cannot suppress custom warnings (defined with a string) or errors.  
Similarly, for the ```@warning```-modifier, the rule is:
```
warning = WARNING STRING
```
where the string can be used to throw a warning with a custom message. When the parser encounters this modifier, the specified warning is immediately thrown.  
Finally, the ```@error```-modifier is used to throw custom errors:
```
error = ERROR STRING
```
where the string part creates the message of the thrown error. Again, the parser throws the error immediately as soon as it encounters it.  
Note that macros (see section 4) and warning tokens may occur anywhere in the file, as long as using them there makes sense.

### Common grammar rules
Some of the grammar rules are used throughout the ADL, and so we first define them here.

The first common rule is the ```config```-rule, which is basically the body of an argument or a type. Specifically, it should accept any number of config parameters:
```
config = config CONFIG values SEMICOLON
       = CONFIG values SEMICOLON
```
The ```config```-rule makes use of the ```values```-rule, which is simply a list of value types (string, regex-expressions, integers or floats):
```
values = values STRING
       = values REGEX
       = values NUM
       = values DECIMAL
       = values BOOL
       = values REFERENCE
       = values SNIPPET
       = STRING
       = REGEX
       = NUM
       = DECIMAL
       = BOOL
       = REFERENCE
       = SNIPPET
```

The other common rule is the ```def_body``` rule, which is used to define the body of each of the definitions in the ADL:
```
def_body = LCURLY config RCURLY
         = LCURLY RCURLY
```
where the ```config```-rule is the one defined above.

### Meta definition
As a special type of toplevel construct, the ADL has space for the meta-definition. In this toplevel, some special values are allowed that influence some meta-strategies of the to-be-created parser; additionally, any property may be given here s.t. it may be referenced later. Its syntax is simply an identifier 'meta', followed by a normal bracket-config construct (see the arg_body grammar rule):
```
meta = ID def_body
```
Note that it's distinguishable from a normal Positional definition by its lack of types, which are mandatory for a Positional.  
Unlike other definitions, is the meta to be specified multiple times - this way, each file may implement its own, global parameters.

### Argument definition
The biggest feature of ADL is, of course, how to define new arguments. Of these, there are two types: Positionals and Options. Both of these will be defined below.

The first type of argument, the Positionals, is given by the following grammar rule:
```
positional = ID types def_body
           = ID types TDOT def_body
           = LSQUARE ID RSQUARE types def_body
           = LSQUARE ID RSQUARE types TDOT def_body
```
Here, the rule with square brackets indicates that the positional is optional, and the triple dots indicate that it is variadic - i.e., the positional can have any arbitrary number of values.

The ```types```-rule used here is simpy a list of ```TYPE```-tokens:
```
types = types TYPE
      = TYPE
```

Similarly, the second type of arguments can be given with the next grammar rule:
```
option = option_id def_body
       = option_id types def_body
       = option_id LSQUARE types RSQUARE def_body
       = option_id types TDOT def_body
       = option_id LSQUARE types TDOT RSQUARE def_body
```
Again, here the rule with square brackets indicates that the positional is optional, and the triple dots indicate that it is variadic. Additionally, the types rule is the same as defined above.

What is different is the ```option_id```-rule, which is defined as follows:
```
option_id = SLABEL
          = LLABEL
          = SLABEL LLABEL
          = LLABEL SLABEL
          = LSQUARE SLABEL RSQUARE
          = LSQUARE LLABEL RSQUARE
          = LSQUARE SLABEL LLABEL RSQUARE
          = LSQUARE LLABEL SLABEL RSQUARE
```
Again, the rules with the square brackets indicate that the option is Optional.

For both arguments type holds that one can specify additional properties besides the one recognised by the parser for reference purposes, just like the META namespace.

### Type definition
To support defining arguments, the ADL also allows users to define their own types. Grammar-wise, the syntax for defining a type is very similar to that of defining an argument, and is defined as follows:
```
typedef = TYPE LCURLY config RCURLY
```
The type definition, too, supports the specification of unrecognised properties for reference purposes.

## 4. Preprocessor Macros
Like C or C++, the ADL has a concept of a preprocessor. In this case, the preprocessor forms an extra layer of abstraction on top of the Tokenizer but below the Parser. This way, the preprocessor needn't implement a new Tokenizer from scratch, but still allows the parser to get only to-be-parsed tokens. Right now, the preprocessor implements including other files and conditional compilation.

### Including
Like in normal programming languages, it may be very useful to re-use earlier created arguments, or to separate them in separate files for overview. To the end, the ```include```-macro or -directive has been added. Using this directive will "place" the file contents of the target file in the file where they are included. In reality, this is not really done by actually copy/pasting them, but the preprocessor simply opens another Tokenizer for the included file and drains that first before the previous Tokenizer is resumed. Since it's all in virtual memory, the Preprocessor also prevents double includes as it knows which already have been opened.

The syntax to include a file is:
```
#include provided_file
```
or
```
#include "path_to_local_file"
```
The first case is used to include files baked in the compiler (see below), which usually just provide standard types for your parser to use. The second case is used to include your own files, and relies on your OS' path resolving to find the correct file.

A list of standard files included in the compiler:
- ```stdtypes```: Contains standard definitions for commonly used types. Includes definitions for signed & unsigned, 8-, 16-, 32- and 64-bit integers; single-precision and double-precision floating points; booleans; strings and single-letter characters.

### Conditional compilation
Sometimes, it's very useful to include an argument, but at other times it's better to exclude it. To this end, the ADL supports conditional compilation in a similar way as C does. One can specify a so-called define, which can then be used to include certain pieces of code in compilation or not. Additionally, ADL compilers should also allow users to give these beforehand as CLI-argument, to make conditional compilation even easier.

Be aware that, unlike in C, the defines cannot be used for constants or any sort of expension; they are purely there to deterimine if a certain piece of code should be compiled or not. This is done because all of the ArgumentParser is done at compile time anyway, and such a system does not have performance bonusses over the referencing system already in present in the ADL.

To define a new define in your file, use:
```
#define NAME
```
or
```
#def NAME
```
where ```NAME``` is a unique identifier for that define, consisting only of alphanumeric characters and underscores.

Similarly, one can also choose to undefine a previously defined define:
```
#undefine NAME
```
or
```
#undef NAME
```

To compile a piece of code only when a certain defined is present, wrap it in the following two macros:
```
#ifdef NAME
...
#endif
```
where ```NAME``` is, again, a unique identifier for a define. One can also use the negation of the above macro to only compile code if a certain define is _not_ defined:
```
#ifndef NAME
...
#endif
```
Note that any of these macros may be nested in the if-macros, where it will be assumed that two outermost if/endif macros are a pair. Additionally, note that these macros only work as separate tokens; they cannot be used in strings or code snippets (this means you can use defines in your C++ code snippets that will be relevant for the C++-preprocessor instead of the ADL).

## 5. Closing thoughts
This file specifies the Argument Definition Language so that parses can parse the file to a semantically correct Abstract Syntax Tree. More information, especially on using the ADL in the context of the ArgumentParser, can be found in the online wiki [wiki](https://github.com/Lut99/ArgumentParser/wiki) of the ArgumentParser.
