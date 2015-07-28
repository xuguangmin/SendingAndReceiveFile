
all:
	cd arm && $(MAKE)
	cd pc  && $(MAKE)
	
	mv arm/tz	./	
	mv pc/tz	arm/
	mv ./tz		./pc
clean:
	cd arm && $(MAKE) MAKEFLAGS= clean -w
	cd pc  && $(MAKE) MAKEFLAGS= clean -w
