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

# Install Yuji
make install

# Uninstall Yuji
make uninstall
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
- Array (`array`): Ordered collection of elements, e.g., `[1, 2, 3]`.
```

### Variables

Declare variables with `let`:

```yuji
let x = 5
let name = "Yuji"
let pi = 3.14
```

Reassign existing variables with `=`:

```yuji
x = 1
```

Variables are scoped to blocks {}.

### Operators

- Arithmetic: `+`, `-`, `*`, `/`, `%` (modulo), `+=`, `-=`, `*=`, `/=`, `%=`.
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

### Anonymous Functions

> [!NOTE]
> added in v0.2.0

Anonymous functions (also called lambdas) can be created without a name and assigned to variables:

```yuji
let add = fn(a, b) { a + b }
let result = add(3, 4)  // 7
```

They can be passed as arguments to other functions:

```yuji
use "std/io"

fn do_twice(f, x) {
    f(f(x))
}

let double = fn(x) { x * 2 }
println(do_twice(double, 2))  // 8
```

### Modules

Import modules with use:

```yuji
use "std/io"
use "std/core"

use "@/some_file.yuji" // import user-defined module
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

### std/os

> [!NOTE]
> added in v0.2.0

- `system(command)`: Executes a command in the shell.
- `setenv(key, value)`: Sets an environment variable.
- `getenv(key)`: Gets an environment variable.

### std/time

> [!NOTE]
> added in v0.2.0

- `time()`: Returns the current Unix timestamp in seconds.
- `sleep(seconds)`: Pauses execution for the specified number of seconds.
- `sleepms(milliseconds)`: Pauses execution for the specified number of milliseconds.

### std/math

> [!NOTE]
> added in v0.2.0

- `sin(x)`: Returns the sine of x (in radians).
- `cos(x)`: Returns the cosine of x (in radians).
- `tan(x)`: Returns the tangent of x (in radians).
- `pow(base, exponent)`: Returns base raised to the power of exponent.
- `sqrt(x)`: Returns the square root of x.
- `abs(x)`: Returns the absolute value of x.
- `floor(x)`: Returns the largest integer less than or equal to x.
- `ceil(x)`: Returns the smallest integer greater than or equal to x.
- `round(x)`: Returns the nearest integer to x.
- `random(min, max)`: Returns a random integer between min and max (inclusive).

### std/array

> [!NOTE]
> added in v0.2.0

- `push(array, value)`: Adds a value to the end of an array.
- `pop(array)`: Removes and returns the last element of an array.
- `len(array)`: Returns the length of an array.
