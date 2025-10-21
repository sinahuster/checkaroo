CFLAGS = -g -Wall -Wextra -fsanitize=address -fsanitize=undefined 

all: checkaroo unit_tests

checkaroo: checkaroo.c tasks.c tasks.h
	cc $(CFLAGS) -o $@ checkaroo.c tasks.c

unit_tests: unit_tests.c tasks.c tasks.h
	cc $(CFLAGS) -o $@ unit_tests.c tasks.c

.PHONY: clean tests
clean:
	rm -f checkaroo unit_tests

tests: unit_tests
	@./unit_tests