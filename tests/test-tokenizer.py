from transformers import AutoTokenizer
tokenizer = AutoTokenizer.from_pretrained('sentence-transformers/all-MiniLM-L6-v2')

def ids_are(input, expected):
  actual = input['input_ids'][0].numpy().tolist()
  assert actual == expected

prompt = ["Hello there friend!"]
input = tokenizer(prompt, padding=True, truncation=True, return_tensors='pt')
ids_are(input, [101, 7592, 2045, 2767, 999, 102])

prompt = ["I don't sleep cuz sleep is the cousin of death."]
input = tokenizer(prompt, padding=True, truncation=True, return_tensors='pt')
ids_are(input, [101, 1045, 2123, 1005, 1056, 3637, 12731, 2480, 3637, 2003, 1996, 5542, 1997, 2331, 1012,  102])

prompt = ["hello?goodbye"]
input = tokenizer(prompt, padding=True, truncation=True, return_tensors='pt')
ids_are(input, [101, 7592, 1029, 9119, 102])

prompt = ["hello\ngoodbye"]
input = tokenizer(prompt, padding=True, truncation=True, return_tensors='pt')
ids_are(input, [101, 7592, 9119, 102])

prompt = ["poof deleon"]
input = tokenizer(prompt, padding=True, truncation=True, return_tensors='pt')
ids_are(input, [101, 13433, 11253, 3972, 10242, 102])

prompt = ["poivrée"]
input = tokenizer(prompt, padding=True, truncation=True, return_tensors='pt')
ids_are(input, [101, 13433, 12848, 9910, 102])
tokenizer.decode([101, 13433, 12848, 9910, 102])