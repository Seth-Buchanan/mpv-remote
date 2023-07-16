all: mpv-remote.c
	gcc -o mpv-remote.so mpv-remote.c `pkg-config --cflags --libs json-c mpv` -shared -fPIC

