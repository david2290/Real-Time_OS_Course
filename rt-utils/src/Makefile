
all:
	gcc -c schedule-utils.c -lm `pkg-config --cflags --libs glib-2.0` `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -export-dynamic -g -Wall
	gcc main.c schedule-utils.o -o rt-util -Wno-format -Wno-format -Wno-deprecated-declarations -Wno-format-security -lm `pkg-config --cflags --libs glib-2.0` `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -export-dynamic -g -Wall -lm
clean:
	rm output.*
	rm rt-util
