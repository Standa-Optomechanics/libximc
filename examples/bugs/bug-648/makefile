CC = gcc
CFLAGS += -g -O0 -Wall -Wextra -Werror -pedantic -Wno-unused -std=c99 -D_POSIX_C_SOURCE=199309L -DNOFRAMEWORK
LDFLAGS += -lximc

NAME = testapp
OBJ = $(NAME).o

all: clean $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@

clean:
	rm -f $(NAME) *.o
	rm -rf build
