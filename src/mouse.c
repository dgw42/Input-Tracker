#include <stdio.h>
#include <linux/input.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "mouse.h"

int mouse_map_init(struct mouse_map** map) {
	int height, width;
	*map = (struct mouse_map*)malloc(sizeof(struct mouse_map));
	if(!(*map))
		return -1;
	(*map)->disp = XOpenDisplay(NULL);
	(*map)->screen = DefaultScreenOfDisplay((*map)->disp);
	(*map)->width = (*map)->screen->width;
	(*map)->height = (*map)->screen->height;
	width = (*map)->screen->width;
	height = (*map)->screen->height;
	(*map)->exit = false;
	(*map)->arr = (int*)calloc((width*height), sizeof(int));
	if(!((*map)->arr))
		return -1;
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
		Window root_return, child_return;
		Window root_win = XRootWindow(map->disp, 0);
		int root_x, root_y, win_x, win_y;
		unsigned int mask_return;
		if(XQueryPointer(map->disp, root_win, 
					&root_return, &child_return, 
					&root_x, &root_y, &win_x, &win_y, &mask_return)) {
			map->arr[(root_x) + (map->width * root_y)] = 1;
		}
		if(map->exit)
			return;
	}
}
