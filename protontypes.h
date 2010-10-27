
#ifndef _PROTONTYPES_H_
#define _PROTONTYPES_H_

typedef struct {
	float r;
	float g;
	float b;
	float a;
} ProtonColor;

void setColor(ProtonColor *color, float r, float g, float b, float a);
void colorClamp(ProtonColor *color);

#endif

