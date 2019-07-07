GPPPARAMS = -m32
ASPARAMS = --32

objects = loader.o kernel.o

#to create .o file from .cpp file
%.o: %.cpp
	#run g++ with params, output should be target file ($@), and compile input file ($<)
	g++ $(GPPPARAMS) -o $@ -c $<

$.o: $.s
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

install: mykernel.bin
	sudo cp $< /boot/mykernel.bin