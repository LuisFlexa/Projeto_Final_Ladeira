CC = gcc
CFLAGS = -Wall -Iinclude -g
LDFLAGS = -lm

SRC = src/main.c src/ClassFileReader.c src/ClassFileViewer.c
OBJ = $(SRC:.c=.o)
EXEC = bin

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(EXEC)
