#ifndef __IO_H__
#define __IO_H__


#include "encode.h"
#include "decode.h"


//Checking crc32 sum of array
void check_crc(FILE *archive);


//Adding new file to array.
//Assuming that everything is ok.
//Return 0 if everything is moved to tmp.
//Return -1 if found file with same name, that not replaced.
int add_to_archive(FILE *tmp, FILE *archive);


int extract_from_archive(char *name, FILE *archive);


int extract_all(FILE *archive);


int delete_from_archive(char *name, FILE *archive, FILE *tmp);


#endif
