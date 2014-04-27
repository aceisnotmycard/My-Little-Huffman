#include "decode.h"


int fread16(unsigned short int *variable, FILE *file) {
    unsigned char buffer[2];
    int i = 0;
    *variable = 0;
    if(fread(buffer, sizeof(char), 2, file) != 2) {
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
    if(fread(buffer, sizeof(char), 4, file) != 4) {
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
    if(fread(buffer, sizeof(char), 8, file) != 8) {
        return 1;
    }
    while(i != 8) {
        *variable |= (unsigned long long) buffer[i] << (8 * (7 - i));
        printf("MEM %u\n", buffer[i] << (8 * (7 - i)));
        printf("VAR %llu\n", *variable);
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
    char *filename;
    char *tree;
    fprintf(stderr, "\n------------\n");
    fprintf(stderr, "%lu\n", ftell(file));
    if(fread32(namesize, file)) {
        return 1;
    }
    fprintf(stderr, "%lu\n", ftell(file));
    header->namesize = *namesize;
    filename = malloc(sizeof(char) * *namesize);
    fread(filename, sizeof(char), header->namesize, file);
    header->filename = filename;
    fprintf(stderr, "%lu\n", ftell(file));
    if(fread16(treesize, file)) {
        return 1;
    }
    header->treesize = *treesize;
    tree = malloc(sizeof(char) * *treesize);
    fprintf(stderr, "%lu\n", ftell(file));
    fread(tree, sizeof(char), header->treesize, file);
    header->tree = tree;
    fprintf(stderr, "%lu\n", ftell(file));
    if(fread64(filesize, file)) {
        return 1;
    }
    header->filesize = *filesize;
    fprintf(stderr, "%lu\n", ftell(file));
    fprintf(stderr, "\n------------\n");
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

void print_tree(Node *head) {
    if(head != NULL) {
        fprintf(stderr,"%c", head->ch);
        print_tree(head->left);
        print_tree(head->right);
    }
}

int read_archive(FILE *archive, FILE *output) {
    unsigned long long i;
    Node *head = create_node(ALPHABET, 0);
    int byte;

    Header *header = malloc(sizeof(Header));
    fread_header(header, archive);

    printf("NAMESIZE %u\n", header->namesize);
    printf("FILENAME %s\n", header->filename);
    printf("TREESIZE %hu\n", header->treesize);
    printf("%s\n", header->tree);
    printf("\nFILESIZE %llu\n", header->filesize);

    read_tree(&head, &header->tree);
    print_tree(head);
    printf("\n");

    output = fopen(header->filename, "w");

    if(output == NULL) {
        return 1;
    }

    Node *current = head;
    for(i = 0; i < header->filesize; i++) {
        if(i % 8 == 0) {
            fread(&byte, sizeof(char), 1, archive);
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
            fwrite(&current->ch, sizeof(char), 1, output);
            current = head;
        }

        //Showing progress
        if ((int) 100 * (i+1)/header->filesize > (int) 100 * i/header->filesize) {
            fprintf(stderr, ".");
        }
    }
    fprintf(stderr, "\n");
    fclose(archive);
    fclose(output);
    //remove(archivename);
    return 0;
}
