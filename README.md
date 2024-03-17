# c-tokenizer
C tokenizer based on video by Andrej Karpathy

for memory allocation troubleshooting run: `gcc tokenizer.c decoder.c encoder.c -o tokenizer -fsanitize=address && ./tokenizer < input.txt`