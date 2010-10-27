
#ifndef _PROTONSCENE_H_
#define _PROTONSCENE_H_

#include "protonmath.h"
#include "protontypes.h"

#define TYPE_SPHERE 0
#define TYPE_BOX 1
#define TYPE_LIGHT 2
#define TYPE_CAMERA 3

typedef struct {
	ProtonColor diffuseColor;
	float reflectivity;
	
	float specPower;

} ProtonMaterial;

typedef struct {
	short type;
	float size;
	float brightness;

	Vector pos;
	ProtonMaterial material;

	float fov;

} ProtonObject;

typedef struct {
	
	float ambientLight;

	int numObjects;
	ProtonObject **objects;

	ProtonObject *camera;

} ProtonScene;

ProtonObject *createObject();
ProtonObject *createCamera();
void createMaterial(ProtonMaterial *material);
ProtonScene *protonCreateScene();
ProtonObject *protonAddSphere(ProtonScene *scene, float size);
ProtonObject *protonAddLight(ProtonScene *scene, float brightness, float size);
void protonAddObject(ProtonScene *scene, ProtonObject *object);

#endif
