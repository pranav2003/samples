#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "utils.h"
#include "fileio.h"

int main(int argc, char* argv[]) {
	// Check number of inputs
	if (argc < 2) {
		printf("Provide an input file.\n");
		return -1;
	} else if (argc > 2) {
		printf("Too many inputs.\n");
		return -1;
	}
	char* input_fn = argv[1];

	// Declare required structs
	struct view_params view;
	struct window_params window;
	struct scene scene;

	// Populate view and scene information from input file
	if (parseInput(&view, &scene, input_fn) == -1) {
		printf("Error parsing input file.\n");
		return -1;
	}

	// Populate window information
	if (genWindow(&window, &view) == -1) {
		printf("Error generating viewing window.\n");
		return -1;
	}

	FILE* output = initPPM(input_fn, view.width, view.height);

	for (int row = 0; row < view.height; row++) {
		for (int col = 0; col < view.width; col++) {
			ray ray = pointsToRay(view.eye, 
									sum(sum(window.ul, scale(col, window.dh)), 
										 	scale(row, window.dv)));
			float dist = 0, min_dist = 0;

			writePixelToPPM(output, traceRay(&ray, &scene));
		}
	}

	fclose(output);

	return 0;
}
