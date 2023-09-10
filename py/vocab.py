# write a dummy vocab.bin file
# sample vocab taken from https://huggingface.co/learn/nlp-course/chapter6/6
import struct

# test vocab
vocab = ["[UNK]", "b", "h", "p", "##g", "##n", "##s", "##u", "##gs", "hu", "hug"]
vocab_size = len(vocab)

# bert vocab from a sentence transformer
with open("py/all-MiniLM-L6-v2-vocab.txt", "r") as f:
    vocab = [line.rstrip() for line in f]
    vocab_size = len(vocab)

# bytes vocab
tokens = [w.encode('utf-8') for w in vocab]
tokens_size = [len(b) for b in tokens]
max_token_size = max(tokens_size)

with open("build/vocab.bin", "wb") as f:
    f.write(struct.pack("II", vocab_size, max_token_size))
    for t,s in zip(tokens, tokens_size):
        f.write(struct.pack("I", s))
        f.write(t)