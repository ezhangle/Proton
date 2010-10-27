
#include "protonscene.h"

ProtonColor launchRay(ProtonScene *scene, Ray *ray, ProtonObject *bounceObject, int raydepth);
void protonRender(ProtonScene *scene, const char *fileName, int w, int h);
