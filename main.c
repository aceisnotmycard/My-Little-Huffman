#include <sys/stat.h>
#include "io.h"


typedef enum {
	ARCHIVE = 0,
	LIST,
	EXTRACT,
	CHECK,
	HELP
} Mode;

Mode parse_input(char *argument);

const char* help_message =  "Usage: mylihaf -mode archive_name {files}\n"
		   				"modes:\n"
		   				"-a (archive)\n"
		   				"-x (extract)\n"
		   				"-l (list)\n"
						   "-t (check crc32 sum)\n";

int main(int argc, char *argv[]) {

	struct stat buffer;
	char *archivename;
	char *filename;
	FILE *archive;
	FILE *file;
	FILE *tmp;
	int fd = -1;
	int i;
	char tmp_name[] = "TMP_ARCHIVE_XXXXXX";

	//Should be mode and archive's name at least.
	if(argc < 3) {
		fprintf(stderr, "%s", help_message);
		return 0;
	}

	archivename = argv[2];

	switch(parse_input(argv[1])) {
		case ARCHIVE:
			for(i = 3; i < argc; i++) {
				fd = mkstemp(tmp_name);
				if(fd == -1) {
					fprintf(stderr, "Cannot create tmp file.\n");
					return 1;
				}

				tmp = fdopen(fd, "w+b");

				file = fopen(argv[i], "rb");
				if(file == NULL) {
					fprintf(stderr, "Cannot open %s for reading.\n", argv[i]);
					return 1;
				}
				if(write_file(file, argv[i], tmp)) {
					fprintf(stderr, "Cannot archive %s.", argv[i]);
					return 1;
				}

				if(i == 3 && stat(archivename, &buffer)) {
					archive = fopen(archivename, "w+b");
					if(archive == NULL) {
						fprintf(stderr, "Cannot open %s for writing.\n", archivename);
						return 1;
					}
					rename(tmp_name, archivename);
				} else {
					archive = fopen(archivename, "r+b");
					if(archive == NULL) {
						fprintf(stderr, "Cannot open %s for writing.\n", archivename);
						return 1;
					}
					switch(add_to_archive(tmp, archive)) {
						case -1:
							remove(tmp_name);
							break;
						case 0:
							remove(archivename);
							rename(tmp_name, archivename);
							break;
						case 1:
							return 1;
							break;
					}
				}
			}
			break;
		case EXTRACT:
			archive = fopen(archivename, "rb");
			if(archive == NULL) {
				fprintf(stderr, "Cannot open %s for reading.\n", archivename);
				return 1;
			}
			for(i = 3; i < argc; i++) {
				if(extract_from_archive(argv[i], archive)) {
					fprintf(stderr, "Cannot extract %s.\n", argv[i]);
					return 1;
				}
			}
			break;
		case LIST:
			archive = fopen(archivename, "rb");
			if(archive == NULL) {
				fprintf(stderr, "Cannot open %s for reading.\n", archivename);
				return 1;
			}
			list(archive);
			break;
		case HELP:
			fprintf(stderr, "%s", help_message);
			break;

		case CHECK:
			archive = fopen(archivename, "rb");
			if(archive == NULL) {
				fprintf(stderr, "Cannot open %s for reading.\n", archivename);
				return 1;
			}
			if(check_crc(archive)) {
				fprintf(stderr, "OK\n");
			} else {
				fprintf(stderr, "DAMAGED\n");
			}
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
	if(!strcmp(argument, "-t")) {
		return CHECK;
	}
	return HELP;
}
