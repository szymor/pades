.PHONY: all clean

PROJECT = pades

CC = gcc
CFLAGS = -std=c99 -g -I/usr/include/SDL
LDFLAGS = -lSDLmain -lSDL -lSDL_gfx


all: $(PROJECT)

$(PROJECT): main.c
	$(CC) $(CFLAGS) -o $(PROJECT) $< $(LDFLAGS)

clean:
	rm -rf $(PROJECT)
