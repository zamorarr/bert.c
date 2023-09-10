# choose compiler
# example override to clang: make tokenizer CC=clang
CC = gcc

# build tokenizer
.PHONY: tokenizer
tokenize: src/tokenize.c
	$(CC) -g -O0 -o build/tokenize src/tokenize.c -lm

# clean
clean:
	rm -f build