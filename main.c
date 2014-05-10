#include <sys/stat.h>
#include "io.h"


typedef enum {
	ARCHIVE = 0,
	LIST,
	EXTRACT,
	CHECK,
	DELETE,
	HELP
} Mode;


Mode parse_input(char *argument);


const char* help_message =  "Usage: mylihaf -mode archive_name {files}\n"
		   				"modes:\n"
		   				"-a (archive) + 1 and more files\n"
		   				"-x (extract) + 0 and more files\n"
		   				"-l (list) + 0 files\n"
						   "-t (check crc32 sum) + 0 files\n"
						   "-d (delete) + 1 file";


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
			if(argc == 3) {
				fprintf(stderr, "%s", help_message);
				return 0;
			}

			for(i = 3; i < argc; i++) {
				if(strstr(argv[i], "/")) {
						fprintf(stderr, "Cannot archive files from subdirectories.\n");
						return 1;
				}
				if(stat(argv[i], &buffer)) {
					fprintf(stderr, "Cannot open %s.\n", argv[i]);
					return 1;
				}
				if(!S_ISREG(buffer.st_mode)) {
					fprintf(stderr, "%s is not a regular file.\n", argv[i]);
				}
			}

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
					remove(tmp_name);
					return 1;
				}
				if(write_file(file, argv[i], tmp)) {
					fprintf(stderr, "Cannot archive %s.", argv[i]);
					remove(tmp_name);
					return 1;
				}

				if(i == 3 && stat(archivename, &buffer)) {
					archive = fopen(archivename, "w+b");
					if(archive == NULL) {
						fprintf(stderr, "Cannot open %s for writing.\n", archivename);
						remove(tmp_name);
						return 1;
					}
					rename(tmp_name, archivename);
				} else {
					archive = fopen(archivename, "r+b");
					if(archive == NULL) {
						fprintf(stderr, "Cannot open %s for writing.\n", archivename);
						remove(tmp_name);
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
				fclose(archive);
				fclose(tmp);
			}
			break;
		case EXTRACT:
			archive = fopen(archivename, "rb");
			if(archive == NULL) {
				fprintf(stderr, "Cannot open %s for reading.\n", archivename);
				return 1;
			}
			if (argc > 3) {
				for(i = 3; i < argc; i++) {
					if(extract_from_archive(argv[i], archive)) {
						fprintf(stderr, "Cannot extract %s.\n", argv[i]);
						return 1;
					}
				}
			} else {
				if(extract_all(archive)) {
					fprintf(stderr, "Cannot extract %s.\n", archivename);
					return 1;
				}
			}
			break;
		case LIST:

			if(argc != 3) {
				fprintf(stderr, "%s", help_message);
				return 0;
			}

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

			if(argc != 3) {
				fprintf(stderr, "%s", help_message);
				return 0;
			}

			archive = fopen(archivename, "rb");
			if(archive == NULL) {
				fprintf(stderr, "Cannot open %s for reading.\n", archivename);
				return 1;
			}
			check_crc(archive);
			break;

		case DELETE:
			if(argc != 4) {
				fprintf(stderr, "Sorry, you can delete only one file at same time.\n");
				fprintf(stderr, "%s", help_message);
				return 0;
			}

			fd = mkstemp(tmp_name);
			if(fd == -1) {
				fprintf(stderr, "Cannot create tmp file.\n");
				return 1;
			}
			tmp = fdopen(fd, "w+b");

			archive = fopen(archivename, "rb");
			if(archive == NULL) {
				fprintf(stderr, "Cannot open %s for reading.\n", archivename);
				return 1;
			}

			switch(delete_from_archive(argv[3], archive, tmp)) {
				case -1:
					fprintf(stderr, "%s not found.\n", argv[3]);
					break;

				case 0:
					fprintf(stderr, "Successfully deleted %s.\n", argv[3]);
					break;
				case 1:
					fprintf(stderr, "Cannot delete %s from %s.\n", argv[3], archivename);
					remove(tmp_name);
					fclose(tmp);
					fclose(archive);
					return 1;
					break;
			}

			remove(archivename);
			rename(tmp_name, archivename);
			fclose(tmp);
			fclose(archive);
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
	if(!strcmp(argument, "-d")) {
		return DELETE;
	}
	return HELP;
}
