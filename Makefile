compile:
	gcc -o input -lm -pthread src/main.c src/keyboard.c src/mouse.c -lX11 `pkg-config --cflags --libs opencv`
run:
	./input
