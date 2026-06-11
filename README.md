# Educational Mini Compiler (Flex + C)

A beginner-friendly lexical analyser built with **Flex (Lex)** that:
- Recognises all standard C tokens
- Detects common beginner mistakes
- Prints **human-readable** error messages with explanations and fixes

---

## Project Files

| File               | Purpose                                      |
|--------------------|----------------------------------------------|
| `step1_basic.l`    | Step 1 – bare-bones Flex setup               |
| `step2_tokens.l`   | Step 2 – token recognition                   |
| `step3_errors.l`   | Step 3 – basic error detection               |
| `step4_friendly.l` | Step 4 – rich beginner-friendly messages     |
| `mini_compiler.l`  | ⭐ Step 5 – final complete version           |
| `test_input.c`     | Sample C file with intentional errors        |
| `build_and_run.sh` | Automated build + run script                 |

---

## Quick Start

### 1. Install Flex

```bash
# Ubuntu / Debian / WSL
sudo apt-get install flex

# macOS (Homebrew)
brew install flex

# Verify
flex --version
```

### 2. Build & Run (automated)

```bash
chmod +x build_and_run.sh
./build_and_run.sh
```

### 3. Build manually

```bash
flex mini_compiler.l          # generates lex.yy.c
gcc lex.yy.c -o mini_compiler # compiles it
./mini_compiler test_input.c  # run on a file
```

### 4. Run on your own file

```bash
./mini_compiler your_code.c
```

---

## What It Detects

| Error / Warning               | Example                     |
|-------------------------------|-----------------------------|
| Missing semicolon             | `int x = 5` (no `;`)        |
| Invalid identifier (digit-start) | `3badname`               |
| Unclosed string literal       | `"hello world` (no `"`)     |
| Unmatched parenthesis         | `(a + b` without `)`        |
| Unmatched brace               | `{ int x; ` without `}`     |
| Invalid symbols               | `@`, `$`, backtick          |
| Single-letter variable names  | `int a;` (warning)          |

---

## Error Message Format

Every error is printed in a box like this:

```
  ┌──────────────────────────────────────────────────┐
  │  ❌  ERROR at Line 7                              │
  ├──────────────────────────────────────────────────┤
  │  🔍 What happened:                               │
  │     Missing semicolon at end of statement        │
  │                                                  │
  │  📖 Why it's wrong:                              │
  │     Every C statement must end with ';'          │
  │                                                  │
  │  💡 Suggested fix:                               │
  │     Add ';' at the end → int total = 15;         │
  └──────────────────────────────────────────────────┘
```

---

## How Flex Works (Quick Reference)

```
[Definitions Section]    ← #includes, named patterns
%%
[Rules Section]          ← pattern  { action }
%%
[User Code]              ← main(), helper functions
```

- `yytext`  – the matched text string  
- `yylex()` – starts scanning  
- `yyin`    – input file pointer (default: stdin)  
- `line_number` – manually tracked via `\n` rules  

---

## Step-by-Step Learning Path

```
Step 1 → Basic Flex skeleton — runs and reads input
Step 2 → Token recognition — keywords, identifiers, operators
Step 3 → Error detection — semicolons, invalid names
Step 4 → Rich error messages — box format, colours, context
Step 5 → Final compiler — all features combined
```

---

## Extending the Compiler

Ideas for your next learning steps:

1. **Add a symbol table** – track declared variables, detect use-before-declare
2. **Integrate with Yacc/Bison** – add grammar rules (proper parsing)
3. **Add type checking** – warn when assigning float to int
4. **Track column numbers** – show exact error position like GCC does
5. **Output to file** – write a compiler report to `.log`
