# YUJI

[![build](https://github.com/0xM4LL0C/yuji/actions/workflows/build.yaml/badge.svg)](https://github.com/0xM4LL0C/yuji/actions/workflows/build.yaml)
[![CodeQL](https://github.com/0xM4LL0C/yuji/actions/workflows/github-code-scanning/codeql/badge.svg)](https://github.com/0xM4LL0C/yuji/actions/workflows/github-code-scanning/codeql)

<img src="/assets/yuji-logo.png">

A simple programming language implementation written in C.

## Quick Start

### Build
```bash
make build
```

### Run
```bash
./build/yuji test.yuji
```

## Example

```yuji
use "std/io"
use "std/core"

fn factorial(n) {
    if n <= 1 {
        1
    } else {
        n * factorial(n - 1)
    }
}

let result = factorial(5)
println(format("5! = {}", result))
```

## Status

> [!WARNING]
> Work in progress - not ready for production use

## Documentation

For detailed language documentation, syntax guide, and standard library reference, see:
- [Language Documentation](docs/index.md)

## Roadmap

See the [project board](https://github.com/users/0xM4LL0C/projects/14) for planned features and updates.

## License

YUJI is licensed under the [MIT License](LICENSE).
