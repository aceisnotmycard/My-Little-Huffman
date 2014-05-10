#include "list.h"


int list(FILE *archive) {
	unsigned long long bytes = 0;
	fseek(archive, 0, SEEK_SET);
	Header *header = malloc(sizeof(Header));
	while(!fread_header(header, archive)) {
		//fprintf(stderr, "\nNamesize: %d\n", header->namesize);
		fprintf(stderr, "\nName: %s\n", header->filename);
		bytes = header->filesize / 8 + (header->filesize % 8 != 0);
		fprintf(stderr, "Original size: %llu bytes\n", header->originalsize);

		fseek(archive, bytes, SEEK_CUR);
	}
}
