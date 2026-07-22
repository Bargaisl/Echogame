CC = gcc
CFLAGS = -std=c99 -Os -s -ffunction-sections -fdata-sections -Wall
LDFLAGS = -Wl,--gc-sections -s -lraylib -lopengl32 -lgdi32 -lwinmm -luser32 -lshell32

SRC = main.c game.c world.c particles.c audio.c
OBJ = $(SRC:.c=.o)
TARGET = echo.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /f /q *.o $(TARGET) 2>NUL || rm -f *.o $(TARGET)

.PHONY: all clean
