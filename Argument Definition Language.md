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
/([A-Za-z])([A-Za-z0-9_-])*/g
```

For Options, there are two different identifiers that are used. The first is the so-called longlabel identifier. This is an identifier of arbitrary length, which must always start with a double dash (--). Then, it can be followed by any sequence of alphanumerical characters, an underscore or more dashes, as long as it's at least one of those. The regex experession for this identifier would be:
```
/--([A-Za-z0-9_-])+/g
```

The other identifier used for Options, the shortlabel, exists only of a dash and any alphanumerical characters, plus a question mark. The regex-expression would be:
```
/-[A-Za-z0-9?]/g
```

Finally, the identifier used for types is defined as follows. \<TBD\>

### Values

### Special characters

### Comments
