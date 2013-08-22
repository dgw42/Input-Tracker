struct mouse_map {
	int *arr; //2D array allocated to size of screen in row-major order
	int width;
	int height;
}

int mouse_map_init(struct mouse_map** map);

int mouse_map_free(struct mouse_map** map);

void * mouse_read(void * args);
