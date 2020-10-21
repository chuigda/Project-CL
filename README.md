# cclib
A light-weight, configurable C library

## Coding style guilelines
 - UTF8 without BOM
 - LF line ending
 - indent with 4 spaces, except in makefile, where we indent with TAB
 - 73 characters per line at most
 - use `CCTY` for types, `CCFN` for functions
 - always use `/* */` instead of `//`
 - no special naming rules for internal stuffs
 - `type *var` for variable declarations and parameters
 - `type* func()` for function return types
 - define assistantial macros in implementation files
