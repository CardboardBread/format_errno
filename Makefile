FLAGS = -Wall -Werror -std=gnu99 -g
.PHONY: all clean

all: format_errno

format_errno: format_errno.o
	gcc ${FLAGS} -o $@ $^

%.o : %.c
	gcc ${FLAGS} -c $<

clean:
	rm -f *.o format_errno
