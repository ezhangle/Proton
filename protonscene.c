
#include <stdio.h>
#include <stdlib.h>
#include "protonscene.h"


void createMaterial(ProtonMaterial *material) {
	material->reflectivity = 0;
	material->specPower = 0;
	setColor(&material->diffuseColor, 0.5, 0.5, 0.5, 1);
}

ProtonObject *createCamera() {
	ProtonObject *camera = createObject();
	camera->type = TYPE_CAMERA;
	camera->fov = 45.0f;
}

void protonDestroyScene(ProtonScene *scene) {

	if(scene->skyImage)
		destroyImage(scene->skyImage);

	free(scene);
}

int protonLoadSkyImage(ProtonScene *scene, const char *imageFile) {
	ProtonImage*image = loadImage_HDR(imageFile);
	if(!image)
		return 0;

	scene->skyImage = image;

	return 1;
}


ProtonScene *protonCreateScene() {
	ProtonScene *newScene = (ProtonScene*) malloc(sizeof(ProtonScene));
	newScene->camera = createCamera();
	newScene->numObjects = 0;
	newScene->ambientLight = 0;

	newScene->skyImage = NULL;

	return newScene;
}

ProtonObject *createObject() {
	ProtonObject *newObject = (ProtonObject*) malloc(sizeof(ProtonObject));
	newObject->pos.x = 0;
	newObject->pos.y = 0;
	newObject->pos.z = 0;
	return newObject;
}

ProtonObject *protonAddSphere(ProtonScene *scene, float size) {
	ProtonObject *sphere = (ProtonObject*) malloc(sizeof(ProtonObject));
	sphere->type = TYPE_SPHERE;
	sphere->size = size;
	createMaterial(&sphere->material);
	protonAddObject(scene, sphere);
	return sphere;
}

ProtonObject *protonAddLight(ProtonScene *scene, float brightness, float size) {
	ProtonObject *light = (ProtonObject*) malloc(sizeof(ProtonObject));
	light->type = TYPE_LIGHT;
	light->size = size;
	light->brightness = brightness;
	protonAddObject(scene, light);
	return light;

}

void protonAddObject(ProtonScene *scene, ProtonObject *object) {
	scene->numObjects++;
	scene->objects = (ProtonObject**) realloc(scene->objects, sizeof(ProtonObject*) * scene->numObjects);
	scene->objects[scene->numObjects-1] = object;
}
