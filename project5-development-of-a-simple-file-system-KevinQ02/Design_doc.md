## Design Document
# a) Volume Layout (Space Allocation)
    • virtual disk of 8,192 blocks
    • each block is 4KB in size 
    
    The file system arrangment:
    - Block 0: Superblock
            Stores metadata about the filesystem layout, including the location of the directory, FAT, & data region
    - Block 1: Root Directory
            Stores up to 64 file entries. Each file entry includes a filename, file size, head block index, & metadata
    - Blocks 2 - 5: File Allocation Table (FAT)
            The FAT is 4 blocks * 4096 bytes = 16,384 bytes. Each FAT entry is 4 bytes, so theirs 16,384 / 4 = 4,096 FAT entries. Each entry corresponds to one data block
            Special values in the FAT:
                - 0 for a free block
                - -1 (FAT_EOF) for end of file chain
    -  Blocks 6 - 4101: Data Region (4,096 data blocks)
            Store the data of the file. Each FAT entry maps one data block. The max file size is 16MB (4,096 blocks * 4KB)
     
# b) Structure of the Physical Directory (Space Allocation)
    The root directory is stored in a single block (Block 1). And holds an array of 64 FileInfo structures

    Each FileInfo structure contains:
        - used: checks if entry is active 
        - name: filename (up to 15 chars + null terminator) 
        - size: current file size in bytes 
        - head: FAT index of the first data block, or FAT_EOF if empty 
        - num_blocks: how many data blocks this file currently uses 
        - fd_count: how many file descriptors are currently open on this file

# c) Method of Space Allocation Including Management of Free Blocks
    The FAT manages free blocks. All FAT entries are set to 0 (FAT_FREE) except the ones being used
    - When file needs a new data block, we scan the FAT for an entry == 0
    - We set it to FAT_EOF & link it into the file’s chain by changing the previous tail block’s FAT entry from FAT_EOF to the new block index
    - On file deletion or truncation, we follow the FAT chain from the file’s head & set each used entry back to 0 (FAT_FREE) until we reach FAT_EOF

    This manages free space. The FAT acts like a linked list of blocks for each file

# d) Logical Directory Structure
    the directory is an array of files with no subdirectories. Each FileInfo record corresponds to one file. Filenames are unique within this directory
    To find a file, we scan through file_table in memory searching for used == true  &  name == filename 

# e) Relationship Between the Logical & Physical Directories
    - Physical Directory: A single block on disk (Block 1) that has an array of FileInfo 
    - Logical Directory: In-memory representation (file_table) of these entries after mounting
    They're one-to-one. Each FileInfo entry on disk corresponds to one entry in file_table. On mount_fs(), we read the directory block into file_table. On umount_fs(), we write file_table back to disk

# f) Clear Description of Functions
    Management Functions 
    - make_fs(disk_name): Creates a new empty filesystem, writes superblock, directory, FAT to disk
    - mount_fs(disk_name): Opens the disk, reads superblock, directory, & FAT into memory, making the filesystem ready for operations
    - umount_fs(disk_name): Writes all changes back to disk & closes the disk

    File System Functions
    - fs_create(name): Adds a new file to the directory with zero size & no data blocks
    - fs_delete(name): Removes a file from the directory & frees its blocks. Fails if the file is open
    - fs_open(name): Opens a file & returns a file descriptor
    - fs_close(fildes): Closes an open file descriptor
    - fs_read(fildes, buf, nbyte): Reads up to nbyte bytes from the file at the current offset into buf, advancing the offset
    - fs_write(fildes, buf, nbyte): Writes up to nbyte bytes to the file at the current offset, extending the file if needed, & advances the offset
    - fs_get_filesize(fildes): Returns the file’s size in bytes
    - fs_lseek(fildes, offset): Sets the file’s offset
    - fs_truncate(fildes, length): Truncates the file to length bytes, freeing any extra blocks

    These functions all rely on the in-memory file_table, fd_table, & fat arrays, modifying them & writing changes back to disk on umount_fs() 

    Helper Functions :
    - write_fat_to_disk(): Writes the in-memory FAT array to the FAT region on the disk
    - read_fat_from_disk(): Reads the FAT blocks from the disk into the in-memory fat array
    - find_file_index(name): Searches the in-memory file_table for a file matching name. Returns the file’s index if found, or -1 if not found
    - find_free_fd(): Scans the fd_table for an unused file descriptor slot and returns its index, or -1 if all are in use
    - allocate_data_block_for_file(file): Finds a free FAT entry corresponding to a free data block, links it into the file’s FAT chain, updates file->head or the last block’s FAT entry, and increases file->num_blocks
    - get_data_block_for_file(file, block_index): Given a zero-based block index within the file, follows the file’s FAT chain starting from file->head to find the disk block number. Returns -1 if the block does not exist
    - free_blocks(start_block): Follows the FAT chain starting at start_block, freeing each block (setting its FAT entry to FAT_FREE) until it reaches FAT_EOF

    these helper functions help with implementing the logic needed by the public API. While not called directly, they help with file block allocation, file lookup, and maintenance of filesystem consistency


SOURCES:
https://github.com/masum035/file-system-implementation-in-xv6/blob/main/README.md
https://github.com/masum035/file-system-implementation-in-xv6/tree/main
https://stackoverflow.com/questions/4358728/end-of-file-eof-in-c
https://stackoverflow.com/questions/1361560/simple-virtual-filesystem-in-c-c
https://stackoverflow.com/questions/2652545/extern-and-global-in-c
https://stackoverflow.com/questions/5859707/rationale-for-system-calls-that-allow-request-of-size-t-but-result-of-only-ssize
