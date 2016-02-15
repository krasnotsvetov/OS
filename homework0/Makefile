all:hello	
Act = $< -o $@
clean:
	rm hello hello.o
hello.o: hello.c
	gcc -c $(Act)
hello: hello.o
	gcc -s $(Act)