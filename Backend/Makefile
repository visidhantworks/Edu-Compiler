# ============================================================
#  Educational Mini Compiler v2.0 — Makefile
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wno-unused-function -g

TARGET  = mini_compiler

all: $(TARGET)

# 1. Generate parser (C + header) from Bison grammar
mini_compiler.tab.c mini_compiler.tab.h: mini_compiler.y
	bison -d mini_compiler.y

# 2. Generate lexer from Flex spec
lex.yy.c: mini_compiler.l mini_compiler.tab.h
	flex mini_compiler.l

# 3. Link everything into one binary
$(TARGET): mini_compiler.tab.c lex.yy.c
	$(CC) $(CFLAGS) -o $@ mini_compiler.tab.c lex.yy.c -lfl

clean:
	rm -f $(TARGET) mini_compiler.tab.c mini_compiler.tab.h lex.yy.c

.PHONY: all clean
