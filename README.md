# BF64

A standalone x64 operating system with a JIT brainfuck compiler.

## Building

Requires `gcc` and `xorriso` to build the iso.

`make` will download the limine bootloader repository and build the kernel elf.

`make iso` will build the ISO image.

`make run` or `make test` will run the ISO in qemu.
