#include <stdio.h>
#include <linux/input.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "mouse.h"

extern char* save_format;
extern char* name_format;

int mouse_map_init(struct mouse_map** map, int screen_num) {
	int height, width;
	*map = (struct mouse_map*)malloc(sizeof(struct mouse_map));
	if(!(*map))
		return -1;
	(*map)->disp = XOpenDisplay(NULL);
	if(!((*map)->disp))
		return -1;
	if(screen_num >= XScreenCount((*map)->disp))
		return -1;
	(*map)->screen = XScreenOfDisplay((*map)->disp, screen_num);
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
		Window root_win = XRootWindowOfScreen(map->screen);
		int root_x, root_y, win_x, win_y;
		unsigned int mask_return;
		if(XQueryPointer(map->disp, root_win, 
					&root_return, &child_return, 
					&root_x, &root_y, &win_x, &win_y, &mask_return)) {
			map->arr[(root_x) + (map->width * root_y)] = 1;
		}
		if(map->exit) {
			char path[50];
			sprintf(path, "%s%d.%s", name_format, map->screen_num, save_format);
			create_map_image(map, path);
			return;
		}
	}
}
