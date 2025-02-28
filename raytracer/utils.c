#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <float.h>

void printVec3(vec3 vec) {
	printf("\tx: %f\n\ty: %f\n\tz: %f\n", vec.x, vec.y, vec.z);
}

vec3i sumi(vec3i vec1, vec3i vec2) {
	return (vec3i) { vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z };
}

vec3 sum(vec3 vec1, vec3 vec2) {
	return (vec3) { vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z };
}

color sumc(color color1, color color2) {
	return (color) { color1.r + color2.r, color1.g + color2.g, color1.b + color2.b };
}

vec3i diffi(vec3i vec1, vec3i vec2) {
	return (vec3i) { vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z };
}

vec3 diff(vec3 vec1, vec3 vec2) {
	return (vec3) { vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z };
}

color diffc(color color1, color color2) {
	return (color) { color1.r - color2.r, color1.g - color2.g, color1.b - color2.b };
}

vec3 scale(float s, vec3 vec) {
	return (vec3) { s*vec.x, s*vec.y, s*vec.z };
}

color scalec(float s, color c) {
	return (color) { (s*c.r > 1) ? 1 : s*c.r, (s*c.g > 1) ? 1: s*c.g, (s*c.b > 1) ? 1 : s*c.b };
}

vec3 cross(vec3 vec1, vec3 vec2) {
	vec3 res;

	res.x = vec1.y*vec2.z - vec1.z*vec2.y;
	res.y = vec1.z*vec2.x - vec1.x*vec2.z;
	res.z = vec1.x*vec2.y - vec1.y*vec2.x;
	
	return res;
}

vec3 normalize(vec3 vec) {
	float mag = NORM(vec);
	vec3 res;

	res.x = vec.x/mag;
	res.y = vec.y/mag;
	res.z = vec.z/mag;

	return res;
}

color clampc(color c) {
	c.r = (c.r > 1) ? 1 : c.r;
	c.g = (c.g > 1) ? 1 : c.g;
	c.b = (c.b > 1) ? 1 : c.b;

	return c;
}

ray pointsToRay(vec3 origin, vec3 point) {
	ray res;

	res.origin = origin;
	res.dir = normalize(diff(point, origin));

	return res;
}

vec3 surfaceNormal(vec3 int_point, struct object* obj) {
	switch (obj->type) {
		case SPHERE:
			return normalize(diff(int_point, obj->sph.center));
		case CYLINDER:
			return (vec3) { 0, 0, 0 };
	}
}

int genWindow(struct window_params* win, struct view_params* view) {
	vec3 n, u_prime, v_prime, u, v;
	float w, h;
	
	// Determine h and w for width and height of window in object space
	h = 2*tan((view->vfov*M_PI/180)/2);
	w = h*view->width/view->height;

	if (h == 0 || w == 0) {
		printf("Window width or height zero.\n");
		return -1;
	}

	// Determine unit vectors n, u, and v
	n = normalize(view->viewdir);
	u_prime = cross(view->viewdir, view->updir);
	u = normalize(u_prime);
	v_prime = cross(u, view->viewdir);
	v = normalize(v_prime);

	// Determine locations of the corners of the window
	win->ul = sum(diff(sum(view->eye, n), scale(w/2, u)), scale(h/2, v));
	win->ur = sum(sum(sum(view->eye, n), scale(w/2, u)), scale(h/2, v));
	win->ll = diff(diff(sum(view->eye, n), scale(w/2, u)), scale(h/2, v));
	win->lr = diff(sum(sum(view->eye, n), scale(w/2, u)), scale(h/2, v));

	// Determine vectors to traverse vertically and horizontally between pixels
	win->dh = scale(1.0/(view->width - 1), diff(win->ur, win->ul));
	win->dv = scale(1.0/(view->height - 1), diff(win->ll, win->ul));

	return 0;
}

