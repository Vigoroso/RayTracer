#pragma once

#include <stdio.h>
#include "glm/vec3.hpp"

// Output in P6 format, a binary file containing:
// P6
// ncolumns nrows
// Max colour value
// colours in binary format thus unreadable
void save_imageP6(int Width, int Height, char* fname, unsigned char* pixels) {
	FILE* fp;
	const int maxVal = 255;

	printf("Saving image %s: %d x %d\n", fname, Width, Height);
	fopen_s(&fp, fname, "wb");
	if (!fp) {
		printf("Unable to open file '%s'\n", fname);
		return;
	}
	fprintf(fp, "P6\n");
	fprintf(fp, "%d %d\n", Width, Height);
	fprintf(fp, "%d\n", maxVal);

	for (int j = 0; j < Height; j++) {
		fwrite(&pixels[j * Width * 3], 3, Width, fp);
	}

	fclose(fp);
}

// Output in P3 format, a text file containing:
// P3
// ncolumns nrows
// Max colour value (for us, and usually 255)
// r1 g1 b1 r2 g2 b2 .....
void save_imageP3(int Width, int Height, char* fname, vec3** pixels) {
	FILE* fp;
	const int maxVal = 255;

	printf("Saving image %s: %d x %d\n", fname, Width, Height);
	fopen_s(&fp, fname, "w");
	if (!fp) {
		printf("Unable to open file '%s'\n", fname);
		return;
	}
	fprintf(fp, "P3\n");
	fprintf(fp, "%d %d\n", Width, Height);
	fprintf(fp, "%d\n", maxVal);

	int k = 0;
	for (int j = 0; j < Height; j++) {

		for (int i = 0; i < Width; i++)
		{
			fprintf(fp, " %d %d %d", (int)(255.0f * pixels[i][j].r), (int)(255.0f * pixels[i][j].g), (int)(255.0f * pixels[i][j].b));
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}
