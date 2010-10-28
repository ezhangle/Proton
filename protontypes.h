
#ifndef _PROTONTYPES_H_
#define _PROTONTYPES_H_

#define ONE_OVER_PI 0.31830988618 

typedef struct {
	float r;
	float g;
	float b;
	float a;
} ProtonColor;

void setColorFromColor(ProtonColor *color, ProtonColor color2);
void setColor(ProtonColor *color, float r, float g, float b, float a);
void colorClamp(ProtonColor *color);

#endif

