# HydraOS Filesystem Usage Guide

## Overview

HydraOS includes a simple inode-based filesystem that supports basic file operations. The filesystem uses the ATA/IDE driver to persist data on a virtual disk.

## Prerequisites

- A 10MB virtual disk image (`disk.img`) must exist
- The Makefile will automatically create it if missing

## Filesystem Workflow

### Step 1: Format the Filesystem

Before using the filesystem for the first time, you must format it:

```
format
```

This initializes the superblock, inode table, and prepares the disk for use.

### Step 2: Mount the Filesystem

After formatting (or on each boot), mount the filesystem:

```
mount
```

This loads the filesystem metadata from disk into memory.

### Step 3: Use File Operations

Once mounted, you can perform file operations:

#### Create a File

```
touch myfile.txt
```

#### Write to a File

```
write myfile.txt
```

Note: Currently writes test data. In future versions, this will accept user input.

#### List Files

```
ls
```

#### Display File Contents

```
cat myfile.txt
```

#### Delete a File

```
rm myfile.txt
```

## Complete Example Session

```
HydraOS Shell v0.1
$ format
Formatting file system...
File system formatted successfully!

$ mount
Mounting file system...
File system mounted successfully!

$ touch test.txt
File 'test.txt' created.

$ write test.txt
Wrote 52 bytes to 'test.txt'

$ ls
Files:
  test.txt (52 bytes)
Total: 1 file(s)

$ cat test.txt
Hello from HydraOS filesystem!
This is a test file.

$ rm test.txt
File 'test.txt' deleted.

$ ls
Files:
  (empty)
Total: 0 file(s)
```

## Technical Details

### Filesystem Structure

- **Sector 0**: Unused (boot sector)
- **Sector 1**: Superblock (magic: 0x48594452 "HYDR")
- **Sectors 2-9**: Inode table (64 inodes, 8 per sector)
- **Sector 10+**: Data sectors

### Limitations

- Maximum 64 files
- Maximum filename length: 32 characters
- Simple linear sector allocation (no fragmentation handling)
- No directory support (flat structure only)
- No permissions or timestamps (basic fields exist but unused)

### File Types

- `INODE_TYPE_EMPTY`: Unused inode
- `INODE_TYPE_REGULAR`: Regular file
- `INODE_TYPE_DIRECTORY`: Directory (not yet implemented)

## Error Messages

### "Failed to create file"

**Cause**: Filesystem not mounted or disk full
**Solution**: Run `format` then `mount`

### "Failed to mount file system. Try 'format' first."

**Cause**: Disk not formatted or corrupted
**Solution**: Run `format`

### "File not found"

**Cause**: Trying to read/write/delete a non-existent file
**Solution**: Use `ls` to list files, or `touch` to create

## Future Enhancements

- Interactive write command with user input
- Directory support
- File permissions
- Timestamps using PIT
- Defragmentation
- Larger file support with indirect blocks
- File size limits based on available sectors

## Author

Shubham Phapale (2025)
