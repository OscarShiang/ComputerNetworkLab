CC = gcc
CFLAGS = -std=c11 -g -I./include

TARGET = server client

SERVER_OBJS = src/server.o
CLIENT_OBJS = src/client.o

all: $(TARGET)

include common.mk

%.o: %.c
	$(VECHO) "   CC\t$@\n"
	$(Q)$(CC) -c $< $(CFLAGS) -o $@

server: $(SERVER_OBJS)
	$(VECHO) "   LD\t$@\n"
	$(Q)$(CC) -o $@ $^ $(CFLAGS)

client: $(CLIENT_OBJS)
	$(VECHO) "   LD\t$@\n"
	$(Q)$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(TARGET) $(OBJS)
.PHONY: clean
