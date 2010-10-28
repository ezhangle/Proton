
#include "protonrender.h"
#include "protontypes.h"
#include "protonimage.h"
#include <stdio.h>
#include <stdlib.h>
#include "bmpwrite.h"
#include "protonmath.h"
#include <time.h>
#include <math.h>

float intersectObject(Ray *ray, ProtonObject *object) {
	switch(object->type) {
		case TYPE_SPHERE:
			return intersectSphere(ray, object->pos, object->size);
		break;
		default:
		break;
	}
	return -1;
}

ProtonObject *canSee(ProtonScene *scene, Vector eye, Vector dest) {

	Ray ray;
	ray.o = eye;
	ray.d = vsub(dest, eye); 

	vnormalize(&ray.d);
	
	float lastDist = 9999999999;

	int objn;
	ProtonObject *closest = NULL;
	for(objn =0; objn < scene->numObjects; objn++) {
		ProtonObject *object = scene->objects[objn];
		float dist = intersectObject(&ray, object);
		if(dist != -1) {
			if(dist < lastDist) {
				closest = object;
				lastDist = dist;
			}
		}
	}
	return closest;
}

void phongSpecular(ProtonColor *color, float specPower, Ray *ray, ProtonObject *light, Vector iPos, Vector nor, Vector lnor) {

	
	Vector R = vsub(nor, vmulf(2.0f * vdot( lnor,nor ), nor));
	float dot = vdot( ray->d, R );

	if(dot > 0) { 
		float val = powf( dot,10) * specPower;
	
		color->r += val;
		color->g += val;
		color->b += val;


	}
}

void diffuseShade(ProtonColor *color, Ray *ray, ProtonObject *light, Vector iPos, Vector nor, Vector lnor) {
		float val = vdot(lnor, nor);
		if(val < 0) 
			val = 0;
		val = val * light->brightness;
		color->r += color->r * val;
		color->g += color->g * val;
		color->b += color->b * val;
}

float applyLighting(ProtonColor *color, ProtonScene *scene, Ray *ray, ProtonObject *object, ProtonObject *light) {
	float final = 0;

	float dist = intersectObject(ray, object);
	Vector iPos = {ray->o.x + (ray->d.x*dist), ray->o.y+ (ray->d.y*dist), ray->o.z + (ray->d.z*dist)};
	ProtonObject *seeObj = canSee(scene, light->pos, iPos);

	if(seeObj == object || seeObj == NULL) {
		Vector nor = vsub(iPos, object->pos);
		Vector lnor = vsub(light->pos, iPos);

		vnormalize(&nor);
		vnormalize(&lnor);

		diffuseShade(color, ray, light, iPos, nor, lnor);
		phongSpecular(color, object->material.specPower, ray, light, iPos, nor, lnor);

	} else {
		return 0;
	}

	return 1;
}

ProtonColor getImageColorFromRay(ProtonImage *image, Ray *ray) {
	ProtonColor color;

	float invl = 1.0f / (0.00000001 + sqrtf(ray->d.x * ray->d.x + ray->d.y * ray->d.y));
        double r = ONE_OVER_PI * acosf(ray->d.z) * invl;
        double u = ray->d.x * r;
        double v = ray->d.y * r;

        /* Calculate pixel's coordinates in the image */
        int x = ((int) (u * image->width + image->height)) >> 1;
        int y = ((int) (v * image->height + image->width)) >> 1;

	return getImagePixel(image, x, y);
}

ProtonColor launchRay(ProtonScene *scene, Ray *ray, ProtonObject *bounceObject, int raydepth) {
	float lastDist = 99999999;
	ProtonColor finalColor;
	float ambInt = scene->ambientLight;
	
	if(scene->skyImage) {
		scene->skyImage->exposure = ambInt;
		setColorFromColor(&finalColor, getImageColorFromRay(scene->skyImage, ray));
	} else {
		setColor(&finalColor, ambInt, ambInt, ambInt, 0);
	}

	int objectHit = 0;			
	int objn, lobjn;
	int refn;
	int ambn;

	float rayDist;

	for(objn =0; objn < scene->numObjects; objn++) {
		ProtonObject *object = scene->objects[objn];
		if(object != bounceObject) {
		float dist = intersectObject(ray, object);
		if(dist > -1) {
			if(dist < lastDist) {
				float amb = scene->ambientLight;
	//			setColor(&finalColor,  object->material.diffuseColor.r*amb, object->material.diffuseColor.g*amb, object->material.diffuseColor.b*amb, 0);

				Vector iPos = {ray->o.x + (ray->d.x*dist), ray->o.y+ (ray->d.y*dist), ray->o.z + (ray->d.z*dist)};
				Vector nor = vsub(iPos, object->pos);
				vnormalize(&nor);
			
				Vector R = vsub(ray->d, vmulf(2.0f * vdot( ray->d,nor ), nor));
				Ray refray = {iPos, R};			

				// skylighting
				int i;
				ProtonColor lightColor;
				setColor(&lightColor,0,0,0,0);
				Ray ambRay = refray;
						
				float ambQuality = 1000;

				for(i=0; i < ambQuality; i++) {
					ambRay.d.x = refray.d.x-1+(((float)rand()/(float)RAND_MAX) * 2);
					ambRay.d.y = refray.d.y-1+(((float)rand()/(float)RAND_MAX) * 2);
					ambRay.d.z = refray.d.z-1+(((float)rand()/(float)RAND_MAX) * 2);
					vnormalize(&ambRay.d);	

					float didHit = 0;
					for(ambn =0; ambn < scene->numObjects; ambn++) {
						ProtonObject *ambObject = scene->objects[ambn];
						float ambdist = intersectObject(&ambRay, ambObject);
						if(ambdist > 0 && ambObject != object && ambObject != bounceObject) {
							didHit = 1;
							break;
						}
					}
					if(!didHit) {
						if(scene->skyImage) {
							ProtonColor skyColor = getImageColorFromRay(scene->skyImage, &ambRay);
							lightColor.r += skyColor.r/ambQuality;
							lightColor.g += skyColor.g/ambQuality;
							lightColor.b += skyColor.b/ambQuality;
						} else {
							lightColor.r += ambInt/ambQuality;
							lightColor.g += ambInt/ambQuality;
							lightColor.b += ambInt/ambQuality;
						}
					}

				}


				if(lightColor.r < 0)
					lightColor.r = 0;
				if(lightColor.g < 0)
					lightColor.g = 0;
				if(lightColor.b < 0)
					lightColor.b = 0;

				finalColor.r = object->material.diffuseColor.r * lightColor.r;
				finalColor.g = object->material.diffuseColor.g * lightColor.g;
				finalColor.b = object->material.diffuseColor.b * lightColor.b;

				if(raydepth < 2 && object->material.reflectivity > 0) {
					ProtonColor refColor = launchRay(scene, &refray, object, raydepth+1);	
					float refVal = object->material.reflectivity;
					finalColor.r = (finalColor.r * (1-refVal)) + (refColor.r * refVal);
					finalColor.g = (finalColor.g * (1-refVal)) + (refColor.g * refVal);
					finalColor.b = (finalColor.b * (1-refVal)) + (refColor.b * refVal);
				}


				for(lobjn = 0; lobjn < scene->numObjects; lobjn++) {
					ProtonObject *light = scene->objects[lobjn];
					if(light->type == TYPE_LIGHT) {
						applyLighting(&finalColor, scene, ray, object, light);
					}
							
				}



				lastDist = dist;
			}
		}
		}
	}		
	colorClamp(&finalColor);
	return finalColor;
}

