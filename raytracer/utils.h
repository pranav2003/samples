#pragma once

#include <math.h>

/** Integer vector **/
typedef struct {
	int x, y, z;
} vec3i;

/** Float vector **/
typedef struct {
	float x, y, z;
} vec3;

/** Float ray **/
typedef struct {
	vec3 origin, dir;
} ray;

/** Dot product **/
#define DOT(vec1, vec2) vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z

/** Euclidean norm (magnitude) **/
#define NORM(vec) sqrt((double) (vec.x*vec.x + vec.y*vec.y + vec.z*vec.z))

enum light_type { ATT_POINT, POINT, DIRECTIONAL };

/** Light source **/
struct light {
	enum light_type type;
	float intensity;
	float c1, c2, c3;
	union {
		vec3 origin;
		vec3 dir;
	};
};

enum object_type { SPHERE, CYLINDER };

struct sphere {
	vec3 center;
	float radius;
};

struct cylinder {
	vec3 center;
	vec3 dir;
	float radius;
	float length;
};

typedef struct {
	float r, g, b;
} color;

struct mtl_color {
	color Od;
	color Os;
	float ka, kd, ks, n;
};

struct object {
	enum object_type type;
	struct mtl_color color;
	union {
		struct sphere sph;
		struct cylinder cyl;
	};
};

struct view_params {
	vec3 eye;
	vec3 viewdir;
	vec3 updir;
	float vfov;
	int width;
	int height;
};

struct window_params {
	vec3 ul, ur, ll, lr, dh, dv;
};

#define MAX_OBJECTS 512
#define MAX_LIGHTS 512

struct scene {
	color bkgcolor;
	int num_obj;
	int num_light;
	struct object* objects[MAX_OBJECTS];
	struct light* lights[MAX_LIGHTS];
};

void printVec3(vec3);

/** Vector addition **/
vec3i sumi(vec3i vec1, vec3i vec2);
vec3 sum(vec3 vec1, vec3 vec2);
color sumc(color color1, color color2);

/** Vector subtraction **/
vec3i diffi(vec3i vec1, vec3i vec2);
vec3 diff(vec3 vec1, vec3 vec2);
color diffc(color color1, color color2);

/** Vector scaling **/
vec3 scale(float s, vec3 vec);
color scalec(float s, color c);

/** Cross products **/
vec3 cross(vec3 vec1, vec3 vec2);

/** Vector normalization **/
vec3 normalize(vec3 vec1);

/** Color clamping **/
color clampc(color c);

/** Convert points to ray **/
ray pointsToRay(vec3 origin, vec3 point);

/** Calculate surface normal **/
vec3 surfaceNormal(vec3 int_point, struct object* obj);

/** Check if ray intersects object **/
float checkIntersect(ray* ray, struct object* obj);
/** Trace ray checking all objects for intersection **/
color traceRay(ray* ray, struct scene* scene);

/** Shade ray with correct color **/
color shadeRay(ray* ray, vec3 int_point, struct scene* scene, struct object* obj);

/** Generate viewing window **/
int genWindow(struct window_params* win, struct view_params* view);
