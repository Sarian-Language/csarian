---
title: Definitions
description: definitions.h
---

This section describes the role and structure of `definitions.h`.

This file defines the core data structures and abstractions used across the interpreter.

---

## 1. Variable System

### Variable Types

The interpreter uses an enum (`VariableType`) to represent data types.

---

### Global Variables

Global variables are represented using a struct with three fields:

* A key (identifier)
* A type (`VariableType`)
* A generic pointer to the value (`void *`)

---

## 2. Token System

### Token Types

Tokens are represented using an enum (`TokenType`).

This enum defines **categories of syntax elements**, such as:

* Literals (numbers, strings, etc.)
* Identifiers
* Operators
* Keywords
* Structural symbols (parentheses, brackets, etc.)

---

### Token Structure

Each token is represented by a struct containing:

* Its type (`TokenType`)
* Its textual value
* Its precedence (if applicable)

---

### Precedence System
<div style="background-color: #d1ffda; border-left: 4px solid #28a745; padding: 10px; border-radius: 5px; color: black;">
Operator precedence defines the order in which operators are evaluated within an expression. Each operator is assigned a numeric priority, where higher values indicate higher precedence. During parsing or evaluation, operators with greater precedence are applied before those with lower precedence, ensuring expressions are interpreted correctly without requiring explicit parentheses.
</div>

Operator precedence is defined using an enum (```TokenPrecedence```).

---

## 3. Function Representation

Functions are stored as lightweight metadata:

* Identifier (name)
* Start position in the token stream (of the function block).
* End position in the token stream

---

## 4. Macros and Utilities

The file includes macros for:

### Token Classification

- IS_BINARY_OPERATOR(TokenType)
- IS_VALID_BINARY_OPERAND(TokenType)
- IS_COMPARISON_TOKEN(TokenType)

---

### Token Navigation

Access tokens relative to a position (current, previous, next).