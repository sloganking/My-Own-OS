GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
# 32 bit
ASPARAMS = --32
LDPARAMS = -melf_i386

# object files
objects = loader.o gdt.o kernel.o

#to create .o file from .cpp file
%.o: %.cpp
	#run g++ with params, output should be target file ($@), and compile input file ($<)
	g++ $(GPPPARAMS) -o $@ -c $<

%.o: %.s
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


# create mykernel.iso which will depend on mykernel.bin
mykernel.iso: mykernel.bin

	# create directory structure of resulting CD
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub

	# copy kernel into boot dir
		cp $< iso/boot/

	#create grub cfg manually
		# don't wait for user to select entry (since there is only one)
			echo 'set timeout=0' >> iso/boot/grub/grub.cfg

		# one entry so set it as default
			echo 'set default=0' >> iso/boot/grub/grub.cfg

		echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
		echo ' multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
		echo ' boot' >> iso/boot/grub/grub.cfg
		echo '}' >> iso/boot/grub/grub.cfg

	# create CD image
		# make image using iso directory
			grub-mkrescue --output=$@ iso
		# delete no longer used iso directory
			rm -rf iso

# start instance of virtualbox running this OS
# not yet working
run: mykernel.iso
	(killall VirtualBox && sleep 1) || true
	VirtualBox --startvm "My OS" &