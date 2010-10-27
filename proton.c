
#include "stdio.h"
#include "protoncore.h"

int main(int argc, char **argv) {
	printf("Proton raytracer v0.0\n");
	
	ProtonScene *scene = protonCreateScene();
	setVector(&scene->camera->pos, 0,-1,-12);

	ProtonObject *obj;

	scene->ambientLight = 0.2;

	int w=1280/2;
	int h=720/2;

	obj = protonAddSphere(scene, 1);
	obj->pos.y = 0;
	obj->pos.x = 0;
	obj->pos.z = -2;
	setColor(&obj->material.diffuseColor, 0.2, 0.2, 0.2, 0.5);
	obj->material.reflectivity = 1;
	obj->material.specPower = 40;
	
	obj = protonAddSphere(scene, 2);
	obj->pos.y = 1.5;
	obj->pos.x = 0;
	setColor(&obj->material.diffuseColor, 1, 0.4, 0.4, 0.5);
//	obj->material.reflectivity = 0.45;

	obj = protonAddSphere(scene, 2);
	obj->pos.y = -1.5;
	obj->pos.x = -1.5;
	setColor(&obj->material.diffuseColor, 0.4, 1, 0.4, 0.5);
//	obj->material.reflectivity = 0.45;

	obj = protonAddSphere(scene, 2);
	obj->pos.y = -1.5;
	obj->pos.x = 1.5;
	setColor(&obj->material.diffuseColor, 0.4, 0.4, 1, 0.5);
//	obj->material.reflectivity = 0.45;

	obj = protonAddLight(scene, 0.5, 200);
	obj->pos.y = -3;
	obj->pos.x = -2 ;
	obj->pos.z = -5;
	
	protonRender(scene, "out.bmp", w, h);

	return 1;	
}
