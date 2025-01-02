CC = gcc
CFLAGS = -Wall -Wextra -O3 -fopenmp
LDFLAGS = -lSDL2 -lm -lgmp -lmpfr -lmpc

TARGET = mandelbrot
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)
