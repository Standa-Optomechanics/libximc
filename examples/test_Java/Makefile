# build with provided library on osx
UNAME := $(shell uname)
SOVER = 7
NAME = $(shell basename `pwd`).jar
ifeq ($(UNAME), Darwin)
LIBJXIMC=../../ximc/macosx/libjximc.jar
else
LIBJXIMC=libjximc.jar
endif

all: clean $(NAME)

run: all
	java -jar $(NAME)

$(NAME): ru/ximc/TestJava.java
	javac -classpath .:$(LIBJXIMC) -Xlint ru/ximc/TestJava.java
	jar cmf MANIFEST.MF $@ ru

clean:
	rm -f $(NAME) ru/ximc/*.class libjximc.jar
