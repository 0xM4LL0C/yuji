# Yuji Programming Language

Yuji is a lightweight, embeddable scripting language designed for simplicity and
ease of use. It features a clean syntax inspired by modern languages, dynamic
typing, and a modular standard library. Yuji is implemented in C and can be used
as a standalone interpreter or embedded in other applications.
This documentation covers the essentials for users: installation, running
scripts, language basics, and the standard library. For developers interested in
extending Yuji, refer to the source code.

## Quick Start

### Building Yuji

Yuji uses a simple Makefile for building. Ensure you have GCC (GCC is used as
default compiler, you can use other compilers that support GNU99) and Make installed.

```bash
# Debug build (recommended for development)
make debug

# Release build (optimized)
make release

# Clean build artifacts
make clean

# Run test file (test.yuji)
make test
```

The binary yuji will be built in the .build/ directory.

### Running Yuji

- Interactive REPL: Launch the interpreter for live coding.

```
.build/yuji
```

Output:

```
Welcome to Yuji REPL! (version 0.1.0)
ctrl+D to exit
>
```

- Execute a Script File: Run a .yuji file.

```bash
.build/yuji main.yuji
```

- Usage:

```
Usage: yuji [filename]
```

## Language Basics

Yuji has a concise, indentation-insensitive syntax (uses braces for blocks).
It supports basic arithmetic, control flow, functions, and modules.

### Types

Yuji is dynamically typed. Built-in types include:

- Integer (`int`): Whole numbers, e.g., `42` or `-10`.
- Float (`float`): Decimal numbers, e.g., `3.14`.
- String (`string`): Text, e.g., `"hello"`. Supports escapes like `\n` and `\"`.
- Boolean (`bool`): `true` or `false`.
- Null (`null`): Represents absence of value.

### Variables

Declare variables with `let`:

```yuji
let x = 5
let name = "Yuji"
let pi = 3.14
```

Reassign existing variables with `=`:

```yuji
x = x + 1
```

Variables are scoped to blocks {}.

### Operators

- Arithmetic: `+`, `-`, `*`, `/`, `%` (modulo).
- Comparison: `<`, `>`, `<=`, `>=`, `==`, `!=`.
- Logical: `&&` (and), `||` (or)

```yuji
let sum = 2 + 3 * 4  // 14 (operator precedence: * before +)
let is_greater = 10 > 5  // true
```

### Control Flow

#### If/Elif/Else

```yuji
if condition {
  // body
} elif other_condition {
  // body
} else {
  // body
}
```

Example:

```yuji
use "std/io"

let age = 20

if age >= 18 {
  println("Adult")
} else {
  println("Minor")
}
```

#### While Loops

```yuji
while condition {
  // body
}
```

Example:

```yuji
use "std/io"

let i = 0

while i < 5 {
  println(i)
  i = i + 1
}
```

### Functions

Define named functions:

```yuji
fn add(a, b) {
  a + b  // Return the sum of a and b
}
```

Call functions:

```yuji
let result = add(3, 4)
```

Functions support multiple parameters and return the last expression's value (implicit return).

### Modules

Import modules with use:

```yuji
use "std/io"
use "std/core"
```

## Standard Library

### std/core

- `not(value)`: Logical negation (returns bool).
- `typeof(value)`: Returns the type as a string (e.g., "int").
- `assert(condition, [message])`: Throws panic if condition is false.
- `panic(message)`: Throws a runtime error.
- `exit(code)`: Exits the program with integer code.
- `to_number(string)`: Parses a string to int or float.

```yuji
use "std/io"
use "std/core"

let num = to_number("0xA")

println(num)  // 10
```

### std/io

- `print(...args)`: Prints arguments without newline.
- `println(...args)`: Prints arguments with newline.
- `input([prompt])`: Reads a line from stdin (prints prompt if provided).
- `format(template, ...args)`: String interpolation with {} placeholders

```yuji
use "std/io"

let name = "World"
let msg = format("Hello, {}!", name)

println(msg) // "Hello, World!"
```
