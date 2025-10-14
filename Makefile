CFLAGS = -g -Wall -Wextra -fsanitize=address -fsanitize=undefined 

checkaroo: checkaroo.c tasks.c tasks.h
	cc $(CFLAGS) -o $@ checkaroo.c tasks.c 

.PHONY: clean
clean:
	rm -f checkaroo