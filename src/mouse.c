#include <stdio.h>
#include <linux/input.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <mouse.h>

int mouse_map_init(struct mouse_map** map) {
	Display* disp;
	Screen* screen;
	int height, width;
	disp = XOpenDisplay(NULL);
	screen = DefaultScreenOfDisplay(disp);
	*map = (struct mouse_map*)malloc(sizeof(struct mouse_map));
	if(!(*map))
		return -1;
	(*map)->width = screen->width;
	(*map)->height = screen->height;
	width = screen->width;
	height = screen->height;
	(*map)->arr = (int*)calloc((width*height), sizeof(int));
	if(!((*map)->arr))
		return -1;
	return 0;
}

void mouse_map_free(struct mouse_map** map) {
	free((*map)->arr);
	free(*map);
}
