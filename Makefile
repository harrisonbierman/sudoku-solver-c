FLAGS = \
	-g -fsanitize=address -fno-omit-frame-pointer\
	-Wall -Wextra -Wpedantic \
	-std=c99\

default:	
	gcc $(FLAGS) -o game main.c
	./game
