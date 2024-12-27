.PHONY: all clean deps run

all: life

life: life.c
	gcc life.c -o life -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2

clean:
	rm -f life

deps:
	if ! brew list sdl2 > /dev/null 2>&1; then \
		brew install sdl2; \
	fi

run: deps life
	./life
