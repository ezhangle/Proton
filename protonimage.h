
#ifndef _PROTON_IMAGE_H_
#define _PROTON_IMAGE_H_

#include "protontypes.h"

#define FORMAT_HDR 0
#define FORMAT_RGBA 2

typedef struct {
	char *data;
	int dataSize;
	
	int width;
	int height;

	float exposure;
	int dataFormat;
} ProtonImage;

typedef struct {
	int size;
	char **tokens;
} TokenArray;	

TokenArray readTokens(char *line, const char *tokens);
void freeTokens(TokenArray tokens);

ProtonImage *loadImage_HDR(const char *fileName);
ProtonColor getImagePixel(ProtonImage *image, int x, int y);
void destroyImage(ProtonImage *image);

ProtonImage *createImage(int width, int height, int format);

ProtonImage *gaussianBlurImage(ProtonImage *image, float radius, float deviation);

#endif
