# MyOS - A Simple Operating System with File System

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Platform](https://img.shields.io/badge/platform-x86-lightgrey.svg)]()

MyOS is a simple, educational operating system kernel written in C and Assembly that demonstrates fundamental OS development concepts. It features a custom in-memory file system with full text file management capabilities through an interactive shell interface.

## Features

### 🖥️ Core System
- **32-bit x86 kernel** with multiboot compliance
- **GRUB bootloader** support for easy booting
- **VGA text mode** with color support and scrolling
- **Keyboard driver** with full US QWERTY layout support

### 📁 File System
- **Hierarchical directory system** with Unix-like structure
- **File operations**: create, read, write, delete, and list files
- **Directory operations**: mkdir, rmdir, cd, pwd navigation
- **Path resolution**: support for absolute (/) and relative (.., .) paths
- **Custom in-memory file system** with up to 64 files and directories
- **Text file support** with up to 1KB per file
- **Real-time file management** through interactive commands

### 🖱️ User Interface
- **Interactive shell** with command-line interface
- **Color-coded output** for better user experience
- **Command history** and error handling
- **Help system** with detailed command documentation

## Quick Start

### Prerequisites

Make sure you have the following tools installed:

- **Cross-compiler toolchain**: `x86_64-elf-gcc` and `x86_64-elf-ld`
- **GRUB tools**: `i686-elf-grub-mkrescue` for bootable ISO creation
- **QEMU**: `qemu-system-i386` for OS testing and emulation
- **Make**: Build automation tool
- **Homebrew** (on macOS) for package management

### Installation on macOS

```bash
# Install cross-compilation tools
brew install x86_64-elf-gcc qemu grub

# Verify installation
x86_64-elf-gcc --version
qemu-system-i386 --version
i686-elf-grub-mkrescue --version

# Clone and build the project
git clone https://github.com/rajarohan/myos.git
cd myos

# Build and run in one command
make run
```

### Building and Running

```bash
# Build the OS kernel
make

# Create bootable ISO image
make kernel.iso

# Run in QEMU emulator (recommended)
make run

# Clean all build artifacts
make clean

# Force rebuild everything
make clean && make run
```

### Build Process Details

The build system:
1. Compiles C source files with the cross-compiler (`x86_64-elf-gcc`)
2. Assembles the boot code (`boot.S`) for multiboot compliance
3. Links everything using the custom linker script (`linker.ld`)
4. Creates a GRUB-bootable ISO image with `i686-elf-grub-mkrescue`
5. Launches the OS in QEMU emulator

## Usage

Once MyOS boots, you'll see the interactive shell with the prompt `myos>`. Here are the available commands:

### File Management Commands

| Command | Alias | Description | Example |
|---------|-------|-------------|---------|
| `create <file>` | - | Create a new text file | `create hello.txt` |
| `read <file>` | `cat` | Display file contents | `read hello.txt` |
| `write <file>` | `edit` | Write text to file | `write hello.txt` |
| `delete <file>` | `rm` | Delete a file | `delete hello.txt` |

### Directory Management Commands

| Command | Description | Example |
|---------|-------------|---------|
| `mkdir <dir>` | Create a new directory | `mkdir documents` |
| `rmdir <dir>` | Remove an empty directory | `rmdir documents` |
| `cd <path>` | Change directory (/, .., dirname) | `cd documents` |
| `pwd` | Show current directory path | `pwd` |
| `list` | `ls` | List directory contents | `list` |

### System Commands

| Command | Description | Example |
|---------|-------------|---------|
| `help` | Show all available commands | `help` |
| `clear` | Clear the screen | `clear` |
| `info` | Show file system information | `info` |

### Example Session

```
MyOS File System Shell v1.0
============================

File system with directory support initialized successfully.

Type 'help' for available commands.

myos:/$ mkdir documents
Directory 'documents' created successfully.

myos:/$ mkdir photos  
Directory 'photos' created successfully.

myos:/$ list
Contents of /:
Type Name                    Size (bytes)
----------------------------------------
DIR  documents               <DIR>
DIR  photos                  <DIR>

Total: 2 items

myos:/$ cd documents
Changed to directory: /documents

myos:/documents$ create hello.txt
File 'hello.txt' created successfully.

myos:/documents$ write hello.txt
Enter text for file 'hello.txt' (press Ctrl+D or empty line to finish):
Hello, World!
This is my first file in the documents folder!

Data written to file 'hello.txt' (50 bytes).

myos:/documents$ list
Contents of /documents:
Type Name                    Size (bytes)
----------------------------------------
FILE hello.txt               50

Total: 1 items

myos:/documents$ read hello.txt
Contents of 'hello.txt':
--- BEGIN FILE ---
Hello, World!
This is my first file in the documents folder!
--- END FILE ---

myos:/documents$ cd ..
Changed to directory: /

myos:/$ pwd
/

myos:/$ info

File System Information:
Current Directory: /
Total entries: 3/64
Directories: 2, Files: 1
Data used: 50/65536 bytes
Free space: 65486 bytes
```

## Architecture

### Project Structure

```
myos/
├── src/
│   ├── kernel.c        # Main kernel entry point
│   ├── boot.S          # Assembly boot code with multiboot header
│   ├── vga.c/h         # VGA text mode driver with color support
│   ├── keyboard.c/h    # PS/2 keyboard input driver
│   ├── filesystem.c/h  # Hierarchical in-memory file system
│   └── shell.c/h       # Interactive command shell with directory support
├── boot/
│   └── grub.cfg        # GRUB configuration
├── linker.ld           # Linker script for memory layout
├── Makefile            # Cross-compilation build system
└── README.md           # This file
```

### Technical Details

- **Target Architecture**: x86 32-bit (i386)
- **Bootloader**: GRUB with multiboot specification
- **Memory Model**: Flat memory model with 16KB kernel stack
- **Display**: VGA text mode (80x25 characters, 16 colors)
- **Input**: PS/2 keyboard with scan code translation
- **File System**: Simple allocation table with linear data storage

## Development

### Building from Source

The build system uses a cross-compilation toolchain to generate 32-bit x86 code:

1. **Compile C sources** to object files
2. **Assemble boot code** with multiboot header
3. **Link kernel** using custom linker script
4. **Create ISO image** with GRUB bootloader
5. **Run in emulator** for testing

### Memory Layout

```
0x00100000 (1MB)  : Kernel start (multiboot header)
0x00101000        : Text section (.text)
0x00102000        : Read-only data (.rodata)
0x00103000        : Data section (.data)
0x00104000        : BSS section (.bss)
0x00105000+       : Kernel stack and heap
```

### File System Design

The file system uses a simple design with:

- **File Allocation Table**: Array of file entries with metadata
- **Data Area**: Linear storage for file contents
- **Memory Management**: Simple bump allocator for file data
- **Maximum Capacity**: 32 files, 1KB each (32KB total)

## Contributing

Contributions are welcome! Here are some ideas for enhancements:

### Potential Features
- [ ] Persistent file system (disk storage)
- [ ] Directory support
- [ ] File permissions and attributes
- [ ] Process management and multitasking
- [ ] Network stack implementation
- [ ] Device driver framework
- [ ] Memory management unit (MMU) support

### Getting Started
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly in QEMU
5. Submit a pull request

## Educational Value

This project demonstrates key operating system concepts:

- **Kernel Development**: Low-level system programming
- **Bootloading**: Understanding system startup process
- **Device Drivers**: Hardware abstraction and I/O
- **File Systems**: Data organization and storage
- **Memory Management**: Allocation and addressing
- **User Interfaces**: Command-line interaction design


## Acknowledgments

- **OSDev Wiki** for excellent OS development resources
- **GRUB Project** for the bootloader
- **QEMU Project** for emulation capabilities
- **GNU Toolchain** for cross-compilation tools

## Troubleshooting

### Common Issues

**Error: `x86_64-elf-gcc: No such file or directory`**
```bash
# Install the cross-compiler toolchain
brew install x86_64-elf-gcc
# Verify installation
which x86_64-elf-gcc
```

**Error: `no multiboot header found`**
- Ensure `boot.S` contains the multiboot header magic numbers
- Verify the linker script places `.multiboot` section at the beginning
- Check that boot.o is the first object file in linking order

**Error: `i686-elf-grub-mkrescue: command not found`**
```bash
# Install GRUB tools
brew install grub
# Verify GRUB installation
i686-elf-grub-mkrescue --version
```

**QEMU doesn't start or crashes**
```bash
# Ensure QEMU is properly installed
brew install qemu
qemu-system-i386 --version

# Run with explicit ISO path
qemu-system-i386 -cdrom ./kernel.iso

# Try with additional memory
qemu-system-i386 -cdrom kernel.iso -m 256M
```

**Build fails with linker errors**
```bash
# Clean and rebuild
make clean
make

# Check for missing object files
ls -la *.o
```

### Debug Mode

For development and debugging:
```bash
# Run QEMU with monitor console for debugging
qemu-system-i386 -cdrom kernel.iso -monitor stdio -no-reboot

# Build with debug symbols
make clean
CFLAGS="-g -DDEBUG" make

# Enable verbose output during build
make V=1
```

### Performance Tuning

```bash
# Run with hardware acceleration (if available)
qemu-system-i386 -cdrom kernel.iso -enable-kvm

# Allocate more memory to the VM
qemu-system-i386 -cdrom kernel.iso -m 512M

# Use multiple CPU cores
qemu-system-i386 -cdrom kernel.iso -smp 2
```

## Contact

- **GitHub**: [@rajarohan](https://github.com/rajarohan)
- **Project**: [MyOS Repository](https://github.com/rajarohan/myos)
