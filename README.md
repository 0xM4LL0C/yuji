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

## Benchmark

```bash
Benchmark 1: ./.build/yuji benchmark/factorial.yuji
  Time (mean ± σ):       2.7 ms ±   0.8 ms    [User: 0.6 ms, System: 1.4 ms]
  Range (min … max):     1.8 ms …  10.0 ms    1028 runs

Benchmark 2: python3 benchmark/factorial.py
  Time (mean ± σ):     319.8 ms ±  22.2 ms    [User: 145.5 ms, System: 164.4 ms]
  Range (min … max):   293.1 ms … 359.4 ms    10 runs

Benchmark 3: lua benchmark/factorial.lua
  Time (mean ± σ):       5.2 ms ±   1.3 ms    [User: 1.6 ms, System: 2.6 ms]
  Range (min … max):     3.6 ms …  15.4 ms    522 runs

Summary
  ./.build/yuji benchmark/factorial.yuji ran
    1.91 ± 0.73 times faster than lua benchmark/factorial.lua
  118.67 ± 35.52 times faster than python3 benchmark/factorial.py
```

### Versions

- yuji: 0.1.0
- python: 3.13.5
- lua: 5.4.8

### Running Benchmarks

Dependencies:
- [hyperfine](https://github.com/sharkdp/hyperfine)

```bash
hyperfine -N \
    "./.build/yuji benchmark/factorial.yuji" \
    "python3 benchmark/factorial.py" \
    "lua benchmark/factorial.lua"
```

## Roadmap

See the [project board](https://github.com/users/0xM4LL0C/projects/14) for planned features and updates.

## License

YUJI is licensed under the [MIT License](LICENSE).
