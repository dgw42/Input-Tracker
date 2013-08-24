#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include "mouse.h"

void create_map_image(struct mouse_map* map, char* path);

int main(void) {
	struct mouse_map* map;
	pthread_t thread;
	mouse_map_init(&map);
	pthread_create(&thread, NULL, mouse_read, map);
	sleep(10);
	map->exit = true;
	pthread_join(thread, NULL);
	create_map_image(map, "out.png");
	mouse_map_free(&map);
}

void create_map_image(struct mouse_map* map, char* path) {
	IplImage* image;
	CvScalar black, white;
	int r, c;
	image = cvCreateImage(cvSize(map->width, map->height), IPL_DEPTH_8U, 3);
	black = cvScalar(0,0,0);
	white = cvScalar(255,255,255);
	for(r = 0; r < map->height; r++) {
		for(c = 0; c < map->width; c++) {
			if(map->arr[c + (map->width * r)])
				cvSet2D(image, c, r, black);
			else
				cvSet2D(image, c, r, white);
		}
	}
	cvSaveImage(path, image);
	cvRealeaseImage(&image);
}
