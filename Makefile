# Compiler & flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic
EXE_EXT =
RM = rm -f
SOCKET_LIBS =

TARGETS = server$(EXE_EXT) client$(EXE_EXT)

all: $(TARGETS)

server$(EXE_EXT): server.c
	$(CC) $(CFLAGS) $^ -o $@ $(SOCKET_LIBS)

client$(EXE_EXT): client.c
	$(CC) $(CFLAGS) $^ -o $@ $(SOCKET_LIBS)

clean:
	$(RM) $(TARGETS)