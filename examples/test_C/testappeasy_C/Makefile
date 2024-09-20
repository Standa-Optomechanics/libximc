UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
CFLAGS += -F../../../ximc/macosx
LDFLAGS += -framework libximc -rpath '@executable_path/' -rpath '../../../ximc/macosx'
else
CFLAGS += -I../../../ximc/ -I../../../ximc/macosx
LDFLAGS += -L../../../ximc/macosx -lximc
endif

CC ?= gcc
CFLAGS += -g -O0 -Wall -Wextra -Werror -pedantic -Wno-unused -std=c99

NAME = $(shell basename `pwd`)
OBJ = $(NAME).o

all: clean $(NAME)

run: all
	./$(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@

clean:
	rm -f $(NAME) *.o
	rm -rf build
