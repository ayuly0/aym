# aym

Just a simple Hybird based VM in c for learning purpose

---

## ✨ Features

- Hybrid VM: Supports both register-based and stack-based execution
- Custom instruction set with math, logic, memory, and control flow
- Operand types: Register, Immediate, Memory
- Stack operations: PUSH, POP, DUP, SWAP, etc.
- Syscall interface for basic I/O
- Error and status code abstraction
- Optional bytecode file loading support
- Modular design, easy to expand

---

## 🧱 Architecture Overview

- **Stack:** Fixed-size data stack
- **Registers:** 14 general-purpose + special registers (IP, FLAGS, etc.)
- **Memory:** 1MB general-purpose memory
- **Instructions:** Hand-crafted IR (`Inst`) or serialized bytecode
- **Program:** Preloaded `Inst` array or bytecode file

---

## Build & Run

```bash
make
./build/aym
```

---

## 🔮 TODO

* Bytecode optimizer (SSA or linear scan)
* More syscalls (read, mmap, file I/O)
* Debugger interface or trace hooks
* JIT backend (DynASM, libgccjit)
* Lua/JS style scripting engine

---

## Reference

- [@tsoding](https://github.com/tsoding/) - [bm](https://github.com/tsoding/bm/)

---

## 📜 License

This project is licensed under the [MIT License](/LICENSE).
