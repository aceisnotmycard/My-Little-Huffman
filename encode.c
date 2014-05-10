#include "encode.h"
#include <sys/stat.h>


//Universal function to write unsigned short, int and long long
//mode should be 2, 4 or 8 (number of bytes)
int fwrite_int(unsigned long long variable, int mode, FILE *file) {
    int i = 0;
    while(i != mode) {
        char tmp = (variable >> 8 * (mode - 1 - i)) & 0xFF;
        if (fwrite(&tmp, sizeof(char), 1, file) == EOF) {
            return 1;
        }
        i++;
    }
    return 0;
}


int fwrite_header(Header *header, FILE *file) {

    if(fwrite_int(header->namesize, 4 ,file)) {
        printf("cannot write namesize\n");
        return 1;
    }

    if(fwrite(header->filename, sizeof(char), header->namesize, file) != header->namesize) {
        printf("cannot write filename\n");
        return 1;
    }

    if(fwrite_int(header->treesize, 2, file)) {
        printf("cannot write treesize\n");
        return 1;
    }

    if(fwrite(header->tree, sizeof(char), header->treesize, file) != header->treesize) {
        printf("cannot write tree\n");
        return 1;
    }

    if(fwrite_int(header->crc, 4, file)) {
        printf("cannot write crc\n");
        return 1;
    }

    if(fwrite_int(header->originalsize, 8, file)) {
        printf("cannot write filesize\n");
        return 1;
    }

    if(fwrite_int(header->filesize, 8, file)) {
        printf("cannot write filesize\n");
        return 1;
    }
    return 0;
}


void write_tree(Node *head, char **buffer, int *size) {
    if(head != NULL) {
        if(head->ch > ALPHABET - 1) {
            snprintf(*buffer, TREE_HEIGHT, "N");
            *size += 1;
            *buffer += 1;
            write_tree(head->left, buffer, size);
            write_tree(head->right, buffer, size);
        } else {
            snprintf(*buffer, TREE_HEIGHT, "C%c", head->ch);
            *size += 2;
            *buffer += 2;
        }
    }
}


int fwrite_symbol(Code symbol, FILE *file, char *byte, int *count, unsigned long long *size, uint32_t *crc) {
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
            //fprintf(stderr, "%d\n", *byte);
            *crc = crc32(*crc, *byte);
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
    unsigned long long i = 0;
    unsigned long long original_size;
    int count = 7;
    char byte = 0;
    uint32_t crc = 0xFFFFFFFF;
    struct stat buf;
    Code table[ALPHABET] = {0,0};
    char *buffer = calloc(TREE_HEIGHT * ALPHABET, sizeof(char));
    char *p = buffer;

    if(!stat(filename, &buf))
        original_size = buf.st_size;

    Node *head = create_huffman_tree(file);
    Header *header = malloc(sizeof(Header));

    if(head) {
        create_coding_table(head, "", table);
        write_tree(head, &p, &length);
    }
    header->namesize = strlen(filename);
    header->filename = filename;
    header->tree = buffer;
    header->treesize = length;
    header->crc = 0;
    header->originalsize = original_size;
    header->filesize = 0;

    if (fwrite_header(header, archive)) {
    	return 1;
    }
    fseek(file, 0, SEEK_SET);
    fprintf(stderr, "Encoding file...\n");
    while((ch = fgetc(file)) != EOF) {
        if(fwrite_symbol(table[ch], archive, &byte, &count, &size, &crc)) {
        	return 1;
        }
        show_progress(i++, original_size);
    }
    if(count != 7) {
        if(fwrite(&byte, 1, 1, archive) != 1) {
        	return 1;
        }
        crc = crc32(crc, byte);
        size += 7 - (count);
    }
    fprintf(stderr, "\n");

    header->crc = crc;
    header->filesize = size;
    fseek(archive, 0, SEEK_SET);
    if (fwrite_header(header, archive)) {
    	return 1;
    }
    fprintf(stderr, "\nDone\n");
    return 0;
}
