all:
	gcc counter.c uthread.c -o counter -Wno-deprecated -Wno-pointer-to-int-cast
	gcc thread-hello.c uthread.c -o hello -Wno-deprecated -Wno-pointer-to-int-cast
	gcc demo.c -o demo -Wno-deprecated

debug:
	gcc -g counter.c uthread.c -o counter -Wno-deprecated -Wno-pointer-to-int-cast
	gcc -g thread-hello.c uthread.c -o hello -Wno-deprecated -Wno-pointer-to-int-cast
	gcc -g demo.c -o demo -Wno-deprecated



# Note: you can use valgrind to find the location
# of segmentation fault errors compile with -g (debug)
# and then run valgrind:
# valgrind --leak-check=full ./counter
# valgrind --leak-check=full ./hello