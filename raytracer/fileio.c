#include "fileio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parseInput(struct view_params* view, struct scene* scene, char* input_fn) {
	FILE* input = fopen(input_fn, "r");
	if (input == NULL) {
		printf("Error opening file. Ensure the file exists.\n");
		return -1;
	}
	
	char line[64];
	char* token;

	int ind_obj = 0, ind_light = 0;
	struct mtl_color cur_color = { 0, 0, 0 };
	
	// Parse all lines in input file using keywords
	while (fgets(line, 64, input) != NULL) {
		token = strtok(line, " \n");
		if (token == NULL || strcmp(token, "//") == 0) {   // Comment or empty line
			continue;
		} else if (strcmp(token, "eye") == 0) {   // Eye position
			token = strtok(NULL, " \n");
			view->eye.x = (float) atof(token);
			token = strtok(NULL, " \n");
			view->eye.y = (float) atof(token);
			token = strtok(NULL, " \n");
			view->eye.z = (float) atof(token);
		} else if (strcmp(token, "viewdir") == 0) {   // Viewing direction
			token = strtok(NULL, " \n");
			view->viewdir.x = (float) atof(token);
			token = strtok(NULL, " \n");
			view->viewdir.y = (float) atof(token);
			token = strtok(NULL, " \n");
			view->viewdir.z = (float) atof(token);
		} else if (strcmp(token, "updir") == 0) {   // Up direction
			token = strtok(NULL, " \n");
			view->updir.x = (float) atof(token);
			token = strtok(NULL, " \n");
			view->updir.y = (float) atof(token);
			token = strtok(NULL, " \n");
			view->updir.z = (float) atof(token);
		} else if (strcmp(token, "vfov") == 0) {   // Vertical field of view
			token = strtok(NULL, " \n");
			view->vfov = (float) atof(token);
		} else if (strcmp(token, "imsize") == 0) {   // Image resolution
			token = strtok(NULL, " \n");
			view->width = (float) atoi(token);
			token = strtok(NULL, " \n");
			view->height = (float) atoi(token);
		} else if (strcmp(token, "bkgcolor") == 0) {   // Background color
			token = strtok(NULL, " \n");
			scene->bkgcolor.r = (float) atof(token);
			token = strtok(NULL, " \n");
			scene->bkgcolor.g = (float) atof(token);
			token = strtok(NULL, " \n");
			scene->bkgcolor.b = (float) atof(token);
		} else if (strcmp(token, "mtlcolor") == 0) {   // Material color
			token = strtok(NULL, " \n");
			cur_color.Od.r = (float) atof(token);
			token = strtok(NULL, " \n");
			cur_color.Od.g = (float) atof(token);
			token = strtok(NULL, " \n");
			cur_color.Od.b = (float) atof(token);
			token = strtok(NULL, " \n");
			cur_color.Os.r = (float) atof(token);
			token = strtok(NULL, " \n");
			cur_color.Os.g = (float) atof(token);
			token = strtok(NULL, " \n");
			cur_color.Os.b = (float) atof(token);
			token = strtok(NULL, " \n");
			cur_color.ka = (float) atof(token);
			token = strtok(NULL, " \n");
			cur_color.kd = (float) atof(token);
			token = strtok(NULL, " \n");
			cur_color.ks = (float) atof(token);
			token = strtok(NULL, " \n");
			cur_color.n = (float) atof(token);
		} else if (strcmp(token, "sphere") == 0) {   // Sphere object
			struct object* obj = malloc(sizeof(struct object));
			obj->type = SPHERE;
			token = strtok(NULL, " \n");
			obj->sph.center.x = atof(token);
			token = strtok(NULL, " \n");
			obj->sph.center.y = atof(token);
			token = strtok(NULL, " \n");
			obj->sph.center.z = atof(token);
			token = strtok(NULL, " \n");
			obj->sph.radius = atof(token);

			obj->color = cur_color;

			scene->objects[ind_obj++] = obj;
		} else if (strcmp(token, "cylinder") == 0) {   // Cylinder object
			struct object* obj = malloc(sizeof(struct object));
			obj->type = CYLINDER;
			token = strtok(NULL, " \n");
			obj->cyl.center.x = atof(token);
			token = strtok(NULL, " \n");
			obj->cyl.center.y = atof(token);
			token = strtok(NULL, " \n");
			obj->cyl.center.z = atof(token);
			token = strtok(NULL, " \n");
			obj->cyl.dir.x = atof(token);
			token = strtok(NULL, " \n");
			obj->cyl.dir.y = atof(token);
			token = strtok(NULL, " \n");
			obj->cyl.dir.z = atof(token);
			token = strtok(NULL, " \n");
			obj->cyl.radius = atof(token);
			token = strtok(NULL, " \n");
			obj->cyl.length = atof(token);

			obj->color = cur_color;

			scene->objects[ind_obj++] = obj;
		} else if (strcmp(token, "light") == 0) {
			struct light* light = malloc(sizeof(struct light));
			token = strtok(NULL, " \n");
			light->origin.x = atof(token);
			token = strtok(NULL, " \n");
			light->origin.y = atof(token);
			token = strtok(NULL, " \n");
			light->origin.z = atof(token);

			token = strtok(NULL, " \n");
			if (atoi(token)) {
				light->type = POINT;
			} else {
				light->type = DIRECTIONAL;
			}

			token = strtok(NULL, " \n");
			light->intensity = atof(token);

			scene->lights[ind_light++] = light;
		} else if (strcmp(token, "attlight") == 0) {
			struct light* light = malloc(sizeof(struct light));
			light->type = ATT_POINT;
			token = strtok(NULL, " \n");
			light->origin.x = atof(token);
			token = strtok(NULL, " \n");
			light->origin.y = atof(token);
			token = strtok(NULL, " \n");
			light->origin.z = atof(token);
			
			token = strtok(NULL, " \n");
			if (atoi(token) != 1) {
				printf("Attenuated light source must be a point.\n");
				return -1;
			}

			token = strtok(NULL, " \n");
			light->intensity = atof(token);

			token = strtok(NULL, " \n");
			light->c1 = atof(token);
			token = strtok(NULL, " \n");
			light->c2 = atof(token);
			token = strtok(NULL, " \n");
			light->c3 = atof(token);

			scene->lights[ind_light++] = light;
		} else {   // Unrecognized keyword (not comment)
			printf("Invalid input keyword \"%s\".\n", token);
			fclose(input);
			return -1;
		}
	}

	scene->num_obj = ind_obj;
	scene->num_light = ind_light;

	fclose(input);
	return 0;
}

FILE* initPPM(char* input_fn, int width, int height) {
	// Replace input file suffix with .ppm for output file
	char* output_fn = strtok(input_fn, ".");
	if (output_fn == NULL) output_fn = input_fn;
	strcat(output_fn, ".ppm");

	// Open PPM file and write header
	FILE* output = fopen(output_fn, "w");
	if (output != NULL)
		fprintf(output, "P3\n%d %d\n255\n", width, height);

	return output;
}

void writePixelToPPM(FILE* output, color color) {
	fprintf(output, "%d %d %d\n", 
			(int) round(255*color.r),
			(int) round(255*color.g),
			(int) round(255*color.b));
}
