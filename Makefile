GPPPARAMS = -m32
# 32 bit
ASPARAMS = --32
LDPARAMS = -melf_i386

# object files
objects = loader.o kernel.o

#to create .o file from .cpp file
%.o: %.cpp
	#run g++ with params, output should be target file ($@), and compile input file ($<)
	g++ $(GPPPARAMS) -o $@ -c $<

$.o: $.s
	as $(ASPARAMS) -o $@ $<

# at the end create mykernel.bin
# this will depend on linker.ld and the object (.o) files
mykernel.bin: linker.ld $(objects)

	# -T $< - as input file use linker.ld
	# -o $@ - create target file
	# $(objects) - as input use all the object files
	ld $(LDPARAMS) -T $< -o $@ $(objects)

# once mykernel.bin is complete, we want to install it
install: mykernel.bin
	sudo cp $< /boot/mykernel.bin