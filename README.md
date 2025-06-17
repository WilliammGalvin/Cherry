# 🍒 Cherry Programming Language

⚠️ **Work in Progress**  
This repository is an ongoing project, and many features are still under development.

Cherry is a high-level, multi-purpose programming language designed with both power and clarity in mind. My long-term vision is to evolve Cherry into something unique yet familiar, bridging the gap between robust system-level programming and elegant, modern syntax. The ultimate goal is to provide a full-featured compiler and language ecosystem that enables developers to build real-world applications confidently.

[Documentation Plans](https://www.notion.so/Cherry-Documentation-v0-1-20fdaf69839b80a8b282c12c5b44be91?source=copy_link)

---

### 🛠️ Compiler Pipeline

The Cherry compiler has undergone several design iterations. The current architecture is a modern, multi-stage pipeline aimed at correctness, extensibility, and performance.

---

### 🔹 Lexical Analysis

Like most language compilers, Cherry begins with lexical analysis, breaking source code into a sequence of meaningful tokens.

---

### 🔹 Syntactic Analysis

This phase parses tokens into a structured Abstract Syntax Tree (AST). Cherry performs syntactic analysis in two stages:

1. **Raw Parsing** - The token stream is parsed into a basic AST structure.
2. **Semantic Enhancement** - The AST is enriched with semantic information, including type checking, scope resolution, and early validation of language features. This phase also supports Cherry’s unique language capabilities, such as customizable control flow constructs and context-aware type inference (planned), which offer both safety and expressive power.

---

### 🔹 Cherry IR

Once the AST is validated and enriched, it is compiled into **Cherry Intermediate Representation (IR)**, a lower-level but language-agnostic form that simplifies optimization and backend code generation.

---

### 🔹 LLVM IR

Finally, Cherry IR is translated into **LLVM Intermediate Representation**, leveraging LLVM's mature backend toolchain. This makes Cherry portable across platforms and architectures while benefiting from decades of compiler research and optimization techniques.
