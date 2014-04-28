#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ALPHABET 256
#define TREE_HEIGHT 100
#define MAX_CODE_SIZE 32


//Unified structure for tree and list
typedef struct Node {
	size_t weight;
	int ch;

	//for list
	struct Node *next;

	//for tree
	struct Node *left;
	struct Node *right;
} Node;


//Structure that holding symbol's codes
// Based on frequency
typedef struct Code {
	char code[MAX_CODE_SIZE];
	int size;
} Code;


//Creating Node
Node *create_node(int ch, size_t weight);


//Making frequency table
size_t *count_frequency(FILE *file);


//Converting to sorted linked list
Node *make_list(size_t *table);


//Converting to Huffman tree
Node *make_tree(Node *current);


//Making binary codes
void create_coding_table(Node *node, char *prev, Code *table);


//General functions
Node *create_huffman_tree(FILE *file);

//Debugging functions

#endif
