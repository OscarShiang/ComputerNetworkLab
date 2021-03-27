CC = gcc
CFLAGS = -std=c11 -g -I./include

TARGET = lab1
OBJS = src/main.o src/file_trans.o

all: $(TARGET)

include common.mk

%.o: %.c
	$(VECHO) "   CC\t$@\n"
	$(Q)$(CC) -c $< $(CFLAGS) -o $@

$(TARGET): $(OBJS)
	$(VECHO) "   LD\t$@\n"
	$(Q)$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(TARGET) $(OBJS)
.PHONY: clean
