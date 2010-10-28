#include "protontypes.h"

struct BMPHeader {
     char id[2];
     int filesize;
     short reserved[2];
     int headersize;
} __attribute__((packed));

struct BMPInfo {
     int infoSize;
     int width;
     int depth;
     short biPlanes;
     short bits;
     int biCompression;
     int biSizeImage;
     int biXPelsPerMeter;
     int biYPelsPerMeter;
     int biClrUsed;
     int biClrImportant;
} __attribute__((packed));

typedef struct {
	short width;
	short height;
	char *data;
} Bitmap;

Bitmap createBitmap(short width, short height);
void destroyBitmap(Bitmap bitmap);
int writeBitmap(Bitmap bitmap, const char *fileName);
void setPixelRGB(Bitmap bitmap, short x, short y, unsigned char r, unsigned char g, unsigned char b);
void setPixel(Bitmap bitmap, short x, short y, ProtonColor color);
void setPixelRGBf(Bitmap bitmap, short x, short y, float r, float g, float b);
