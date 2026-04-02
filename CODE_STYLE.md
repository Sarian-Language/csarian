# Sarian Code Style Guidelines

This document defines the preferred coding style for the **Sarian programming language**. All contributors are expected to follow these conventions.

---

## 1. Automatic Formatting

* After finishing your code, always run:

  ```bash
  make format
  ```

  This automatically formats your code, including **indentation, bracket placement, and other formatting rules.**

* Anything not covered by the automatic formatter is explained in the following sections.

* Make sure **`clang-format-17`** is installed on your system.

---

## 2. Naming Conventions

### 2.1 Variables

* Must be **fully descriptive**. Abbreviations are not allowed (unless it's very obvious what it means).

  * ❌ `int TK;`
  * ✅ `int TOKEN;`
  * ❌ `char *strfn;`
  * ✅ `char *string_function;`

* Use **snake_case** for variable names:

  * ✅ `word_other`
  * ✅ `current_token_value`

### 2.2 Functions

* Use **PascalCase**.

  * ✅ `ParseNextToken()`
  * ✅ `InitializeParser()`

### 2.3 Structs and Enums

* Use **PascalCase** (UpperCamelCase) for struct and enum names as well:

  * ✅ `TokenType`
  * ✅ `AstNode`

---

## 3. Comments

### 3.1 Comment Style

* Use **`//` single-line comments** unless the comment is very long (4-5+ lines) or necessary for documentation/licensing.

  * Multi-line comments using `/* ... */` are allowed **only** for licenses, file headers, or very long explanations.

### 3.2 Comment Placement

* **Inline comments** (brief, on the same line) should be used sparingly to explain a non-obvious expression.

  * ✅ `value = default_value; // Ensure correct value.`

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

### 3.3 Best Practices

* Avoid commenting obvious code; focus on **clarifying complex or non-intuitive logic**.
* Write comments in **clear, concise English**.
* Keep inline comments short.

---

## 4. Grammar

* **No periods at the end** for labels describing code sections.

  * ✅ `// Parser`
  * ❌ `// Parser.`
* For full sentences or explanations, use proper **SPaG**: correct spelling, punctuation, and grammar.

  * ✅
    
    ```c
    // This is a comment with proper spelling and grammar (SPaG).
    // The formatter will automatically split it if it's too long, 
    // so you don't need to worry about that.
    ```

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

## 7. Keeping It Simple (KISS Principle)

* Do not overcomplicate expressions or logic.
* Strive for the simplest possible implementation that meets the requirements.

---

If you have questions or need clarification on any rule, you can ask in the Discord server or by email.
