#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define FAIL(...) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }

// WordPiece Tokenizer
typedef struct {
  char *str;
  int id;
} TokenIndex;

typedef struct {
  char **vocab;
  unsigned int vocab_size;
  unsigned int max_token_size;
  TokenIndex *vocab_sorted;
} Tokenizer;

int compare_tokens(const void *a, const void *b) {
  return strcmp(((TokenIndex*) a)->str, ((TokenIndex*) b)->str);
}

int str_lookup(char *str, TokenIndex *vocab_sorted, unsigned int vocab_size) {
  // find match for str in sorted vocab, return index or -1
  TokenIndex key = { .str = str };
  TokenIndex *res = bsearch(&key, vocab_sorted, vocab_size, sizeof(TokenIndex), compare_tokens);
  return res != NULL ? res->id : -1;
}

void build_tokenizer(Tokenizer *t, char *path) {
  // read in file
  FILE *file = fopen(path, "rb");
  if (!file) FAIL("couldn't load %s\n", path)

  // read vocab size
  if (fread(&t->vocab_size, sizeof(int), 1, file) != 1) FAIL("could not read vocab size\n")
  printf("vocab size: %i\n", t->vocab_size);

  // read max_token_size
  if (fread(&t->max_token_size, sizeof(int), 1, file) != 1) FAIL("could not read max_token_size\n")
  printf("max token size: %i\n", t->max_token_size);

  // malloc space to hold vocab
  t->vocab = (char**) malloc(t->vocab_size * sizeof(char*));
  unsigned int token_len;

  // read vocab
  for (int i = 0; i < t->vocab_size; i++) {
    // get token size
    if (fread(&token_len, sizeof(int), 1, file) != 1) FAIL("could not read token size for token %i\n", i)
    printf("[%i] %i bytes: ", i, token_len);
    
    // get token
    t->vocab[i] = (char*) malloc(token_len + 1); // +1 for the null terminator
    if (fread(t->vocab[i], token_len, 1, file) != 1) FAIL("could not read token %i\n", i)
    t->vocab[i][token_len] = '\0'; // add null terminator

    // print token
    printf("%s\n", t->vocab[i]);
  }

  // close file
  fclose(file);

  // init sorted vocab
  t->vocab_sorted = (TokenIndex*) malloc(t->vocab_size * sizeof(TokenIndex));
  for (int i = 0; i < t->vocab_size; i++) {
    t->vocab_sorted[i].str = t->vocab[i];
    t->vocab_sorted[i].id = i;
  }
  qsort(t->vocab_sorted, t->vocab_size, sizeof(TokenIndex), compare_tokens);
}

void free_tokenizer(Tokenizer *t) {
  for (int i = 0; i < t->vocab_size; i++) { free(t->vocab[i]); }
  free(t->vocab);
  free(t->vocab_sorted);
}

void encode(Tokenizer *t, char *text, int8_t bos, int8_t eos, int *tokens, int *n_tokens) {
  // encode input text into a pre-allocated output tokens[] array
  if (text == NULL) FAIL("cannot encode NULL text\n")

  // split text into words

  // tempoary buffer to store merge candidates?
}

void encode_word(Tokenizer *t, char *word, char *str_buffer, int *tokens, int *n_tokens) {
  if (word == NULL) FAIL("cannot encode NULL word\n")

  // 1. find largest substring in the vocab
  //   a. start from full word and keep shortening
  //   b. if none, return UNK
  // 3. recursively encode the remaining substring
  size_t len = strlen(word);
  int id;
  
  // look for largest substring
  for (int i = len; i > 0; i--) {
    // copy substring into str_buffer
    strncpy(str_buffer, word, i);
    str_buffer[i] = '\0';

    // check if substring in vocab
    id = str_lookup(str_buffer, t->vocab_sorted, t->vocab_size);

    if (id != -1) {
      // found in vocab!
      tokens[(*n_tokens)++] = id;
      break;
    }
  }

  if (id == -1) {
    // no match found
    tokens[(*n_tokens)++] = 0; // or whatever the UNK token is
  }

  // continue along with remaining substring

  // return 0;
}

// CLI
void error_usage() {
  fprintf(stderr, "Usage: tokenize path\n");
  fprintf(stderr, "Example: tokenize tokenizer.bin\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  char *tokenizer_path;

  // poor man's C argparse
  if (argc >= 2) { 
    tokenizer_path = argv[1];
  } else {
    error_usage();
  }

  // build tokenizer
  Tokenizer tokenizer;
  build_tokenizer(&tokenizer, tokenizer_path);


  //printf("\nlookup tests\n");
  //printf("%s at %i\n", tokenizer.vocab[3], str_lookup(tokenizer.vocab[3], tokenizer.vocab_sorted, tokenizer.vocab_size));
  //printf("%s at %i\n", "blahblah", str_lookup("blahblah", tokenizer.vocab_sorted, tokenizer.vocab_size));

  char *str_buffer = (char*) malloc((tokenizer.max_token_size + 1) * sizeof(char));
  char *prompt = "hugs";
  int *tokens = (int*) malloc((strlen(prompt) + 3) * sizeof(int)); // +3 for \0, BOS, EOS
  int n_tokens = 0;
  encode_word(&tokenizer, prompt, str_buffer, tokens, &n_tokens);

  printf("\nprompt: %s\n", prompt);
  printf("n_tokens: %i\n", n_tokens);

  for (int i = 0; i < n_tokens; i++) {
    printf("[%i] %s | id = %i\n", i, tokenizer.vocab[(*tokens + i)], *(tokens + i));
  }

  // memory cleanup
  free_tokenizer(&tokenizer);
  free(str_buffer);
  free(tokens);

  return EXIT_SUCCESS;
}