all: mpv-remote.c
	gcc -o mpv-remote.so mpv-remote.c `pkg-config --cflags --libs json-c mpv` -shared -fPIC
	rm ~/.config/mpv/scripts/mpv-remote.so
	ls ~/.config/mpv/scripts/
	mv mpv-remote.so ~/.config/mpv/scripts/
	ls ~/.config/mpv/scripts/

