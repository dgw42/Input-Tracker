#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include "mouse.h"
#include "cv.h"
#include "highgui.h"
#include "math.h"

static unsigned int runtime = 10;
char* save_format = "png";
char* name_format = "out";

int main(int argc, char** argv) {
	//needed call variables
	struct mouse_map** maps;
	int num_displays, i, option;
	Display* disp;
	pthread_t* threads;

	//options section
	while((option = getopt(argc, argv, "t:n:i:")) != -1) {
		switch(option) {
			//'-t' option, specifies time in seconds to run
			case 't': runtime = atoi(optarg);
					  break;
			case 'n': name_format = optarg;
					  break;
			case 'i': switch(strcmp(optarg, "png") && strcmp(optarg, "jpg")) {
						  case 0: save_format = optarg;
								  break;
						  default: fprintf(stderr, 
"-i specified with invalid format%s, png and jpg supported, do not include '.'\n", 
										   optarg);
								   break;
					  }
					  break;
			case '?': fprintf(stderr, "unknown option specified");
					  exit(-1);
					  break;
			default: break;
		}
	}

	//meat section
	disp = XOpenDisplay(NULL);
	num_displays = XScreenCount(disp);
	XCloseDisplay(disp);
	maps = (struct mouse_map**)malloc(sizeof(struct mouse_map*) * num_displays);
	threads = (pthread_t*)malloc(sizeof(pthread_t) * num_displays);
	for(i = 0; i < num_displays; i++)
	{
		mouse_map_init(&(maps[i]), i);
		pthread_create(&(threads[i]), NULL, mouse_read, maps[i]);
	}
	printf("Threads Created!\n");
	sleep(runtime);
	for(i = 0; i < num_displays; i++) {
		maps[i]->exit = true;
		pthread_join(threads[i], NULL);
	}
	for(i = 0; i < num_displays; i++)
		mouse_map_free(&(maps[i]));
	free(maps);
	free(threads);
}

void create_map_image(struct mouse_map* map, char* path) {
	IplImage* image;
	CvScalar black, white;
	int r, c;
	image = cvCreateImage(cvSize(map->width, map->height), IPL_DEPTH_8U, 3);
	black = cvScalar(0,0,0,0);
	white = cvScalar(255,255,255,255);
	for(r = 0; r < map->height; r++) {
		for(c = 0; c < map->width; c++) {
			if(map->arr[c + (map->width * r)])
				cvSet2D(image, r, c, black);
			else
				cvSet2D(image, r, c, white);
		}
	}
	cvSaveImage(path, image, NULL);
	cvReleaseImage(&image);
}
