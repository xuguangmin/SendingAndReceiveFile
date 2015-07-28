.PHONY: a.out
a.out : rz tz

rz : rz.c
	gcc  rz.c -o rz
tz : tz.c
	arm-none-linux-gnueabi-gcc tz.c -o tz

clean:
	rm -rfv *.o rz tz
