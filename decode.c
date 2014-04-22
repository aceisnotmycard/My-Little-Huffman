#include "decode.h"


int fread16(unsigned short int *variable, FILE *file) {
    unsigned char buffer[2];
    int i = 0;
    *variable = 0;
    if(fread(buffer, 1, 2, file) != 2) {
        return 1;
    }
    while(i != 2) {
        *variable |= buffer[i] << (8 * (1 - i));
        i++;
    }   
    return 0;
}


int fread32(unsigned int *variable, FILE *file) {
    unsigned char buffer[4];
    int i = 0;
    *variable = 0;
    if(fread(buffer, 1, 4, file) != 4) {
        return 1;
    }
    while(i != 4) {
        *variable |= buffer[i] << (8 * (3 - i));
        i++;
    }
    return 0;
}


int fread64(unsigned long long *variable, FILE *file) {
    unsigned char buffer[8];
    int i = 0;
    *variable = 0;
    if(fread(buffer, 1, 8, file) != 8) {
        return 1;
    }
    while(i != 8) {
        *variable |= buffer[i] << (8 * (7 - i));
        i++;
    }
    return 0;
}


//REFACTOR
int fread_header(Header *header, FILE *file) {
    int i = 0;
    unsigned int *namesize = malloc(sizeof(unsigned int));
    unsigned short *treesize = malloc(sizeof(unsigned short));
    unsigned long long *filesize = malloc(sizeof(unsigned long long));
    char *filename = malloc(sizeof(char) * *namesize);
    char *tree = malloc(sizeof(char) * *treesize);
    if(fread32(namesize, file)) {
        return 1;
    }
    header->namesize = *namesize;

    fread(filename, 1, header->namesize, file);
    header->filename = filename;

    if(fread16(treesize, file)) {
        return 1;
    }
    header->treesize = *treesize;

    fread(tree, 1, header->treesize, file);
    header->tree = tree;

    if(fread64(filesize, file)) {
        return 1;
    }
    header->filesize = *filesize;

    return 0;
}


void read_tree(Node **head, char **buffer) {
    char ch;
    Node *x = create_node(0,0);
    *head = x;
    ch = (*buffer)[0];
    *buffer += 1;
    if(ch == 'N') {      
        x->ch = ALPHABET;
        read_tree(&x->left, buffer);
        read_tree(&x->right, buffer);
    } else {
        x->ch = (*buffer)[0];
        *buffer += 1;
   }
}


int read_archive(FILE *archive, FILE *output) {
    unsigned long long i;
    Node *head = create_node(ALPHABET, 0);
    int byte;

    Header *header = malloc(sizeof(Header));
    fread_header(header, archive);

    output = fopen(header->filename, "w");

    if(output == NULL) {
        return 1;
    }

    //printf("%u\n", header->namesize);
    //printf("%s\n", header->filename);
    //printf("%hu\n", header->treesize);
    //printf("%s\n", header->tree);
    //printf("%llu\n", header->filesize);
    //fseek(archive, 0, SEEK_SET);

    char *tree = malloc(header->treesize);
    memcpy(tree, header->tree, header->treesize);
    read_tree(&head, &tree);

    Node *current = head;
    unsigned long long size = header->filesize;
    for(i = 0; i < size; i++) {
        if(i % 8 == 0) {
            fread(&byte, 1, 1, archive);
            if(byte == EOF) {
                return 1;
            }
        }
        if(byte & TAB[7 - (i % 8)]) {
            current = current->right;
        } else {
            current = current->left;
        }
        if(current == NULL) return 1;

        if(current->ch < ALPHABET) {
            fwrite(&current->ch, 1, 1, output);
            current = head;
        }

        //Showing progress
        if ((int) 100 * (i+1)/size > (int) 100 * i/size) {
            fprintf(stderr, ".");
        }
    }
    fprintf(stderr, "\n");
    fclose(archive);
    fclose(output);
    //remove(archivename);
    return 0;
}