all: hello_world	
Act = $< -o $@
clean:
	rm -f hello_world hello_world.o

hello_world.o: hello_world.c
	gcc -c $(Act)

hello_world: hello_world.o
	gcc -s $(Act)
