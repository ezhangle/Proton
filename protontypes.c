
#include "protontypes.h"

void setColor(ProtonColor *color, float r, float g, float b, float a) {
	color->r = r;
	color->g = g;
	color->b = b;
	color->a = a;
}

void colorClamp(ProtonColor *color) {
	if(color->r > 1)
		color->r = 1;
	if(color->g > 1)
		color->g = 1;
	if(color->b > 1)
		color->b = 1;
}
