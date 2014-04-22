#include <sys/stat.h>
#include "encode.h"
#include "decode.h"


typedef enum {
	ARCHIVE = 0,
	LIST,
	EXTRACT,
	HELP
} Mode;

Mode parse_input(char *argument);

const char* help_message =  "Usage: mylihaf -mode archive_name {files}\n" 
		   					"modes:\n" 
		   					"-a (archive)\n"
		   					"-x (extract)\n"
		   					"-l (list)\n";

int main(int argc, char *argv[]) {

	struct stat buffer;
	char *archivename;
	char *filename;
	FILE *archive;
	FILE *file;

	//Should be mode and archive's name at least.
	if(argc < 3) {
		fprintf(stderr, "%s", help_message);
		return 0;
	}


	archivename = argv[2];
	filename = argv[3];

	
	switch(parse_input(argv[1])) {
		case ARCHIVE:
			archive = fopen(archivename, "wb");
			if(archive == NULL) {
				fprintf(stderr, "Cannot open %s for writing.\n", archivename);
				return 1;
			}
			file = fopen(filename, "r");
			if(file == NULL) {
				printf("Cannot open %s for reading.\n", filename);
				return 1;
			}
			if(write_file(file, filename, archive)) {
				printf("Cannot archive %s.\n", filename);
			}
			break;
		case EXTRACT:
			archive = fopen(archivename, "rb");
			if(archive == NULL) {
				fprintf(stderr, "Cannot open %s for reading.\n", archivename);
				return 1;
			}
			if(read_archive(archive, file)) {
				printf("Cannot decode %s\n", archivename);
			}
			break;			
		case LIST:
			archive = fopen(archivename, "rb");
			if(archive == NULL) {
				fprintf(stderr, "Cannot open %s for reading.\n", archivename);
			}
			fprintf(stderr, "LIST %s\n", archivename);
			break;
		case HELP:
			fprintf(stderr, "%s", help_message);
			break;
		default:
			fprintf(stderr, "Something went wrong.\n");
	}
	return 0;
}

Mode parse_input(char *argument) {
	if(!strcmp(argument, "-a")) {
		return ARCHIVE;
	}
	if(!strcmp(argument, "-x")) {
		return EXTRACT;
	}
	if(!strcmp(argument, "-l")) {
		return LIST;
	}
	return HELP;
}