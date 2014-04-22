#ifndef __COMMON_H__
#define __COMMON_H__

//ORDER IN HEADER
//NAMESIZE
//FILENAME
//TREESIZE
//TREE
//FILESIZE
typedef struct {
	unsigned short int treesize;
	unsigned long long filesize;
	unsigned int namesize;
	char *filename;
	char *tree;
} Header;


//Mask for binary codes
static char TAB[] = {
    1, //0000001
    2,
    4,
    8,
    0x10,
    0x20,
    0x40,
    0x80 //10000000
};

#endif