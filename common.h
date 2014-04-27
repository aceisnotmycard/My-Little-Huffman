#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

//ORDER IN HEADER
//NAMESIZE
//FILENAME
//TREESIZE
//TREE
//!!!CRC
//FILESIZE
typedef struct {
	unsigned short int treesize;
	unsigned long long filesize;
    //uint32_t crc;
	unsigned int namesize;
	char *filename;
	char *tree;
} Header;


typedef enum {
	CANNOT_READ_NAMESIZE = 1
} ERROR;

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



uint32_t crc32(uint32_t crc, char buf);

#endif
