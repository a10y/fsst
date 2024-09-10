#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "fsst.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "must specify file as first argument.");
		exit(1);
	}

	// How large of a buffer to allocate.
	FILE *fp  = fopen(argv[1], "rb");
	if (!fp) {
		fprintf(stderr, "failed to load file");
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp) - 3;
	// The first 3 bytes are a block_size that their tool outputs
	fseek(fp, 3, SEEK_SET);

	unsigned char *buffer = (unsigned char*) malloc(file_size);

	// Return a buffer and a length to load stuff
	size_t read_size = fread(buffer, 1, file_size, fp);
	if (read_size != file_size) {
		fprintf(stderr, "invalid read_size returned from fread");
		exit(1);
	}

	fclose(fp);

	// Create a decoder to populate from the file.
	fsst_decoder_t decoder;

	if (fsst_import(&decoder, buffer) == 0) {
		fprintf(stderr, "error in fsst_import from file");
		exit(1);
	}

	// Print the contents of the decoder symbol table.
	fprintf(stdout, "version = %llu\n", decoder.version);
	// How to find the number of symbols in the table?
	
	for (size_t i = 0; i < 255; i++) {
		char len = decoder.len[i];
		unsigned long long symbol = decoder.symbol[i];
		if (len == 0) {
			continue;
		}


		fprintf(stdout, "symbol[%ld] = [", i);
		for (size_t k = 0; k < len; k++) {
			int character = (symbol >> 8*k) & 0xFF;
			if (character > 32 && character < 127) {
				putc(character, stdout);
			} else if (character == ' ') {
				fprintf(stdout, " ' ' ");
			} else if (character == '\n') {
				fprintf(stdout, " '\\n' ");
			} else if (character == '\t') {
				fprintf(stdout, " '\\t' ");
		        } else {
				fprintf(stdout, " 0x%x ", character);
			}
		}
		fprintf(stdout, "]\n");
	}
}
