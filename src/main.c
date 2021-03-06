/*
Author: David Walton
Date: 8/24/2013
Project: Input Tracker
   */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include "mouse.h"
#include "keyboard.h"
#include "cv.h"
#include "highgui.h"
#include "math.h"

/* globals, only used as constants after setting them in options*/
static unsigned int runtime = 10;
char* save_format = "png";
char* name_format = "out";

int main(int argc, char** argv) {
	//needed call variables
	struct mouse_map** maps;
	struct key_thread_args* key_args;
	int num_displays, i, option;
	char *logfile, *statfile;
	Display* disp;
	pthread_t *threads, board_thread;

	logfile = "log.txt";
	statfile = "stat.txt";

	//options section
	while((option = getopt(argc, argv, "t:n:i:s:l:h")) != -1) {
		switch(option) {
			case 'h': printf("Usage: %s [-t time in seconds] [-n name prefix format] [-i image format(png or jpg)]\n", argv[0]);
					  exit(-1);
					  break;
			//'-t' option, specifies time in seconds to run
			case 't': runtime = atoi(optarg);
					  break;
			//'-n' option, specifies name prefix for output image
			case 'n': name_format = optarg;
					  break;
			//'-i' option, specifies file format, currently png and jpg supported
			case 'i': switch(strcmp(optarg, "png") && strcmp(optarg, "jpg")) {
						  case 0: save_format = optarg;
								  break;
						  default: fprintf(stderr, "-i specified with invalid format '%s', png and jpg supported, do not include '.'\n", optarg);
								   break;
					  }
					  break;
			//'-s' option, specifies file to output stats
			case 's': statfile = optarg;
					  break;
			//'-l' option, specifies log file
			case 'l': logfile = optarg;
					  break;
			//bad option
			case '?': fprintf(stderr, "unknown option specified");
					  exit(-1);
					  break;
			//use defaults
			default: break;
		}
	}

	disp = XOpenDisplay(NULL);
	num_displays = XScreenCount(disp);
	XCloseDisplay(disp);

	//Allocate needed mouse map structures
	maps = (struct mouse_map**)malloc(sizeof(struct mouse_map*) * num_displays);
	key_args = (struct key_thread_args*)malloc(sizeof(struct key_thread_args));
	key_args->logfilename = logfile;
	key_args->statfilename = statfile;
	key_args->exit = false;
	//Allocate array to hold thread structs
	threads = (pthread_t*)malloc(sizeof(pthread_t) * num_displays);
	//spawn threads, 1 per display
	for(i = 0; i < num_displays; i++)
	{
		mouse_map_init(&(maps[i]), i);
		pthread_create(&(threads[i]), NULL, mouse_read, maps[i]);
	}
	pthread_create(&board_thread, NULL, key_thread, key_args);
	
	//sleep for specified runtime amout
	sleep(runtime);
	key_args->exit = true;
	for(i = 0; i < num_displays; i++) {
		//signal thread to save image of mouse map and exit
		maps[i]->exit = true;
		pthread_join(threads[i], NULL);
	}
	pthread_join(board_thread, NULL);

	//Free up used memory
	for(i = 0; i < num_displays; i++)
		mouse_map_free(&(maps[i]));
	free(maps);
	free(threads);
	free(key_args);
}

void create_map_image(struct mouse_map* map, char* path) {
	IplImage* image;
	CvScalar black, white;
	int r, c;

	//allocate image
	image = cvCreateImage(cvSize(map->width, map->height), IPL_DEPTH_8U, 3);
	black = cvScalar(0,0,0,0);
	white = cvScalar(255,255,255,255);

	//loop through each pixel of the screen
	for(r = 0; r < map->height; r++) {
		for(c = 0; c < map->width; c++) {
			//if mouse was there, black else white
			if(map->arr[c + (map->width * r)])
				cvSet2D(image, r, c, black);
			else
				cvSet2D(image, r, c, white);
		}
	}
	//save the image and then free it
	cvSaveImage(path, image, NULL);
	cvReleaseImage(&image);
}
