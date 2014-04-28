CC = gcc-4.9
FILES = main.c encode.c decode.c huffman.c common.c list.c io.c
OUT = mylihaf
build:
	$(CC) -o $(OUT) $(FILES)
