CC=x86_64-elf-gcc
LD=x86_64-elf-ld
CFLAGS=-m32 -ffreestanding -nostdlib -fno-stack-protector -fno-pic -Wall -Wextra -Isrc

SOURCES=src/kernel.c src/vga.c src/keyboard.c src/filesystem.c src/shell.c
OBJECTS=$(SOURCES:.c=.o)

all: kernel.iso

kernel.elf: $(OBJECTS) boot.o linker.ld
	$(LD) -m elf_i386 -T linker.ld -o kernel.elf boot.o $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

boot.o: src/boot.S
	$(CC) $(CFLAGS) -c src/boot.S -o boot.o

kernel.iso: kernel.elf
	mkdir -p iso/boot/grub
	cp kernel.elf iso/boot/
	cp boot/grub.cfg iso/boot/grub/
	i686-elf-grub-mkrescue -o kernel.iso iso

run: kernel.iso
	qemu-system-i386 -cdrom kernel.iso

clean:
	rm -rf *.o src/*.o *.elf *.iso iso
