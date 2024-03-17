# GPT 2 tokenizer
GPT 2 tokenizer based on video by Andrej Karpathy

excluding:
- <|ENDOFTEXT|>
- regex for splitting into tokens is only partial (no negative lookahead in POSIX)

for memory allocation troubleshooting run: `gcc tokenizer.c decoder.c encoder.c -o tokenizer -fsanitize=address && ./tokenizer < input.txt`