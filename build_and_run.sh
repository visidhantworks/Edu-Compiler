#!/bin/bash
# ═══════════════════════════════════════════════
#  Build & Run Script — Educational Mini Compiler
# ═══════════════════════════════════════════════

set -e   # stop on any error

BOLD="\033[1m"
GREEN="\033[1;32m"
RED="\033[1;31m"
CYAN="\033[1;36m"
RESET="\033[0m"

echo -e "${CYAN}${BOLD}"
echo "╔══════════════════════════════════════════╗"
echo "║    Mini Compiler — Build System          ║"
echo "╚══════════════════════════════════════════╝"
echo -e "${RESET}"

# ── 1. Check Flex is installed ──────────────────
echo -e "${BOLD}[1/4] Checking for flex...${RESET}"
if ! command -v flex &> /dev/null; then
    echo -e "${RED}Flex not found! Install it:${RESET}"
    echo "  Ubuntu/Debian : sudo apt-get install flex"
    echo "  macOS         : brew install flex"
    exit 1
fi
echo -e "      ${GREEN}✓ flex $(flex --version | head -1)${RESET}"

# ── 2. Run Flex to generate C code ─────────────
echo -e "${BOLD}[2/4] Running flex on mini_compiler.l ...${RESET}"
flex mini_compiler.l
echo -e "      ${GREEN}✓ lex.yy.c generated${RESET}"

# ── 3. Compile with GCC ─────────────────────────
echo -e "${BOLD}[3/4] Compiling with gcc ...${RESET}"
gcc lex.yy.c -o mini_compiler
echo -e "      ${GREEN}✓ mini_compiler binary built${RESET}"

# ── 4. Run against test file ────────────────────
echo -e "${BOLD}[4/4] Running against test_input.c ...${RESET}\n"

if [ -f "test_input.c" ]; then
    ./mini_compiler test_input.c
else
    echo "  (No test_input.c found — running in stdin mode)"
    echo "  Type C code and press Ctrl+D when done."
    ./mini_compiler
fi
