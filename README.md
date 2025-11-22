# HydraOS

A custom x86 operating system built from scratch in C++ and Assembly.

## 🎯 Current Status

HydraOS is a functional operating system with interactive shell, multitasking, and memory management!

### ✅ Completed Features

- **Bootloader Integration**: GRUB-based bootloader with multiboot support
- **Global Descriptor Table (GDT)**: Memory segmentation and protection
- **Interrupt Handling**: Full interrupt descriptor table (IDT) with hardware and software interrupt support
- **VGA Graphics Driver**: Text mode with 16 colors, cursor control, scrolling
- **Memory Management**: 10MB heap allocator with malloc/free and coalescing
- **Timer (PIT) Driver**: 100 Hz programmable interval timer for timing operations
- **Multitasking**: Cooperative task scheduler with round-robin scheduling
- **Interactive Shell**: Command-line interface with 7 built-in commands
- **Hardware Drivers**:
  - Full keyboard driver with complete scancode mapping
  - PS/2 Mouse driver with cursor movement
  - Port I/O abstraction layer
- **Driver Framework**: Extensible driver manager for hardware initialization

## 🏗️ Architecture

### Project Structure

```
hydraOS/
├── include/           # Header files
│   ├── common/       # Common types and utilities
│   ├── drivers/      # Driver interfaces
│   └── hardwarecommunication/  # Hardware abstraction
├── src/              # Implementation files
│   ├── drivers/      # Driver implementations
│   └── hardwarecommunication/  # Hardware communication
├── Makefile          # Build configuration
└── linker.ld         # Linker script
```

## 🚀 Building & Running

### Prerequisites

- GCC cross-compiler (i686-elf-gcc) or 32-bit GCC
- GNU Make
- NASM or GNU Assembler
- GRUB tools (grub-mkrescue)
- QEMU or VirtualBox for testing

### Build Commands

```bash
# Build the kernel
make

# Create bootable ISO
make mykernel.iso

# Run in QEMU
make run

# Clean build artifacts
make clean
```

## �️ Shell Commands

The interactive shell supports the following commands:

- **help** - Show all available commands
- **clear** / **cls** - Clear the screen
- **echo [text]** - Print text to the screen
- **uptime** - Show system uptime
- **about** - Display HydraOS information
- **meminfo** - Show memory configuration
- **uname** - Display system information

## 📋 Development Roadmap

### ✅ Completed

1. ✅ **VGA Graphics Driver** - Enhanced text mode with colors and cursor control
2. ✅ **Memory Management** - Heap allocator with malloc/free
3. ✅ **Timer (PIT) Driver** - Time-based operations and scheduling
4. ✅ **Multitasking** - Cooperative task scheduler with round-robin scheduling
5. ✅ **Shell/CLI** - Interactive command-line interface

### 🎯 Next Priority

6. **System Calls Interface** - Kernel-user space communication (interrupt 0x80)
7. **ATA/IDE Hard Disk Driver** - Persistent storage access
8. **File System** - Basic file operations (FAT or custom)

### Future Enhancements

- Network stack (Ethernet, TCP/IP)
- Virtual memory management (paging)
- User-space applications
- GUI framework
- More device drivers (USB, Sound, etc.)

## 🔧 Development Workflow

### Branches

- **main**: Stable releases only
- **develop**: Active development branch (use this for features)
- **feature/\***: Feature-specific branches

### Contributing

1. Create feature branch from `develop`
2. Implement and test your feature
3. Submit pull request to `develop`

## 📖 Learning Resources

This OS is built following low-level systems programming concepts:

- x86 Architecture and Assembly
- Protected Mode and Memory Segmentation
- Interrupt Handling (PIC, IDT)
- Hardware I/O Programming
- Driver Development

## 📝 License

See LICENSE file for details.

## 🎓 Credits

Developed as a learning project to understand operating system internals and low-level programming.

---

**Current Branch**: develop  
**Last Updated**: November 22, 2025
