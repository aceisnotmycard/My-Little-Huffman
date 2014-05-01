#include "io.h"

int check_crc(FILE *archive) {
	unsigned long long i;
	unsigned int byte;
	uint32_t crc = 0xFFFFFFFF;

	Header *header = malloc(sizeof(Header));
	fread_header(header, archive);

	for(i = 0; i < header->filesize; i++) {
		if(i % 8 == 0) {
			fread(&byte, sizeof(char), 1, archive);
			crc = crc32(crc, byte);
			if(byte == EOF) {
				return 1;
			}
		}
		show_progress(i, header->filesize);
	}
	fprintf(stderr, "\n");
	fclose(archive);
	return crc == header->crc;
}

int add_to_archive(FILE *tmp, FILE *archive) {
	unsigned long long i;
	unsigned int byte;
	unsigned long long bytes;
	unsigned long long tmp_bytes;
	fseek(tmp, 0, SEEK_SET);
	fseek(archive, 0, SEEK_SET);
	Header *tmp_header = malloc(sizeof(Header));
	Header *archive_header = malloc(sizeof(Header));

	if(fread_header(tmp_header, tmp)) {
		fprintf(stderr, "Cannot read header from tmp.\n");
		return 1;
	}
	tmp_bytes = tmp_header->filesize / 8 + (tmp_header->filesize % 8 != 0);
	fseek(tmp, tmp_bytes, SEEK_CUR);
	while(!fread_header(archive_header, archive)) {
		bytes = archive_header->filesize / 8 + (archive_header->filesize % 8 != 0);

		if(strcmp(archive_header->filename, tmp_header->filename)) {
			fwrite_header(archive_header, tmp);

			for(i = 0; i < bytes; i++) {
				if(fread(&byte, sizeof(char), 1, archive) != 1) {
					fprintf(stderr, "Cannot write %s to tmp", tmp_header->filename);
					return 1;
				}
				//Maybe EOF here?
				if(fwrite(&byte, sizeof(char), 1, tmp) != 1) {
					fprintf(stderr, "Cannot write %s to tmp", tmp_header->filename);
					return 1;
				}
			}
		} else {
			fprintf(stderr, "Found file with same name (%s). Replace? (y/n)\n", tmp_header->filename);
			byte = 0;
			while(1) {
				fread(&byte, sizeof(char), 1, stdin);
				if(byte == 'Y' || byte == 'y') {
					fseek(archive, bytes, SEEK_CUR);
					break;
				}
				if(byte == 'N' || byte == 'n') {
					return -1;
				}
			}
		}
	}
	return 0;
}

int extract_from_archive(char *name, FILE *archive) {
	fprintf(stderr, "Extracting %s...\n", name);
	fseek(archive, 0, SEEK_SET);
	FILE *file;
	Header *header = malloc(sizeof(Header));
	unsigned long long bytes;
	while(!fread_header(header, archive)) {
		if(!strcmp(name, header->filename)) {
			if(read_archive(archive, file, header))
				return 1;
		} else {
			bytes = header->filesize / 8 + (header->filesize % 8 != 0);
			fseek(archive, bytes, SEEK_CUR);
		}
	}
	return 0;
}
