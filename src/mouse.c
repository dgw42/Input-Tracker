/*
Author: David Walton
Date: 8/24/2013
Project: Input Tracker
   */
#include <stdio.h>
#include <linux/input.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "mouse.h"

//globals from main.c, needed for output formatting
extern char* save_format;
extern char* name_format;

int mouse_map_init(struct mouse_map** map, int screen_num) {
	int height, width;
	//allocate struct
	*map = (struct mouse_map*)malloc(sizeof(struct mouse_map));
	//malloc error
	if(!(*map))
		return -1;

	//retrieve display
	(*map)->disp = XOpenDisplay(NULL);
	//Xlib open error
	if(!((*map)->disp))
		return -1;
	//screen number doesn't exist
	if(screen_num >= XScreenCount((*map)->disp))
		return -1;

	//initialize struct fields
	(*map)->screen = XScreenOfDisplay((*map)->disp, screen_num);
	(*map)->width = (*map)->screen->width;
	(*map)->height = (*map)->screen->height;
	width = (*map)->screen->width;
	height = (*map)->screen->height;
	(*map)->exit = false;
	//allocate and initialize map array
	(*map)->arr = (int*)calloc((width*height), sizeof(int));
	//calloc error
	if(!((*map)->arr))
		return -1;
	//no failures
	return 0;
}

void mouse_map_free(struct mouse_map** map) {
	free((*map)->arr);
	XCloseDisplay((*map)->disp);
	free(*map);
}

void * mouse_read(void * args) {
	struct mouse_map * map = (struct mouse_map*)args;
	while(1) {
		//values for XQueryPointer call
		Window root_return, child_return;
		Window root_win = XRootWindowOfScreen(map->screen);
		int root_x, root_y, win_x, win_y;
		unsigned int mask_return;

		//Query pointer set pixel to 1 if proper return
		if(XQueryPointer(map->disp, root_win, 
					&root_return, &child_return, 
					&root_x, &root_y, &win_x, &win_y, &mask_return)) {
			map->arr[(root_x) + (map->width * root_y)] = 1;
		}
		//exit signaled, cleanup and create output image for this thread
		if(map->exit) {
			char path[50];
			sprintf(path, "%s%d.%s", name_format, map->screen_num, save_format);
			create_map_image(map, path);
			return;
		}
	}
}
