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

Finally, the identifier used for types (```TYPE```) is defined as follows. It is wrapped in greater-than and less-than signs, and the identifier itself can exist out of at least one alphanumerical, underscore or dash character. In regex:
```
TYPE = '<[A-Za-z0-9_-]+>'
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

Finally, floating-point numbers (```DECIMAL```) are virtually identical to integers, except that they must a dot somewhere not at the start. If a dot is given at the end, the interpret will interpret that as '.0'. In regex:
```
DECIMAL = '(---)?[0-9]+\.[0-9]*'
```

### Special tokens
Aside from identifiers and values, ADL also features a couple of special tokens that are used to derive structure from the text. There are the following:
- left square bracket (```LSQUARE = '\['```)
- right square bracket (```RSQUARE = '\]'```)
- left curly bracket (```LCURLY = '{'```)
- right curly bracket (```RCURLY = '}'```)
- semicolon (```SEMICOLON = ';'```)

Aside from those structural characters, there is also the triple-dot token (```TDOT = '\.\.\.'```), which is used to indicate that a variable is variadic (i.e., its value can be repeated any number of times).

### Compiler directives
Aside from the stuff that ADL is actually used for, it also supports the ability to pass meta-commands to the parser. Specifically, such a compiler directive starts with a ```DIRECTIVE```-token, followed by any number of other tokens as specified by the parser itself. The special ```DIRECTIVE```-token is any token that starts with a dot, and is followed by at least one of alphanumerical characters, including underscores and hyphens. In regex:
```
DIRECTIVE = '\.[A-Za-z0-9_-]+'
```
Note that the value of the directive excludes the preceding dot.

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
file = file positional
     = file option
     = file typedef
     = file directive
     = positional
     = option
     = typedef
     = directive
```

### Common grammar rules
Some of the grammar rules are used throughout the ADL, and so we first define them here.

The first common rule is the ```config```-rule, which is basically the body of an argument or a type. Specifically, it should accept any number of config parameters:
```
config = config ID values SEMICOLON
       = ID values SEMICOLON
```
The ```config```-rule makes use of the ```values```-rule, which is simply a list of value types (string, integers or floats):
```
values = values STRING
       = values REGEX
       = values NUM
       = values DECIMAL
       = STRING
       = REGEX
       = NUM
       = DECIMAL
```

### Argument definition
The biggest feature of ADL is, of course, how to define new arguments. Of these, there are two types: Positionals and Options. Both of these will be defined below.

The first type of argument, the Positionals, is given by the following grammar rule:
```
positional = ID types LCURLY config RCURLY
           = ID types TDOT LCURLY config RCURLY
           = LSQUARE ID RSQUARE types LCURLY config RCURLY
           = LSQUARE ID RSQUARE types TDOT LCURLY config RCURLY
```
Here, the rule with square brackets indicates that the positional is optional, and the triple dots indicate that it is variadic - i.e., the positional can have any arbitrary number of values.

The ```types```-rule used here is simpy a list of ```TYPE```-tokens:
```
types = types TYPE
      = TYPE
```

Similarly, the second type of arguments can be given with the next grammar rule:
```
option = option_id types LCURLY config RCURLY
       = option_id types TDOT LCURLY config RCURLY
       = LSQUARE option_id RSQUARE types LCURLY config RCURLY
       = LSQUARE option_id RSQUARE types TDOT LCURLY config RCURLY
```
Again, here the rule with square brackets indicates that the positional is optional, and the triple dots indicate that it is variadic. Additionally, the types rule is the same as defined above.

What is different is the ```option_id```-rule, which is defined as follows:
```
option_id = SLABEL
          = LLABEL
          = SLABEL LLABEL
          = LLABEL SLABEL
```

### Type definition
To support defining arguments, the ADL also allows users to define their own types. Grammar-wise, the syntax for defining a type is very similar to that of defining an argument, and is defined as follows:
```
typedef = TYPE LCURLY config RCURLY
```

### Compiler directives
As a final top-level construct, ADL defines compiler directives that do not directly influence the result, but instead influence how the file is parsed. Which directives are implemented are entirely compiler-specific, but they all follow the same grammar rule, defined as follows:
```
directive = directive values
          = DIRECTIVE
```
where values is the same values rule as discussed under the Common Grammar Rules section.

## 4. Closing thoughts
This file specifies the Argument Definition Language so that parses can parse the file. For more information on using the ADL as a user in the context of the ArgumentParser, please refer to the online [wiki](https://github.com/Lut99/ArgumentParser/wiki) of the ArgumentParser.