void protonRender(ProtonScene *scene, const char *fileName, int w, int h) {
	printf("Rendering scene...\n");
	
	int aaSamples = 3;
	float aaWidth = 2.0f;

	float noiseAmt = 0.05;	
	int doNoise = 0;
	int colorNoise = 1;
	
	int partialRender = 0;
	float partialNoise = 0.2;

	int scanLineRender = 0;

	float aaStep = aaWidth/((float)aaSamples);
	float aaCoeff = 1.0f/((float)(aaSamples*aaSamples));

	Bitmap bitmap = createBitmap(w,h);

	float startTime = clock();

	int x,y,objn, lobjn, aaxc, aayc;
	float aax,aay;

	ProtonColor lastColor;

	float screenSizeY = 1;
	float screenSizeX = 1*((float)w/(float)h);

//#pragma omp parallel for schedule(dynamic, 1) private(x)
		for(y=0; y < bitmap.height; y++) {
		if(!scanLineRender || (scanLineRender && y % 2 == 0)) {
	for(x =0 ; x < bitmap.width; x++) {

			ProtonColor endColor;
			setColor(&endColor, 0, 0, 0, 0);
		
			aax = x -(aaWidth/2.0f);
			for(aaxc = 0; aaxc < aaSamples; aaxc ++) {
				aay = y - (aaWidth/2.0f);
				for(aayc = 0; aayc < aaSamples; aayc ++) {
					float camDist = 1.3;
					float xcell = (float)bitmap.width;
					float ycell = (float)bitmap.height;

					Vector screenPoint = {scene->camera->pos.x-(screenSizeX/2.0f) + ((screenSizeX/xcell) * aax),
								scene->camera->pos.y-(screenSizeY/2.0f) + ((screenSizeY/ycell) * aay),
								scene->camera->pos.z+camDist};
					Vector rayVec = vsub(screenPoint, scene->camera->pos);
					vnormalize(&rayVec);
					Ray ray = {scene->camera->pos, rayVec};

					ProtonColor col;
					if(partialRender) {
						if((float)rand()/(float)RAND_MAX > partialNoise)  {
							col = launchRay(scene, &ray, NULL, 0);
						} else {
						//	setColor(&col, 0, 0, 0, 0);
						}
						lastColor = col;
					} else {
						col = launchRay(scene, &ray, NULL, 0);
					}

					if(noiseAmt > 0 && doNoise) {
						ProtonColor noiseColor;
						if(!colorNoise) {
							float noiseCol = (float)rand()/(float)RAND_MAX;
							setColor(&noiseColor, noiseCol, noiseCol, noiseCol, 1);
						} else {
							setColor(&noiseColor, (float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,0);
						}

						col.r = (col.r*(1-noiseAmt)) + (noiseColor.r * noiseAmt);
						col.g = (col.g*(1-noiseAmt)) + (noiseColor.g * noiseAmt);
						col.b = (col.b*(1-noiseAmt)) + (noiseColor.b * noiseAmt);
					}

					endColor.r += col.r * aaCoeff;
					endColor.g += col.g * aaCoeff;
					endColor.b += col.b * aaCoeff;
					aay += aaStep;
				}
				aax += aaStep;
			}


			setPixel(bitmap, x, y, endColor);
		}
		}
	}	


	float endTime = clock();
	printf("Done. Render time %f msecs\n", ((endTime-startTime)/CLOCKS_PER_SEC) * 1000);

	writeBitmap(bitmap, fileName);
	destroyBitmap(bitmap);
}	
