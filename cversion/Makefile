all: mpv-remote.c
	gcc -o mpv-remote.so mpv-remote.c -ljson-c `pkg-config --cflags mpv` -shared -fPIC
	./mpv-remote.so
