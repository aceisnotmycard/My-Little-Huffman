#include "list.h"

int list(FILE *archive) {
	unsigned long long bytes = 0;
	fseek(archive, 0, SEEK_SET);
	Header *header = malloc(sizeof(Header));
	while(!fread_header(header, archive)) {
		fprintf(stderr, "\nNAME: %s\n", header->filename);
		bytes = header->filesize / 8 + (header->filesize % 8 != 0);
		fprintf(stderr, "SIZE: %llu bytes\n", bytes);

		fseek(archive, bytes, SEEK_CUR);
	}
}
