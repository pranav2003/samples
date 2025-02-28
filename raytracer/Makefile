build:
	gcc -lm raytracer.c utils.c fileio.c -o raytracer1b

debug:
	gcc -lm -g -fsanitize=address -fsanitize=undefined raytracer.c utils.c fileio.c -o raytracer1b

clean:
	rm -r raytracer1b* *.ppm
