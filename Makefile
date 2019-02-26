FLAGS = -Wall -Werror -std=gnu99 -g
.PHONY: all clean

all: format_errno

format_errno: format_errno.c
	gcc ${FLAGS} $^ -o $@

clean:
	rm -f *.o format_errno
