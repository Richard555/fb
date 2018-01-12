all:
	gcc -g3 -O0 -o fb fb.c
arm:
	arm-linux-gnueabi-gcc fb.c -o fb_arm
clean:
	rm fb
	rm fb_arm


