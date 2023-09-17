# choose compiler
# example override to clang: make tokenizer CC=clang
CC = gcc

# build tokenizer
.PHONY: tokenizer
tokenize: src/tokenize.c
	$(CC) \
	-Wall \
	-o build/tokenize \
	-g -O0 \
	src/tokenize.c \
	-l utf8proc

# clean
clean:
	rm -f build