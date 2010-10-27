
#ifndef _PROTONMATH_H_
#define _PROTONMATH_H_

typedef struct {
	float x;
	float y;
	float z;
} Vector;

typedef struct {
	Vector o;
	Vector d;	
} Ray;

void setVector(Vector *v, float x, float y, float z);

Vector vsub(Vector v1, Vector v2);
float intersectSphere(Ray *ray, Vector pos, float size);
float vdot(Vector v1, Vector v2);
float vlen(Vector v);
void vadd(Vector *v1, Vector v2);
Vector vmulf(float val, Vector v);
float vnormalize(Vector *v);

#endif
