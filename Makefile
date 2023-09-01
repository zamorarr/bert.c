# choose compiler
# example override to clang: make tokenizer CC=clang
CC = gcc

# build tokenizer
tokenizer: tokenizer.c
	$(CC) -O3 -o tokenizer tokenizer.c -lm

# clean
clean:
	rm -f tokenizer