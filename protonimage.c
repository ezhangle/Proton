
#include "protonimage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rgbe.h"

TokenArray readTokens(char *line, const char *tokenString) {
	char **tokens = malloc(sizeof(void*));
	char *pch;
	int numTokens = 0; 
	pch = strtok (line, tokenString);
	while (pch != NULL) {
		numTokens++;
		tokens = realloc(tokens, sizeof(void*) *numTokens);
		tokens[numTokens-1] = (char*) malloc(strlen(pch)+1);
		memcpy(tokens[numTokens-1], pch, strlen(pch)+1);
		pch = strtok (NULL, tokenString);
	}

	TokenArray ta;
	ta.size = numTokens;
	ta.tokens = tokens;
	return ta;
}

void freeTokens(TokenArray tokens) {
	int i;
	for(i =0; i < tokens.size; i++) {
		free(tokens.tokens[i]);
	}
	free(tokens.tokens);
}

ProtonImage *loadImage_HDR(const char *fileName) {
	
	FILE *file = fopen (fileName, "rb");
	if(!file)
		return NULL;
	
	char line [ 128 ];

	fgets ( line, sizeof(line), file );
	if(strcmp(line, "#?RADIANCE\n") != 0) {
		printf("Invalid HDR image\n");
		fclose(file);
		return NULL;
	}

	ProtonImage *image = (ProtonImage*) malloc(sizeof(ProtonImage));
	image->dataFormat = FORMAT_HDR;
	image->exposure = 1;


 	while ( fgets ( line, sizeof(line), file ) != NULL ) {
		TokenArray ta = readTokens(line, "=");
		if(strcmp(ta.tokens[0], "EXPOSURE") == 0) {
			image->exposure = atof(ta.tokens[1]);
		}
		if(strcmp(ta.tokens[0], "FORMAT") == 0) {
			//printf("format is %s\n", ta.tokens[1]);
		}
		freeTokens(ta);

		ta = readTokens(line, " ");
		if(strcmp(ta.tokens[0], "-Y") == 0) {
			//printf("image size is %d x %d\n", atoi(ta.tokens[1]), atoi(ta.tokens[3]));
			image->width = atoi(ta.tokens[1]);
			image->height = atoi(ta.tokens[3]);
			freeTokens(ta);
			break;
		}
		freeTokens(ta);
	}
	
	image->data = (char *)malloc(sizeof(float)*3*image->width*image->height);
	RGBE_ReadPixels_RLE(file,(float*)image->data,image->width,image->height);

	fclose(file);
	return image;
}

ProtonColor getImagePixel(ProtonImage *image, int x, int y) {
	ProtonColor color;
	if(image->dataFormat == FORMAT_HDR) {
		float *fdata = (float*) image->data;
		color.r = fdata[(image->width * 3 * y) + (3 * x)] * image->exposure;
		color.g = fdata[(image->width * 3 * y) + (3 * x) + 1] * image->exposure;
		color.b = fdata[(image->width * 3 * y) + (3 * x) + 2] * image->exposure;
	}
	return color;
}

void destroyImage(ProtonImage *image) {
	free(image);
}
