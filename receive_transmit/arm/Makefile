option= -Wall
.PHONY: a.out
a.out : rz tz

rz : rz.c
	arm-none-linux-gnueabi-gcc  rz.c -o rz $(option)
tz : tz.c
	gcc  tz.c -o tz $(option)

clean:
	rm -rfv *.o rz tz
