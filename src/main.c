#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "mouse.h"

int main(void) {
	struct mouse_map* map;
	pthread_t thread;
	mouse_map_init(&map);
	pthread_create(&thread, NULL, mouse_read, map);
	pthread_join(thread, NULL);
	mouse_map_free(&map);
}
