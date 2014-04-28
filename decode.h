#ifndef __DECODE_H__
#define __DECODE_H__

#include "huffman.h"
#include "common.h"


//Reading 2, 4, 8 bytes from file to variable
int fread16(unsigned short int *variable, FILE *file);

int fread32(unsigned int *variable, FILE *file);

int fread64(unsigned long long *variable, FILE *file);


//Reading header from archive
int fread_header(Header *header, FILE *file);


//Reading tree in string format and converting it to tree
void read_tree(Node **head, char **buffer);


//Reading archive's body
int read_symbols(FILE *archive, FILE *output, Node *head, unsigned long long size);


//Reading from archive and writing to output
//output should be null pointer;
//Assuming that archive is correct
int read_archive(FILE *archive, FILE *output, Header *header);

#endif
