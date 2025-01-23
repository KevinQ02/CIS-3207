#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fs_core.h"
#include "disk.h"

SuperBlock fs_super;
FileInfo file_table[MAX_FILE];
FileDescriptor fd_table[MAX_FILE_DESCRIPTOR];
int fat[DATA_BLOCK_COUNT];  // In-memory FAT
static int write_fat_to_disk();
static int read_fat_from_disk();

// Initialize the file system on the virtual disk
int make_fs(char *disk_name) {
    if (!disk_name) {
        fprintf(stderr, "DEBUG: make_fs - disk_name is NULL\n");
        return -1;
    }
    // Create a new disk
    if (make_disk(disk_name) != 0) {
        fprintf(stderr, "DEBUG: make_fs - make_disk failed\n");
        return -1;
    }
    // Open the newly created disk
    if (open_disk(disk_name) != 0) {
        fprintf(stderr, "DEBUG: make_fs - open_disk failed\n");
        return -1;
    }
    // Initialize superblock
    memset(&fs_super, 0, sizeof(fs_super));
    fs_super.dir_index = 1;   // start index for directory
    fs_super.dir_len = 0;     // initially no files
    fs_super.data_index = 6;  // start index for data blocks
    fs_super.fat_start = 2;   // FAT starts at block 2
    fs_super.fat_length = 4;  // 4 blocks for FAT

    // ERROR: Print superblock info
    fprintf(stderr, "DEBUG: make_fs - superblock: dir_index=%d, dir_len=%d, data_index=%d, fat_start=%d, fat_length=%d\n",
            fs_super.dir_index, fs_super.dir_len, fs_super.data_index, fs_super.fat_start, fs_super.fat_length);

    // Initialize directory and file table
    memset(file_table, 0, sizeof(file_table));

    // Initialize FAT to all free
    for (int i = 0; i < DATA_BLOCK_COUNT; i++) {
        fat[i] = FAT_FREE;
    }

    // Write superblock to disk
    char buf[BLOCK_SIZE];
    memset(buf, 0, BLOCK_SIZE);
    memcpy(buf, &fs_super, sizeof(SuperBlock));
    if (block_write(0, buf) != 0) {
        fprintf(stderr, "DEBUG: make_fs - block_write(superblock) failed\n");
        close_disk();
        return -1;
    }

    // Write empty directory to disk
    memset(buf, 0, BLOCK_SIZE);
    memcpy(buf, file_table, sizeof(FileInfo)*MAX_FILE);
    if (block_write(fs_super.dir_index, buf) != 0) {
        fprintf(stderr, "DEBUG: make_fs - block_write(directory) failed\n");
        close_disk();
        return -1;
    }

    // Write FAT to disk
    if (write_fat_to_disk() != 0) {
        fprintf(stderr, "DEBUG: make_fs - write_fat_to_disk failed\n");
        close_disk();
        return -1;
    }

    if (close_disk() != 0) {
        fprintf(stderr, "DEBUG: make_fs - close_disk failed\n");
        return -1;
    }

    printf("make_fs() called successfully.\n");
    return 0;
}

// Mount the file system
int mount_fs(char *disk_name) {
    if (!disk_name) {
        fprintf(stderr, "DEBUG: mount_fs - disk_name is NULL\n");
        return -1;
    }

    // ERROR: Check if disk_name file exists before opening
    fprintf(stderr, "DEBUG: mount_fs - Attempting to open_disk(%s)\n", disk_name);

    if (open_disk(disk_name) != 0) {
        fprintf(stderr, "DEBUG: mount_fs - open_disk failed for %s\n", disk_name);
        return -1;
    }

    // Read superblock 
    static char buf[BLOCK_SIZE];
    memset(buf, 0, BLOCK_SIZE);
    fprintf(stderr, "DEBUG: mount_fs - Attempting block_read(0) for superblock\n");
    if (block_read(0, buf) != 0) { // Read superblock
        fprintf(stderr, "DEBUG: mount_fs - block_read(superblock) failed\n");
        close_disk();
        return -1;
    }
    memcpy(&fs_super, buf, sizeof(SuperBlock));

    // ERROR: Print superblock info after reading
    fprintf(stderr, "DEBUG: mount_fs - superblock read: dir_index=%d, dir_len=%d, data_index=%d, fat_start=%d, fat_length=%d\n",
            fs_super.dir_index, fs_super.dir_len, fs_super.data_index, fs_super.fat_start, fs_super.fat_length);

    // Read directory using a temporary char buffer to avoid alignment issues
    static char dir_buf[BLOCK_SIZE];
    memset(dir_buf, 0, BLOCK_SIZE);
    memset(file_table, 0, sizeof(file_table));
    fprintf(stderr, "DEBUG: mount_fs - Attempting block_read(dir_index=%d)\n", fs_super.dir_index);
    if (block_read(fs_super.dir_index, dir_buf) != 0) {
        fprintf(stderr, "DEBUG: mount_fs - block_read(directory) failed\n");
        close_disk();
        return -1;
    }

    // Now copy the data from dir_buf into file_table
    memcpy(file_table, dir_buf, sizeof(FileInfo)*MAX_FILE);

    // Read FAT into memory
    fprintf(stderr, "DEBUG: mount_fs - Attempting read_fat_from_disk()\n");
    if (read_fat_from_disk() != 0) {
        fprintf(stderr, "DEBUG: mount_fs - read_fat_from_disk failed\n");
        close_disk();
        return -1;
    }

    // clear file descriptors in memory
    memset(fd_table, 0, sizeof(fd_table));
    printf("mount_fs() called successfully: File system mounted.\n");
    return 0;
}

