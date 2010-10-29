
#include "protonimage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rgbe.h"
#include <math.h>

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
		color.r = fdata[(image->width * 3 * y) + (3 * x)];
		color.g = fdata[(image->width * 3 * y) + (3 * x) + 1];
		color.b = fdata[(image->width * 3 * y) + (3 * x) + 2];
	}
	return color;
}

void destroyImage(ProtonImage *image) {
	free(image->data);
	free(image);
}


ProtonImage *createImage(int width, int height, int format) {
	ProtonImage *newImage = (ProtonImage*) malloc(sizeof(ProtonImage));
	newImage->dataFormat = format;
	newImage->width = width;
	newImage->height = height;

	switch(format) {
		case FORMAT_HDR:
			newImage->data = (char *)malloc(sizeof(float)*3*width*height);
		break;
		case FORMAT_RGBA:
			newImage->data = (char *)malloc(4*width*height);
		break;
	}

	return newImage;
}

float* createKernel(float radius, float deviation) {
	int size = 2 * (int)radius + 1;
        float* kernel = malloc(sizeof(float) * (size+1));
        float radiusf = fabs(radius) + 1.0f;

        if(deviation == 0.0f) deviation = sqrtf(
                -(radiusf * radiusf) / (2.0f * logf(1.0f / 255.0f))
        );

        kernel[0] = size;

        float value = -radius;
        float sum   = 0.0f;
	int i;

        for(i = 0; i < size; i++) {
                kernel[1 + i] =
                        1.0f / (2.506628275f * deviation) *
                        expf(-((value * value) / (2.0f * (deviation * deviation))))
                ;

                sum   += kernel[1 + i];
                value += 1.0f;
        }

        for(i = 0; i < size; i++) kernel[1 + i] /= sum;

        return kernel;
}

ProtonImage *gaussianBlurImage(ProtonImage *image, float radius, float deviation) {
	ProtonImage *newImage = createImage(image->width, image->height, image->dataFormat);
	
	
	float *horzBlur;
	float *vertBlur;
	
	horzBlur = (float *)malloc(sizeof(float)*3*newImage->width*newImage->height);
	vertBlur = (float *)malloc(sizeof(float)*3*newImage->width*newImage->height);

	float *kernel = createKernel(radius, deviation);
	
	int i, iY, iX;

	float *fdata = (float*) image->data;

	// the following is for HDR only!
	// TODO: write RGBA version

        // Horizontal pass.
        for(iY = 0; iY < image->height; iY++) {
                for(iX = 0; iX < image->width; iX++) {
                        float red   = 0.0f;
                        float green = 0.0f;
                        float blue  = 0.0f;

                        int offset = ((int)kernel[0]) / -2;

                        for(i = 0; i < ((int)kernel[0]); i++) {
                                int x = iX + offset;

                                if(x < 0 || x >= image->width) { offset++; continue; }

				float *dataPtr = &fdata[(image->width * 3 * iY) + (3 * x)];

                                float kernip1 = kernel[i + 1];

                                        red   += kernip1 * dataPtr[0];
                                        green += kernip1 * dataPtr[1];
                                        blue  += kernip1 * dataPtr[2];

                                offset++;
                        }

                        int baseOffset = (image->width * 3 * iY) + (3 * iX);

                                horzBlur[baseOffset] = red;
                                horzBlur[baseOffset + 1] = green;
                                horzBlur[baseOffset + 2]     = blue;
                }
        }

	// Vertical pass.
        for(iY = 0; iY < image->height; iY++) {
                for(iX = 0; iX < image->width; iX++) {
                        float red   = 0.0f;
                        float green = 0.0f;
                        float blue  = 0.0f;

                        int offset = ((int)kernel[0]) / -2;

                        for(i = 0; i < ((int)kernel[0]); i++) {
                                int y = iY + offset;

                                if(y < 0 || y >= image->height) {
                                        offset++;
                                        continue;
                                }

				float *dataPtr = &horzBlur[(image->width * 3 * y) + (3 * iX)];

                                float kernip1 = kernel[i + 1];

                                        red   += kernip1 * dataPtr[0];
                                        green += kernip1 * dataPtr[1];
                                        blue  += kernip1 * dataPtr[2];

                                offset++;
                        }

                        int baseOffset = (image->width * 3 * iY) + (3 * iX);

                                vertBlur[baseOffset ] = red;
                                vertBlur[baseOffset + 1] = green;
                                vertBlur[baseOffset+ 2]     = blue;
                }
        }


	float *nfdata = (float*)newImage->data;

        for(i = 0; i < image->height * image->width * 3; i++) {
		nfdata[i] = vertBlur[i];
        }

	free(horzBlur);
	free(vertBlur);
	free(kernel);

	return newImage;
}

