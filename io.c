#include "io.h"


void check_crc(FILE *archive) {
	unsigned long long i;
	unsigned int byte;
	uint32_t crc;

	Header *header = malloc(sizeof(Header));
	while (!fread_header(header, archive)) {
		crc = 0xFFFFFFFF;
		fprintf(stderr, "Checking... %s\n", header->filename);
		for (i = 0; i < header->filesize; i++) {
			if (i % 8 == 0) {
				fread(&byte, sizeof(char), 1, archive);
				crc = crc32(crc, byte);
				if (byte == EOF) {
					fprintf(stderr, "ERROR\n");
					return;
				}
			}
			show_progress(i, header->filesize);
		}
		fprintf(stderr, "\n");
		if (crc == header->crc) {
			fprintf(stderr, "OK\n");
		} else {
			fprintf(stderr, "DAMAGED\n");
		}
	}
	fclose(archive);
}


int add_to_archive(FILE *tmp, FILE *archive) {
	unsigned long long i;
	int byte;
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
			fprintf(stderr, "Merging files...\n");
			for(i = 0; i < bytes; i++) {
				if(fread(&byte, sizeof(char), 1, archive) != 1) {
					fprintf(stderr, "Cannot write %s to tmp", tmp_header->filename);
					return 1;
				}
				if(fwrite(&byte, sizeof(char), 1, tmp) != 1) {
					fprintf(stderr, "Cannot write %s to tmp", tmp_header->filename);
					return 1;
				}
				show_progress(i, bytes);
			}
			fprintf(stderr, "\nDone\n");
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

int extract_all(FILE *archive) {
	fseek(archive, 0, SEEK_SET);
	FILE *file;
	Header *header = malloc(sizeof(Header));
	unsigned long long bytes;
	while(!fread_header(header, archive)) {
		fprintf(stderr, "Extracting %s...\n", header->filename);
		if(read_archive(archive, file, header))
			return 1;
	}
	return 0;
}


int delete_from_archive(char *name, FILE *archive, FILE *tmp) {
	fseek(archive, 0 , SEEK_SET);
	Header *header = malloc(sizeof(Header));
	unsigned long long bytes;
	unsigned long long i;
	int byte;
	int trigger = -1;

	while(!fread_header(header, archive)) {
		bytes = header->filesize / 8 + (header->filesize % 8 != 0);

		if(!strcmp(name, header->filename)) {
			fseek(archive, bytes, SEEK_CUR);
			trigger = 0;
		} else {
			fwrite_header(header, tmp);

			for(i = 0; i < bytes; i++) {
				if(fread(&byte, sizeof(char), 1, archive) != 1) {
					return 1;
				}
				//Maybe EOF here?
				if(fwrite(&byte, sizeof(char), 1, tmp) != 1) {
					return 1;
				}
			}
		}
	}

	return trigger;
}
