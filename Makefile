GPPPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/loader.o \
			obj/gdt.o \
			obj/memory.o \
			obj/shell.o \
			obj/multitasking.o \
			obj/syscalls.o \
			obj/filesystem.o \
			obj/drivers/driver.o \
			obj/hardwarecommunication/port.o \
			obj/hardwarecommunication/interruptstubs.o \
			obj/hardwarecommunication/interrupts.o \
			obj/drivers/keyboard.o \
			obj/drivers/mouse.o \
			obj/drivers/vga.o \
			obj/drivers/pit.o \
			obj/drivers/ata.o \
			obj/kernel.o

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	gcc $(GPPPARAMS) -c -o $@ $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

# Create disk image if it doesn't exist
disk.img:
	@echo "Creating virtual disk (10MB)..."
	qemu-img create -f raw disk.img 10M

install: mykernel.bin
	sudo cp $< /boot/mykernel.bin

.PHONY: clean
clean:
	rm -rf obj mykernel.bin mykernel.iso

.PHONY: clean-all
clean-all: clean
	rm -f disk.img

mykernel.iso: mykernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp mykernel.bin iso/boot/mykernel.bin
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=mykernel.iso iso
	rm -rf iso

# run: mykernel.iso
# 	(killall VirtualBox && sleep 1) || true
# 	VirtualBox --startvm "My Operating System" &

run: mykernel.iso disk.img
	(killall qemu-system-i386 && sleep 1) || true
	qemu-system-i386 -cdrom mykernel.iso -drive file=disk.img,format=raw &