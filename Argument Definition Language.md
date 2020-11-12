# Argument Definition Language - Specification

## 1. Introduction
In this file, we will list the specification for the Argument Definition
Language (ADL), which can be used to define the rules for parsing arguments in
a high-level and (hopefully) intuitive way.

In this file, we first list the type of tokens that can be written in an ADL-
file, after which we define the grammar rules that are used to parse it. For an
implementation of such parser, see the ADLTokenizer and the ADLParser classes
in the repository on GitHub: github.com/Lut99/ArgumentParser.

## 2. Quick start


## 3. Tokens
In ADL, we define a couple of tokens that are both used to parse the file and
that are used to make up the grammar rules. From a usage perspective, the
tokens are interesting to know as they define the basic syntax blocks that make
up the language itself.

### Identifiers
Arguably one of the most important tokens are the identifiers. They are used to write unique names, such as the name of certain objects. Specifically, identifiers are used to define argument names (Positional and Options), define or reference type names and define or reference configuration settings for an agument.

For Positionals and configuration keywords, the identifier must start with any alphabetical character (a-z & A-Z), and can then be a sequence of arbitrary length of any alphanumerical character, an underscore or a dash. Specifically, the regex expression for this type of identifiers would be:
```
/[A-Za-z][A-Za-z0-9_-]*/g
```
Note that any non-recognised configuration keywords may be expelled by the parser. For a list of acceptable keywords in the default parser, check \<TBD\>.

For Options, there are two different identifiers that are used. The first is the so-called longlabel identifier. This is an identifier of arbitrary length, which must always start with a double dash (--). Then, it can be followed by any sequence of alphanumerical characters, an underscore or more dashes, as long as it's at least one of those. The regex experession for this identifier would be:
```
/--[A-Za-z0-9_-]+/g
```

The other identifier used for Options, the shortlabel, exists only of a dash and any alphanumerical characters, plus a question mark. The regex-expression would be:
```
/-[A-Za-z0-9?]/g
```

Finally, the identifier used for types is defined as follows. It is wrapped in greater-than and less-than signs, and the identifier itself can exist out of at least one alphanumerical, underscore or dash character. In regex:
```
/<[A-Za-z0-9_-]+>/g
```

### Values
The ADL features three different values: strings, integers and float-points. Unlike identifiers can values be repeated.

The first value type, strings, are any sequence of characters wrapped in strings - including spaces, newlines, etc. The regex-expression describing this would look like:
```
/"([^"\\]|\\.)*"/g
```
Note that we did some extra fuzziness to make sure to escape any character preceded by a backslash, including quotes.

The second value type, integers, are simply numbers; this means that they are identified by any number of numerical characters. Note that the parser does not support double negation, and that negative numbers have to begin with three dashes to distinguish them from Option long- or shortlabels. In regex:
```
/(---)?[0-9]+/g
```

Finally, floating-point numbers are virtually identical to integers, except that they must a dot somewhere not at the start. If a dot is given at the end, the interpret will interpret that as '.0'. In regex:
```
/(---)?[0-9]+\.[0-9]*/g
```

### Special characters
Aside from identifiers and values, ADL also features a couple of special tokens that are used to derive structure from the text. There are the following:
- left square bracket (```/\[/g```)
- right square bracket (```/\]/g```)
- left curly bracket (```/{/g```)
- right curly bracket (```/}/g```)
- equals sign (```/=/g```)
- semicolon (```/;/g```)

Aside from those structural characters, there is also the triple-dot token (```/\.\.\./g```), which is used to indicate that a variable is variadic (i.e., its value can be repeated any number of times).

### Comments
Finally, the ADL also supports the use of comments. Although these aren't passed to the parser, the comments are matched by the Tokenizer and are therefore mentioned here.

The comments are C-style, both single-line and multi-line comments. The first type, single-line, starts at a double '//' and continues until the end of the line. In regex:
```
/\/\/.*\n/g
```

Multi-line comments are slightly more complicated. They start at the occurence of a '/\*', and continue until a '\*/' has been reached. Moreover, they are supposed to support nested multi-line comments, and so the regex becomes:
```
/\/\*([^*]|\*+[^/])*\*+\//g
```
Credits to [this blog](http://blogs.perl.org/users/ben_bullock/2017/08/c-comments-and-regular-expressions.html) for the regex expression.
