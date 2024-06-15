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

`make && ./silic examples/main.sil`

## Examples

`hello_world.sil`
```rust
use io;

pub func main() {
    io:println("hello world");
}
```
