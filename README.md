# MyOS - A Simple Operating System with File System

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
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
- **Custom in-memory file system** with up to 32 files
- **File operations**: create, read, write, delete, and list files
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
- **GRUB tools**: `i686-elf-grub-mkrescue`
- **QEMU**: `qemu-system-i386` for emulation
- **Homebrew** (on macOS) for easy installation

### Installation on macOS

```bash
# Install cross-compilation tools
brew install x86_64-elf-gcc qemu i686-elf-grub

# Clone and build the project
git clone https://github.com/rajarohan/myos.git
cd myos
make run
```

### Building and Running

```bash
# Build the OS
make

# Create bootable ISO
make kernel.iso

# Run in QEMU emulator
make run

# Clean build artifacts
make clean
```

## Usage

Once MyOS boots, you'll see the interactive shell with the prompt `myos>`. Here are the available commands:

### File Management Commands

| Command | Alias | Description | Example |
|---------|-------|-------------|---------|
| `create <file>` | - | Create a new text file | `create hello.txt` |
| `list` | `ls` | List all files | `list` |
| `read <file>` | `cat` | Display file contents | `read hello.txt` |
| `write <file>` | `edit` | Write text to file | `write hello.txt` |
| `delete <file>` | `rm` | Delete a file | `delete hello.txt` |

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

Type 'help' for available commands.

myos> create hello.txt
File 'hello.txt' created successfully.

myos> write hello.txt
Enter text for file 'hello.txt' (press Ctrl+D or empty line to finish):
Hello, World!
This is my first file in MyOS!

Data written to file 'hello.txt' (42 bytes).

myos> list
Files in file system:
Name                    Size (bytes)
------------------------------------
hello.txt               42

Total: 1 files

myos> read hello.txt
Contents of 'hello.txt':
--- BEGIN FILE ---
Hello, World!
This is my first file in MyOS!
--- END FILE ---

myos> info

File System Information:
Files: 1/32
Data used: 42/32768 bytes
Free space: 32726 bytes
```

## Architecture

### Project Structure

```
myos/
├── src/
│   ├── kernel.c        # Main kernel entry point
│   ├── boot.S          # Assembly boot code with multiboot header
│   ├── vga.c/h         # VGA text mode driver
│   ├── keyboard.c/h    # Keyboard input driver
│   ├── filesystem.c/h  # In-memory file system
│   └── shell.c/h       # Interactive command shell
├── boot/
│   └── grub.cfg        # GRUB configuration
├── linker.ld           # Linker script
├── Makefile            # Build system
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

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- **OSDev Wiki** for excellent OS development resources
- **GRUB Project** for the bootloader
- **QEMU Project** for emulation capabilities
- **GNU Toolchain** for cross-compilation tools

## Troubleshooting

### Common Issues

**Error: `i386-elf-gcc: No such file or directory`**
```bash
# Install the cross-compiler toolchain
brew install x86_64-elf-gcc
```

**Error: `no multiboot header found`**
- Ensure `boot.S` is compiled and linked first
- Check that the linker script places `.multiboot` section at the beginning

**QEMU doesn't start**
```bash
# Make sure QEMU is installed
brew install qemu
# Run with full path
qemu-system-i386 -cdrom kernel.iso
```

### Debug Mode

To run with debugging information:
```bash
# Run QEMU with monitor console
qemu-system-i386 -cdrom kernel.iso -monitor stdio

# Add debug symbols during compilation
CFLAGS += -g -DDEBUG make
```

## Contact

- **GitHub**: [@rajarohan](https://github.com/rajarohan)
- **Project**: [MyOS Repository](https://github.com/rajarohan/myos)

---

**Happy OS Development!** 🚀