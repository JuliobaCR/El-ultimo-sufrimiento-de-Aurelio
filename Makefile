CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = laberinto
SRCS = main.c grafo.c laberinto.c algoritmos.c archivos.c memes.c
OBJS = $(SRCS:.c=.o)
HEADERS = grafo.h laberinto.h algoritmos.h archivos.h memes.h config.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -DDEBUG -g
debug: clean $(TARGET)

.PHONY: all clean run debug