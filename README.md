# YUJI

[![build](https://github.com/0xM4LL0C/yuji/actions/workflows/build.yaml/badge.svg)](https://github.com/0xM4LL0C/yuji/actions/workflows/build.yaml)
[![CodeQL](https://github.com/0xM4LL0C/yuji/actions/workflows/github-code-scanning/codeql/badge.svg)](https://github.com/0xM4LL0C/yuji/actions/workflows/github-code-scanning/codeql)

<img src="/assets/yuji-logo.png">

A simple programming language implementation written in C.

## Quick Start

### Installation

```bash
git clone https://github.com/0xM4LL0C/yuji.git
cd yuji
git checkout tags/v0.2.0  # you can specify the version you want to install
```

### Build

```bash
make build
```

### Install

```bash
sudo make install
```

You can specify the installation prefix using the `PREFIX` variable:

```bash
sudo make install PREFIX=/usr/locals
```

### Uninstall

```bash
sudo make uninstall
```

### Run

```bash
yuji test.yuji
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
  Time (mean ± σ):       2.9 ms ±   0.5 ms    [User: 0.7 ms, System: 1.6 ms]
  Range (min … max):     2.1 ms …   5.4 ms    797 runs

Benchmark 2: python3 benchmark/factorial.py
  Time (mean ± σ):     293.8 ms ±  15.5 ms    [User: 139.1 ms, System: 145.7 ms]
  Range (min … max):   275.7 ms … 318.1 ms    10 runs

Benchmark 3: lua benchmark/factorial.lua
  Time (mean ± σ):       4.6 ms ±   0.9 ms    [User: 1.5 ms, System: 2.3 ms]
  Range (min … max):     3.5 ms …  14.5 ms    207 runs

Summary
  ./.build/yuji benchmark/factorial.yuji ran
    1.59 ± 0.41 times faster than lua benchmark/factorial.lua
  102.25 ± 18.23 times faster than python3 benchmark/factorial.py
```

### Versions

- yuji: 0.2.0
- python: 3.14.0
- lua: 5.4.8

### Running Benchmarks

Dependencies:
- [hyperfine](https://github.com/sharkdp/hyperfine)

```bash
make benchmark
```

## Roadmap

See the [project board](https://github.com/users/0xM4LL0C/projects/14) for planned features and updates.

## License

YUJI is licensed under the [MIT License](LICENSE).
