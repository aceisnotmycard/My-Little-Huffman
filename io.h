#ifndef __IO_H__
#define __IO_H__

#include "encode.h"
#include "decode.h"

//Checking crc32 sum of array
int check_crc(FILE *archive);

//Adding new file to array.
//Assuming that everything is ok.
//Return 0 if everything is moved to tmp.
//Return -1 if found file with same name, that not replaced.
int add_to_archive(FILE *tmp, FILE *archive);


int extract_from_archive(char *name, FILE *archive);

#endif
