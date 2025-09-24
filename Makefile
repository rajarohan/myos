CC=x86_64-elf-gcc
LD=x86_64-elf-ld
CFLAGS=-m32 -ffreestanding -nostdlib -fno-stack-protector -fno-pic -Wall -Wextra

all: kernel.iso

kernel.elf: src/kernel.c src/boot.S linker.ld
	$(CC) $(CFLAGS) -c src/kernel.c -o kernel.o
	$(CC) $(CFLAGS) -c src/boot.S -o boot.o
	$(LD) -m elf_i386 -T linker.ld -o kernel.elf boot.o kernel.o

kernel.iso: kernel.elf
	mkdir -p iso/boot/grub
	cp kernel.elf iso/boot/
	cp boot/grub.cfg iso/boot/grub/
	i686-elf-grub-mkrescue -o kernel.iso iso

run: kernel.iso
	qemu-system-i386 -cdrom kernel.iso

clean:
	rm -rf *.o *.elf *.iso iso
