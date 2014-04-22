#include "encode.h"


//Universal function to write unsigned short, int and long long
//mode should be 2, 4 or 8 (number of bytes)
int fwrite_int(unsigned long long variable, int mode, FILE *file) {
    int i = 0;
    while(i != mode) {
        char tmp = (variable >> 8 * (mode - 1 - i)) & 0xFF;
        if (fwrite(&tmp, 1, 1, file) == EOF) {
            return 1;
        }
        i++;
    }
    return 0;
}


int fwrite_header(Header *header, FILE *file) {
    if (fwrite_int(header->namesize, 4 ,file)) {
        printf("cannot write namesize\n");
        return 1;
    }
    if (fwrite(header->filename, sizeof(char), header->namesize, file) != header->namesize) {
        printf("cannot write filename\n");
        return 1;
    }
    if (fwrite_int(header->treesize, 2, file)) {
        printf("cannot write treesize\n");
        return 1;
    }
    if (fwrite(header->tree, sizeof(char), header->treesize, file) != header->treesize) {
        printf("cannot write tree\n");
        return 1;
    }
    if (fwrite_int(header->filesize, 8, file)) {
        printf("cannot write filesize\n");
        return 1;
    }
    return 0;
}


void write_tree(Node *head, char **buffer) {
    if(head->ch > ALPHABET - 1) {
        snprintf(*buffer, TREE_HEIGHT, "N");
        *buffer += 1;
        write_tree(head->left, buffer);
        write_tree(head->right, buffer);
    } else {
        snprintf(*buffer, TREE_HEIGHT, "C%c", head->ch);
        *buffer += 2;
    }
}


//Don't forget about crc32
int fwrite_symbol(Code symbol, FILE *file, char *byte, int *count, unsigned long long *size) {
    int  cursor = 0;
    while (cursor < symbol.size) {
        if(symbol.code[cursor] - '0') {
            *byte |= TAB[*count];
        }
        (*count)--;
        cursor++;
        if(*count == -1) {
            if(fwrite(byte, 1, 1, file) != 1) {
            	return 1;
            }
            *count = 7;
            *byte = 0;
            *size += 8;
        }
    }
    return 0;
}


int write_file(FILE *file, char *filename, FILE *archive) {
    int ch;
    int length = 0;
    unsigned long long size = 0;
    int count = 7;
    char byte = 0;

    Header *header = malloc(sizeof(Header));

    Node *head = create_huffman_tree(file);
    Code table[ALPHABET] = {0,0};
    create_coding_table(head, "", table);
    char *buffer = malloc(sizeof(char) * TREE_HEIGHT * ALPHABET);
    char *p = buffer;
    write_tree(head, &p);

    header->namesize = strlen(filename);
    header->filename = filename;
    header->tree = buffer;
    header->treesize = strlen(buffer) + 1;
    header->filesize = 0;

    if (fwrite_header(header, archive)) {
    	return 1;
    }
    fseek(file, 0, SEEK_SET);
    while((ch = fgetc(file)) != EOF) {
        if(fwrite_symbol(table[ch], archive, &byte, &count, &size)) {
        	return 1;
        }
    }
    if(count != 7) {
        if(fwrite(&byte, 1, 1, archive) != 1) {
        	return 1;
        }
        size += 7 - (count);
    }
    header->filesize = size;
    fseek(archive, 0, SEEK_SET);
    if (fwrite_header(header, archive)) {
    	return 1;
    }  
    //printf("%u\n", header->namesize);
    //printf("%s\n", header->filename);
    //printf("%hu\n", header->treesize);
    //printf("%s\n", header->tree);
    //printf("%llu\n", header->filesize);
    fclose(file);
    fclose(archive);
    return 0;
}