# Compiler-Construction

# 1. Design a preprocessor on top of the base compiler infrastructure provided to you.

Preprocessing is a stage in the pipeline before lexing begins. Preprocessors take the source code as input and output code after processing any preprocessor directives inside the code. 
An example of this would be the `#include` preprocessor directive in C code, which the preprocessor replaces with the contents of the file specified as an argument to the directive. Flex is used for this purpose.

The specifications of the required preprocessor are as follows:

  ● The preprocessor performs text substitution.

  ● Directive #def: `#def IDENTIFIER text`. This directive is similar to the `#define` directive in C. After a `#def`   directive, the preprocessor replaces all instances of `IDENTIFIER` in the source code with `text`.
  
Examples regarding all possible variations of this directive are mentioned below.

  ● Directive #undef: `#undef IDENTIFIER`. Undefines the defined IDENTIFIER macro. The macro is not expanded after  this directive. If IDENTIFIER is already undefined before this directive, this directive is ignored.
  
  ● It also matches and removes any comments in the code. There are two types of comments, single line comments (starting with `//`) and multiline comments (enclose within `/*` and `*/`). A multiline comment ends at the first `*/` instance, i.e. nesting multiline comments is not permitted.
  
The #ifdef and #endif macro constructs are also implemented. The #ifdef directive allows for conditional compilation. The preprocessor determines if the provided macro exists before including the subsequent code in the compilation process. #elif and #else cases are also handled.

# 2. Design a parser on top of the baseline compiler infrastructure provided to you.

Adding Variable assignment statement and Ternary operator ( a = <expr> ? <expr> : <expr> )
The appropriate grammar for the parser to take in the generated tokens by the lexer and generate the corresponding Abstract Syntax Tree nodes is also defined.
  
1. Added rules to the parser so that assignment and ternary operatorsare parsed.

Declared new tokens for ? and : and editted src/lexer.lex to add support to these new tokens, and also set the type of the assignment and ternary operator rules.

2. Created new rukes for the appropriate AST nodes. Each new AST node define will inherit the base Node struct, and extends it with extra information according to the type of node. Two abstract methods, to_string and llvm_codegen is alsod implemented.
  
  
This project is about building an end-to-end compiler for the rudimentary language. We are to add named functions (with recursion and composition) and the if-then-else block. Certain architecture-independent optimizations is also added. 

# Types

Added support for types. The compiler works for short (16-bit), int (32-bit), and long (64-bit)datatypes. Smaller types are able to fit into bigger types, and a bigger type should not be able to fit into smaller
types. Arithmetic operations can handle type coercion correctly, result of an arithmetic operation between two different types is always the larger type.
  
# Control Flow
  
Added support for if statements. Note that every if statement has an else statement attached to it. Braces are also mandatory. Scoping issues are handled when implementing the block of the if statement.
  
# Functions
  
Added function support to the language.
  
# Optimizations
