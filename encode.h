#ifndef __ENCODE_H__
#define __ENCODE_H__


#include "huffman.h"
#include "common.h"


//Universal function to write unsigned short, int and long long
//mode should be 2, 4 or 8 (number of bytes)
int fwrite_int(unsigned long long variable, int mode, FILE *file);


//Writing header to header
int fwrite_header(Header *header, FILE *file);


//Converting tree to string
void write_tree(Node *head, char **buffer, int *size);


//Writing sigle symbol to file
//Don't forget about crc32
int fwrite_symbol(Code symbol, FILE *file, char *byte, int *count, unsigned long long *size, uint32_t *crc);


//Compressing filename to archivename
//Assuming that filenames are correct
int write_file(FILE *file, char *filename, FILE *archive);


#endif
