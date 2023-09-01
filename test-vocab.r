read_vocab <- function(path) {
  con <- file(path,"rb")
  on.exit(close(con))

  vocab_size <- readBin(con, "integer")
  token_len <- vector("integer", vocab_size)
  token <- vector("character", vocab_size)

  for (i in 1:vocab_size) {
    token_len[i] <- readBin(con, "integer")
    token[i] <- rawToChar(readBin(con, "raw", n = token_len[i]))

    #cat(sprintf("[%i] %s (%i)\n", i, token, token_len))
  }

  tibble::tibble(token = token, size = token_len)
}

vocab <- read_vocab("~/c-proj/bert.c/vocab.bin")