float checkIntersect(ray* ray, struct object* obj) {
	float xd, yd, zd, x0, y0, z0, xc, yc, zc, r, b, c, disc, pos, neg;

	switch (obj->type) {
		case SPHERE:
			// Assign appropriate variables to reduce clutter in code
			xd = ray->dir.x;
			yd = ray->dir.y;
			zd = ray->dir.z;
			x0 = ray->origin.x;
			y0 = ray->origin.y;
			z0 = ray->origin.z;
			xc = obj->sph.center.x;
			yc = obj->sph.center.y;
			zc = obj->sph.center.z;
			r = obj->sph.radius;

			// Determine B and C from quadratic formula (A is 1 due to unit direction vector)
			b = 2*(xd*(x0 - xc) + yd*(y0 - yc) + zd*(z0 - zc));
			c = pow(x0 - xc, 2) + pow(y0 - yc, 2) + pow(z0 - zc, 2) - r*r;
			
			// Check if intersect using discriminant
			if ((disc = b*b - 4*c) < 0) {
				return 0;
			}

			// Record positive and negative values of t
			pos = (-b + sqrt(disc)) / 2;
			neg = (-b - sqrt(disc)) / 2;

			if (pos < 0) {
				return 0;
			} else if (neg < 0) {
				return pos;
			} else {
				return neg;
			}

		case CYLINDER:
			// Unfinished
			return -1;
	}
}

color traceRay(ray* ray, struct scene* scene) {
	struct object* cur_obj = NULL;
	float dist = 0, min_dist = FLT_MAX;

	// Find intersected object that is closest to eye (if any)
	for (int i = 0; i < scene->num_obj; i++) {
		if ((dist = checkIntersect(ray, scene->objects[i])) != 0) {
			if (dist < min_dist) {
				min_dist = dist;
				cur_obj = scene->objects[i];
			}
		}
	}

	// Return background color if no object is intersected
	if (cur_obj == NULL)
		return scene->bkgcolor;

	// Find intersection point
	vec3 int_point = sum(ray->origin, scale(min_dist, ray->dir));

	// Determine color of object at intersection point
	return shadeRay(ray, int_point, scene, cur_obj);
}

color shadeRay(ray* obj_ray, vec3 int_point, struct scene* scene, struct object* obj) {
	color I;
	vec3 V, N, L, H;
	float S = 1, f_att = 1;

	I = scalec(obj->color.ka, obj->color.Od);
	V = scale(-1, obj_ray->dir);

	switch (obj->type) {
		case SPHERE:
			N = normalize(diff(int_point, obj->sph.center));
			break;
		case CYLINDER:
			N = (vec3) { 0, 0, 0 };
	}
	
	for (int i = 0; i < scene->num_light; i++) {
		struct light* light = scene->lights[i];
		float light_dist = NORM(diff(light->origin, int_point));
		float dist;
		ray light_ray;

		switch (light->type) {
			case ATT_POINT:
				// Attenuation factor
				f_att = 1/(light->c1 + light->c2*light_dist + light->c3*light_dist*light_dist);
				if (f_att > 1) f_att = 1;
			case POINT:
				// Determine L vector and check if light source is blocked for a point source
				L = normalize(diff(light->origin, int_point));

				// Check for shadow
				for (int j = 0; j < scene->num_obj; j++) {
					if (obj == scene->objects[j]) continue;
					
					light_ray = pointsToRay(int_point, light->origin);
					dist = checkIntersect(&light_ray, scene->objects[j]);
					if (dist > 0 && dist <= light_dist) {
						S = 0;
						break;
					}
				}
				
				break;
			case DIRECTIONAL:
				// Determine L vector and check if light source is blocked for a directional source
				L = normalize(scale(-1, light->dir));

				// Check for shadow
				for (int j = 0; j < scene->num_obj; j++) {
					if (obj == scene->objects[j]) continue;
					
					light_ray = (ray) { int_point, normalize(light->dir) };
					if (checkIntersect(&light_ray, scene->objects[j]) > 0) {
						S = 0;
						break;
					}
				}
		}

		// Apply Blinn-Phong illumination equation
		H = normalize(sum(L, V));
		
		float IL, NdotL, NdotH, kd, ks, n;
		color Od, Os;
		IL = light->intensity;
		NdotL = DOT(N, L) > 0 ? DOT(N, L) : 0;
		NdotH = DOT(N, H) > 0 ? DOT(N, H) : 0;
		kd = obj->color.kd;
		ks = obj->color.ks;
		n = obj->color.n;
		Od = obj->color.Od;
		Os = obj->color.Os;

		I = sumc(I, scalec(S, scalec(f_att, scalec(IL, 
							sumc(scalec(kd, scalec(NdotL, Od)), scalec(ks, scalec(pow(NdotH, n), Os)))))));
	}

	return clampc(I);
}
