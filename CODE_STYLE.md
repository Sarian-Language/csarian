# Sarian Code Style Guidelines

This document defines the preferred coding style for the **Sarian programming language**. All contributors are expected to follow these conventions.

---

## 1. Indentation

* Use **tabs** for indentation.
* Avoid mixed indentation.
* Do not insert space characters for indentation purposes except for alignment within a single line if necessary.

---

## 2. Braces Placement

All opening and closing braces `{}` must be placed on **their own lines** as follows:

```c
if (condition)
{
  	// code
}
else
{
	// code
}

void function_name()
{
	// code
}
```

---

## 3. Naming Conventions

### 3.1 Variables

* Must be **fully descriptive**. Abbreviations are not allowed.

  * ❌ `int TK;`
  * ✅ `int TOKEN;`
  * ❌ `char *strfn;`
  * ✅ `char *string_function;`

* Use **snake_case** for variable names:

  * ✅ `word_other`
  * ✅ `current_token_value`

### 3.2 Functions

* Function names must also use **snake_case**.

  * ✅ `parse_next_token()`
  * ✅ `initialize_parser()`

### 3.3 Structs and Enums

* Use **PascalCase** (UpperCamelCase) for struct and enum names:

  * ✅ `TokenType`
  * ✅ `AstNode`

* Enums should also follow PascalCase:

  * ✅ `TokenType`
  * ✅ `NodeValues`

---

## 4. Comments

### 4.1 Comment Style

* Use **`//` single-line comments** unless the comment is very long (5+ lines) or necessary for documentation/licensing.

  * Multi-line comments using `/* ... */` are allowed **only** for licenses, file headers, or very long explanations.

### 4.2 Comment Placement

* **Inline comments** (brief, on the same line) should be used sparingly to explain a non-obvious expression.

  * ✅ `value = default_value; // ensure correct baseline`

* **Above-line comments** should be used to describe the purpose of a block or function when necessary.

  * ✅

    ```c
    // Computes the next token from the input source.
    // This function advances position and handles escape sequences.
    void get_next_token()
    {
    	...
    }
    ```

### 4.3 Grammar

* **No periods at the end** for very short comments or labels describing code sections.

  * ✅ `// Parser`
  * ❌ `// Parser.`
* For full sentences or explanations, use proper **SPaG**: correct spelling, punctuation, and grammar.

  * ✅
    
    ```c
    // This is a very, very, very, very, very, very, very
    // long comment, you can still see this comment
    // in small screens
    ```
    
  * ❌

    ```c
    // This is a very very very very very very very long comment, this comment is so long that in small screens you can't see past right
    ```
    
* If the comment is long, split it into multiple `//` lines rather than a single long line.

### 4.4 Best Practices

* Avoid commenting obvious code; focus on **clarifying complex or non-intuitive logic**.
* Write comments in **clear, concise English**.
* Keep inline comments short.

---

## 5. Variable Declarations

* Each variable must be declared on its own line (no comma-separated declarations).
* Example:

  ```c
  int current_index;
  int token_count;
  ```

---

## 6. Magic Values

* Avoid magic numbers or strings directly in code. Use constants or macros instead.

  * ❌ `if (value == 3.14)`
  * ✅

    ```c
    #define PI 3.14
    if (value == PI)
    ```

---

## 7. Line Length

* Aim to keep lines under **90-100 characters** when possible for readability.
* Break long expressions across multiple lines.

---

## 8. Operators and Spacing

* Use a single space around most binary operators:

  ```c
  result = value + offset;
  ```
* Do not place spaces inside parentheses:
  ❌ `func( value, x )`
  ✅ `func(value, x)`

## 9. Includes Order

* Standard library headers first, then project headers.
* Group logically with blank lines for separation:

  ```c
  #include <stdio.h>
  #include <stdlib.h>

  #include "parser.h"
  #include "lexer.h"
  ```

---

## 10. Keeping It Simple (KISS Principle)

* Do not overcomplicate expressions or logic.
* Strive for the simplest possible implementation that meets the requirements.

---

If you have questions or need clarification on any rule, you can ask in the Discord server.
