---
title: Language's Pipeline
description: Workflow of the code.
---

In this section, we will cover the main pipeline of the language's source code.

<div style="background-color: #d1ffda; border-left: 4px solid #28a745; padding: 10px; border-radius: 5px; color: black;">
The pipeline refers to the sequence of stages that source code passes through from the language’s code to its execution.
</div>

---

## 1. File Reading

`main.c` is the entry point of the language, containing the `main()` function.
### **Functionality** ###

* Accepts a single argument when running the interpreter, which is the path to the Sarian code to be interpreted.
* Determines the file size and allocates memory (`malloc()`) accordingly for the string variable `code`.
* Finally, passes the loaded code to ```lexer.c``` (explained in the next step).

---

## 2. Lexer

### Functionality

#### Core Functions

* **InitTokens()** – Initializes the `tokens` array (type `Token` defined in `definitions.h`) and allocates an initial capacity of 32.
* **AddToken(TokenType type, char value, TokenPrecedence precedence)** – Adds a token to the array. If the array is full, it doubles its size using `realloc`.

### Lexer Process

The lexer scans the source code character by character using a series of conditional checks (`if-else if-else`) to classify input into tokens:

* **End of File** – When the end of the code is reached, the lexer adds a `TOKEN_EOF` token and exits the loop.
* **Newlines** – Each newline produces a `TOKEN_EOL`.
* **Comments** – Lines starting with `//` are skipped until the end of the line.
* **Strings** – Text enclosed in `" "` or `' '` is captured as `TOKEN_STRING`.
* **Identifiers & Keywords** – Alphanumeric sequences and `_` are captured as identifiers. Recognized keywords (`if`, `else`, `while`, `for`, `fn`, etc.) are converted into their respective tokens instead of an identifier token.
* **Numbers** – Integer and floating-point literals are recognized. Multiple decimal points trigger a syntax error.
* **Operators & Symbols** – Handles `+`, `-`, `*`, `%`, `/`, `=`, `==`, `!=`, `<`, `<=`, `>`, `>=`, `!`, colons, parentheses, brackets, etc. with proper precedence where applicable.
* **Error Handling** – Any unrecognized character triggers a `SYNTAX_INVALID` error.

### Finalization

* Passes the token array and the token count to the Interpreter, explained in the next step.

---

## 3. Interpreter

### Functionality

#### Initialization

* **InitGlobalVariables()** – Initializes storage for global variables.
* **InitFunctions()** – Initializes functions.

---

### Execution Model

The interpreter iterates over the token array using a loop, processing each token based on its type using conditionals.

Key internal state:

* `line_num` – Tracks the current line for error reporting.
* `in_block` – Indicates whether the current execution is inside a block (e.g., function).
* `block_end` – Marks the end of the current block if there's one.
* `original_pos` – Stores where execution should return after finishing a block.

---

### Finalization

* **TerminateGlobalVariables()** – Frees variable storage.
* **TerminateFunctions()** – Frees function storage.