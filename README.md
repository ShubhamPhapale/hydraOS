# HydraOS

A custom x86 operating system built from scratch in C++ and Assembly.

## 🎯 Current Status

HydraOS is a work-in-progress operating system with the following implemented features:

### ✅ Completed Features

- **Bootloader Integration**: GRUB-based bootloader with multiboot support
- **Global Descriptor Table (GDT)**: Memory segmentation and protection
- **Interrupt Handling**: Full interrupt descriptor table (IDT) with hardware and software interrupt support
- **Hardware Drivers**:
  - Keyboard driver with event handling
  - PS/2 Mouse driver with cursor movement
  - Port I/O abstraction layer
- **Driver Framework**: Extensible driver manager for hardware initialization
- **Basic Video Output**: Text-mode VGA output via direct memory access

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

## 📋 Development Roadmap

### High Priority (Next Features)

1. **VGA Graphics Driver** - Enhanced text mode with colors and cursor control
2. **Memory Management** - Heap allocator with malloc/free
3. **System Calls Interface** - Kernel-user space communication
4. **Timer (PIT) Driver** - Time-based operations and scheduling

### Medium Priority

5. **ATA/IDE Hard Disk Driver** - Persistent storage access
6. **File System** - Basic file operations (FAT or custom)
7. **Multitasking** - Process scheduler and task switching
8. **Shell/CLI** - Command-line interface for user interaction

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
