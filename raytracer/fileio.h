#include "utils.h"
#include <stdio.h>

/** Parse input file and populate view and scene structs **/
int parseInput(struct view_params* view, struct scene* scene, char* input_fn);

/** Initialize PPM file with header **/
FILE* initPPM(char* input_fn, int width, int height);

/** Write a single pixel to PPM file **/
void writePixelToPPM(FILE* output, color color);
