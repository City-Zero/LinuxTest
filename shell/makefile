cc = gcc
OBJ = te
all:
	$(cc) -c ./te.c 
	$(cc) -o $(OBJ) ./te.o /lib64/libreadline.so.6 -I -lreadlin -lncurses -g
	rm *.o 
clean:
	rm *.o
