#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h> // tolower

#define FAIL(...) \
  do { \
    fprintf(stderr, __VA_ARGS__); \
    exit(EXIT_FAILURE); \
  } while (0)

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
  if (!file) FAIL("couldn't load %s\n", path);

  // read vocab size
  if (fread(&t->vocab_size, sizeof(int), 1, file) != 1) FAIL("could not read vocab size\n");
  printf("vocab size: %i\n", t->vocab_size);

  // read max_token_size
  if (fread(&t->max_token_size, sizeof(int), 1, file) != 1) FAIL("could not read max_token_size\n");
  printf("max token size: %i\n", t->max_token_size);

  // malloc space to hold vocab
  t->vocab = (char**) malloc(t->vocab_size * sizeof(char*));
  unsigned int token_len;

  // read vocab
  for (int i = 0; i < t->vocab_size; i++) {
    // get token size
    if (fread(&token_len, sizeof(int), 1, file) != 1) FAIL("could not read token size for token %i\n", i);
    printf("[%i] %i bytes: ", i, token_len);
    
    // get token
    t->vocab[i] = (char*) malloc(token_len + 1); // +1 for the null terminator
    if (fread(t->vocab[i], token_len, 1, file) != 1) FAIL("could not read token %i\n", i);
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

void encode_word(Tokenizer *t, char *word, char *str_buffer, int *tokens, int *n_tokens) {
  if (word == NULL) FAIL("cannot encode NULL word\n");

  // 1. find largest substring in the vocab
  //   a. start from full word and keep shortening
  //   b. if none, return UNK
  // 3. recursively encode the remaining substring
  size_t len = strlen(word);
  int start = 0;
  int end = len;

  int *subtokens = malloc(len * sizeof(int));
  int n_subtokens = 0;
  int id;
  
  // look for largest substring
  while (start < len) {
    // loop until we have parsed every chr into a subtoken
    end = len;
    while (start < end) {
      // add substring to buffer
      if (start > 0) {
        // prefix with ## since substring does not begin at start of word
        strcpy(str_buffer, "##");
        strncpy(str_buffer + 2, word + start, end - start);
        str_buffer[end - start + 2] = '\0';
      } else {
        strncpy(str_buffer, word, end);
        str_buffer[end] = '\0';
      }

      // lookup substring in vocab
      id = str_lookup(str_buffer, t->vocab_sorted, t->vocab_size);

      // if substring is in vocab, add and break here
      if (id != -1) {
        subtokens[n_subtokens++] = id;
        break;
      }

      // otherwise continue looking at smaller substring
      end--;
    }

    // if could not find any match, stop looking
    if (id == -1) break;

    // otherwise start looking at remaining substring
    start = end;
  }

  if (id == -1) {
      // if no match found for at least one substring, the whole word is UNK
    tokens[(*n_tokens)++] = 0; // or whatever UNK id is
  } else {
    // if matches found, append subtokens to tokens
    for (int i = 0; i < n_subtokens; i++) {
      tokens[(*n_tokens)++] = subtokens[i];
    }
  }

  // memory clean up
  free(subtokens);
}

void encode(Tokenizer *t, const char *text, int *tokens, int *n_tokens) {
  // encode input text into a pre-allocated output tokens[] array
  if (text == NULL) FAIL("cannot encode NULL text\n");

  // tempoary buffer to store merge candidates?
  char *str_buffer = (char*) malloc((t->max_token_size + 1) * sizeof(char));

  // copy text (because strtok will modify when it runs)
  size_t len = strlen(text);
  char *copy = (char*) malloc((len + 1) * sizeof(char));
  strcpy(copy, text);

  // lowercase? normalize?
  for (int i = 0; i < len; i++) {
    copy[i] = tolower(copy[i]);
  }

  // split text into words
  const char *delims = " \t";
  char *word = strtok(copy, delims);

  while (word != NULL) {
    encode_word(t, word, str_buffer, tokens, n_tokens);
    word = strtok(NULL, delims);
  }

  // free memory
  free(str_buffer);
  free(copy);
}


// CLI
void error_usage() {
  fprintf(stderr, "Usage: tokenize path\n");
  fprintf(stderr, "Example: tokenize tokenizer.bin\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  char *tokenizer_path;
  char *prompt;

  // poor man's C argparse
  if (argc >= 3) { 
    tokenizer_path = argv[1];
    prompt = argv[2];
  } else {
    error_usage();
  }

  // build tokenizer
  Tokenizer tokenizer;
  build_tokenizer(&tokenizer, tokenizer_path);


  //printf("\nlookup tests\n");
  //printf("%s at %i\n", tokenizer.vocab[3], str_lookup(tokenizer.vocab[3], tokenizer.vocab_sorted, tokenizer.vocab_size));
  //printf("%s at %i\n", "blahblah", str_lookup("blahblah", tokenizer.vocab_sorted, tokenizer.vocab_size));

  //char *str_buffer = (char*) malloc((tokenizer.max_token_size + 1) * sizeof(char));
  int *tokens = (int*) malloc((strlen(prompt) + 3) * sizeof(int)); // +3 for \0, BOS, EOS
  int n_tokens = 0;
  //encode_word(&tokenizer, prompt, str_buffer, tokens, &n_tokens);
  encode(&tokenizer, prompt, tokens, &n_tokens);

  printf("\nprompt: %s\n", prompt);
  printf("n_tokens: %i\n", n_tokens);

  for (int i = 0; i < n_tokens; i++) {
    printf("[%i] %s | id = %i\n", i, tokenizer.vocab[tokens[i]], tokens[i]);
  }

  // memory cleanup
  free_tokenizer(&tokenizer);
  free(tokens);

  return EXIT_SUCCESS;
}