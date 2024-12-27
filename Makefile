life: life.c
	gcc life.c -o life -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2

clean:
	rm -f life
