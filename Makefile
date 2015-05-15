all:
	g++ -g squeaker.c -o squeaker `sdl2-config --cflags --libs` -lSDL2_mixer -lX11
