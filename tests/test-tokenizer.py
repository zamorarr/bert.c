from transformers import AutoTokenizer
tokenizer = AutoTokenizer.from_pretrained('sentence-transformers/all-MiniLM-L6-v2')

prompt = ["Hello there friend!"]
encoded_input = tokenizer(prompt, padding=True, truncation=True, return_tensors='pt')
print(encoded_input)