cc = gcc
OBJ = ksh
all:
	$(cc) -c ./1.3.c 
	$(cc) -o $(OBJ) ./1.3.o /lib64/libreadline.so.6 -I -lreadlin -lncurses -g
	rm *.o 
clean:
	rm *.o
