<h1>
    <img src="./misc/logo.png" width="100%" alt="silic logo">
</h1>

<div align="center">
    <h4>
        A tiny systems langauge with an emphasis on simplicity.
    </h4>
    <img align="center" alt="Static Badge" src="https://img.shields.io/badge/Version-0.0.0-green">
    <img align="center" alt="Static Badge" src="https://img.shields.io/badge/License-MIT-blue">
</div>


<hr>

> **Note**
>
> ***The language is still early in development and is subject to change.***

## Documentation

`todo...`

## Building

`git clone --recurse-submodules https://gitea.com/cohnon/silic`

`cd silic`

`make && ./silic test/hello_world/main.c`

## Examples

`hello_world.sil`
```rust
use std;

fn main() {
    std:print("hey, there!\n");
}
```

`generics.sil`
```rust
use cstd;

fn alloc(T: type, usize len) -> T {
    cstd:malloc(T.size * len) as *T
}
```

`barebones.sil`
```rust
fn _start() -> unreachable { 
    // write syscall
    asm volatile (
        a = 1,
        D = 1,
        S = "hey, there!\n",
        d = 12,
        rcx, r11
    ) -> a { "syscall" }

    // exit syscall
    asm volatile (
        a = 60,
        D = 0,
        rcx, r11
    ) -> a { "syscall" }

    unreachable
}
```
