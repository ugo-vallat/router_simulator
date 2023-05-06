CC=gcc
CFLAGS=-std=c99 -Wextra -Wall -Werror -pedantic -lsignal

ifeq ($(DEBUG),yes)
	CFLAGS += -g
else
	CFLAGS += -O3 -DNDEBUG
endif

EXEC=test_list test_tab_rout test_packet test_router
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all:
ifeq ($(DEBUG),yes)
	@echo "Generating in debug mode"
else
	@echo "Generating in release mode"
endif
	@$(MAKE) $(EXEC)

test_list: test_list.o linked_list.o
	@$(CC) -o $@ $^

test_tab_rout: test_tab_rout.o tab_rout.o linked_list.o packet.o
	@$(CC) -o $@ $^

test_packet: test_packet.o packet.o linked_list.o
	@$(CC) -o $@ $^

test_router: test_router.o router.o tab_rout.o packet.o linked_list.o
	@$(CC) -o $@ $^



%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)


clean:
	@rm -rf *.o

deliver:
	zip -r $(DELIVER_FMT).zip .

