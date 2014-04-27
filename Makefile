CC = clang
FILES = main.c encode.c decode.c huffman.c common.c
OUT = mylihaf
build:
	$(CC) -o $(OUT) $(FILES)
