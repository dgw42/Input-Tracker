/*Contains data of where mouse has been*/
struct mouse_map {
	int *arr; //2D array allocated to size of screen in row-major order
	int width;
	int height;
};

struct mouse_thread_args {
	struct mouse_map* map;
	char* mouse_path;
};

//initialize mouse_map struct, array is size of screen resolution
int mouse_map_init(struct mouse_map** map);

//free an initialized mouse_map struct
void mouse_map_free(struct mouse_map** map);

//thread function to read from mouse input file
void * mouse_read(void * args);
