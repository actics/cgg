gtkflags = `pkg-config --cflags --libs gtk+-3.0` 
cflags = -lm -pedantic -Wall

build:
	gcc cgg.c cggfirst.c cggsecond.c $(gtkflags) $(cflags) -o cgg
