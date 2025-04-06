CC=gcc
CFLAGS=-I -Wall -Wextra -MMD -MP
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
DEPS=$(OBJS:.o=.d)


%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS) main