int umount_fs(char *disk_name) {
    if (!disk_name) {
        fprintf(stderr, "DEBUG: umount_fs - disk_name is NULL\n");
        return -1;
    }

    char buf[BLOCK_SIZE];
    memset(buf, 0, BLOCK_SIZE);
    memcpy(buf, file_table, sizeof(FileInfo) * MAX_FILE);
    fprintf(stderr, "DEBUG: umount_fs - Writing directory back at block %d\n", fs_super.dir_index);
    if (block_write(fs_super.dir_index, buf) != 0) {
        fprintf(stderr, "DEBUG: umount_fs - block_write(directory) failed\n");
        close_disk();
        return -1;
    }

    memset(buf, 0, BLOCK_SIZE);
    memcpy(buf, &fs_super, sizeof(SuperBlock));
    fprintf(stderr, "DEBUG: umount_fs - Writing superblock back at block 0\n");
    if (block_write(0, buf) != 0) {
        fprintf(stderr, "DEBUG: umount_fs - block_write(superblock) failed\n");
        close_disk();
        return -1;
    }

    fprintf(stderr, "DEBUG: umount_fs - Writing FAT back to disk\n");
    if (write_fat_to_disk() != 0) {
        fprintf(stderr, "DEBUG: umount_fs - write_fat_to_disk failed\n");
        close_disk();
        return -1;
    }

    for (int i = 0; i < MAX_FILE_DESCRIPTOR; i++) {
        fd_table[i].used = false;
        fd_table[i].file = -1;
        fd_table[i].offset = 0;
    }

    if (close_disk() != 0) {
        fprintf(stderr, "DEBUG: umount_fs - close_disk failed\n");
        return -1;
    }

    printf("umount_fs() called successfully: file system [%s] unmounted.\n", disk_name);
    return 0;
}

// Helper function to write FAT to disk
static int write_fat_to_disk() {
    char buf[BLOCK_SIZE];
    int entries_per_block = BLOCK_SIZE / sizeof(int);
    int index = 0;

    fprintf(stderr, "DEBUG: write_fat_to_disk - Writing FAT blocks, fat_length=%d\n", fs_super.fat_length);

    for (int b = 0; b < fs_super.fat_length; b++) {
        memset(buf, 0, BLOCK_SIZE);
        memcpy(buf, &fat[index], entries_per_block * sizeof(int));

        fprintf(stderr, "DEBUG: write_fat_to_disk - block_write at block %d\n", fs_super.fat_start + b);

        if (block_write(fs_super.fat_start + b, buf) != 0) {
            fprintf(stderr, "DEBUG: write_fat_to_disk - block_write(FAT) failed at block %d\n", fs_super.fat_start + b);
            return -1;
        }
        index += entries_per_block;
    }

    return 0;
}

// Helper function to read FAT from disk
static int read_fat_from_disk() {
    char buf[BLOCK_SIZE];
    int entries_per_block = BLOCK_SIZE / sizeof(int);
    int index = 0;

    fprintf(stderr, "DEBUG: read_fat_from_disk - Reading FAT blocks, fat_length=%d\n", fs_super.fat_length);

    for (int b = 0; b < fs_super.fat_length; b++) {
        fprintf(stderr, "DEBUG: read_fat_from_disk - block_read at block %d\n", fs_super.fat_start + b);
        if (block_read(fs_super.fat_start + b, buf) != 0) {
            fprintf(stderr, "DEBUG: read_fat_from_disk - block_read(FAT) failed at block %d\n", fs_super.fat_start + b);
            return -1;
        }
        memcpy(&fat[index], buf, entries_per_block * sizeof(int));
        index += entries_per_block;
    }

    return 0;
}
