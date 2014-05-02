#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <stdio.h>

//ORDER IN HEADER
//NAMESIZE
//FILENAME
//TREESIZE
//TREE
//CRC
//ORIGINALSIZE
//FILESIZE
typedef struct {
	unsigned short int treesize;
	unsigned long long originalsize;
	unsigned long long filesize;
    uint32_t crc;
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

//Generating crc sum
uint32_t crc32(uint32_t crc, unsigned char buf);

//Showing percentage
void show_progress(unsigned long long i, unsigned long long size);

#endif
