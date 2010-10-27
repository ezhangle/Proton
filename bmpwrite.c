#include "bmpwrite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Bitmap createBitmap(short width, short height) {
	Bitmap bitmap;
	bitmap.width = width;
	bitmap.height = height;
	bitmap.data = malloc(width*height*3);
	memset(bitmap.data, 0, width*height*3);
	return bitmap;
}

void destroyBitmap(Bitmap bitmap) {
	free(bitmap.data);
}

int writeBitmap(Bitmap bitmap, const char *fileName) {
	printf("Writing bitmap to %s\n", fileName);

	FILE * bmpFile;

	bmpFile = fopen(fileName, "wb");
	if(!bmpFile)  {
		printf("Error opening file\n");
		return 0;
	}
	
	struct BMPHeader bh;

	memset (&bh,0,sizeof(struct BMPHeader));
	bh.headersize  = sizeof(struct BMPHeader) + sizeof(struct BMPInfo);
	bh.id[0] = 'B';
	bh.id[1] = 'M';

	struct BMPInfo bi;
	memset (&bi,0,sizeof(struct BMPInfo));

	int bytesPerLine;

	bytesPerLine = bitmap.width * 3;
	if (bytesPerLine & 0x0003) {
		bytesPerLine |= 0x0003;
		++bytesPerLine;
	}

	bi.biSizeImage = (int)(bytesPerLine*bitmap.height);
	bh.filesize = (int)bh.headersize+bi.biSizeImage;

	bi.infoSize  =  0x28L;
	bi.width     = (int)bitmap.width;
	bi.depth     = (int)bitmap.height;
	bi.biPlanes  =  1;
	bi.bits      = 24;
	bi.biCompression = 0L;

	fwrite(&bh, 1, sizeof(bh), bmpFile);
	fwrite(&bi, sizeof(bi), 1, bmpFile);

	char *linebuf = (char*) malloc(bytesPerLine);
	memset(linebuf, 1, bytesPerLine);
	int line;
	for (line = bitmap.height; line >= 0; line --) {
		memcpy(linebuf, bitmap.data + ((line-1)*(bitmap.width*3)), bitmap.width*3), 
		fwrite(linebuf, 1, bytesPerLine, bmpFile);
	}
	free(linebuf);

	fclose(bmpFile);	

	return 1;	
}

void setPixel(Bitmap bitmap, short x, short y, ProtonColor color) {
	setPixelRGB(bitmap, x, y, (unsigned char)(255.0f*color.r),(unsigned char) (255.0f*color.g), (unsigned char)( 255.0f*color.b));
}

void setPixelRGB(Bitmap bitmap, short x, short y, unsigned char r, unsigned char g, unsigned char b) {
	bitmap.data[(y*(bitmap.width*3)) + (x*3) + 0] = b;
	bitmap.data[(y*(bitmap.width*3)) + (x*3) + 1] = g;
	bitmap.data[(y*(bitmap.width*3)) + (x*3) + 2] = r;
}
