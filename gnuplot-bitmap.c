#include "stb_image.h"

#include <stdio.h>

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr,
		    "usage: %s <infile> <outfile>\n"
		    "    infile:  image to render\n"
		    "    outfile: PDF file to save graph to\n",
		    argv[0]);
		return 1;
	}

	char *infile = argv[1], *outfile = argv[2];
	int x, y, n;
	// load image as grayscale (1 channel)
	unsigned char *data = stbi_load(infile, &x, &y, &n, 1);
	if (!data) {
		fprintf(stderr, "%s: failed to decode input '%s': %s\n", argv[0], infile, stbi_failure_reason());
		return 1;
	}

	printf("%d x %d, %d channels\n", x, y, n);
	printf("first pixel: %d\n", data[0]);
	printf("outfile: %s\n", outfile);

	return 0;
}
