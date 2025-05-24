CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Iinclude
EXE_EXT =
RM = rm -f
SOCKET_LIBS =

TARGETS = server$(EXE_EXT) client$(EXE_EXT)

all: $(TARGETS)

# (server.c + logger.c -> server)
server$(EXE_EXT): server.c include/logger.c
	$(CC) $(CFLAGS) $^ -o $@ $(SOCKET_LIBS)

# (client.c + logger.c → client)
client$(EXE_EXT): client.c include/logger.c
	$(CC) $(CFLAGS) $^ -o $@ $(SOCKET_LIBS)

# Temizleme (sadece çalıştırılabilir dosyaları sil)
clean:
	$(RM) $(TARGETS)