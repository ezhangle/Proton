#include <stdio.h>
#include <math.h>
#include "protonmath.h"

void setVector(Vector *v, float x, float y, float z) {
	v->x = x;
	v->y = y;
	v->z = z;
}

void vadd(Vector *v1, Vector v2) {
	v1->x += v2.x;
	v1->y += v2.y;
	v1->z += v2.z;
}

Vector vsub(Vector v1, Vector v2) {
	Vector ret;
	ret.x = v1.x - v2.x;
	ret.y = v1.y - v2.y;
	ret.z = v1.z - v2.z;

	return ret;
}

Vector vmulv(Vector v1, Vector v2) {
	Vector ret;
	ret.x = v1.x * v2.x;
	ret.y = v1.y * v2.y;
	ret.z = v1.z * v2.z;
	return ret;
}

Vector vmulf(float val, Vector v) {
	Vector ret;
	ret.x = v.x * val;
	ret.y = v.y * val;
	ret.z = v.z * val;
	return ret;
}

float vdot(Vector v1, Vector v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

float vlen(Vector v) {
	return sqrtf( v.x * v.x + v.y * v.y + v.z * v.z );
}

float vnormalize(Vector *v) {
	float fLength = sqrtf( v->x * v->x + v->y * v->y + v->z * v->z );
	if(fLength > 1e-08) {
		float fInvLength = 1.0f / fLength;
		v->x *= fInvLength;
		v->y *= fInvLength;
		v->z *= fInvLength;
	}
}

float intersectSphere(Ray *ray, Vector pos, float size) {
	Vector dst = vsub(ray->o, pos);
	float B = vdot(dst, ray->d);
	float C = vdot(dst, dst) - (size*size);
	float D = B*B - C;
	if(D < 0.0f) {
		return -1;
	} else {
		return -B - sqrt(D);
	}
}
