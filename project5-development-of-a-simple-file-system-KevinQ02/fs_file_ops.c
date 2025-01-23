#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fs_core.h"
#include "disk.h"
#include <inttypes.h>


// Helper functions 
static int find_file_index(const char *name);
static int find_free_fd();
static int allocate_data_block_for_file(FileInfo *file);
static int get_data_block_for_file(FileInfo *file, int block_index);
static void free_blocks(int start_block);
static int min(int a, int b) { return (a < b) ? a : b; }

int fs_open(char *name) {
    // DEBUG:
    fprintf(stderr, "DEBUG: fs_open('%s') called\n", (name ? name : "NULL"));

    // Ensure the file name is not NULL 
    if (!name || strlen(name) == 0) {
        fprintf(stderr, "fs_open error: Invalid file name.\n");
        return -1;
    }

    int file_index = find_file_index(name);
    if (file_index == -1) {
        fprintf(stderr, "fs_open error: File '%s' not found.\n", name);
        return -1;
    }

    int fd = find_free_fd();
    if (fd == -1) {
        fprintf(stderr, "fs_open error: Maximum number of file descriptors reached.\n");
        return -1;
    }

    fd_table[fd].used = true;
    fd_table[fd].file = file_index;
    fd_table[fd].offset = 0;

    file_table[file_index].fd_count++;
    fprintf(stderr, "DEBUG: fs_open - '%s' opened with fd %d\n", name, fd);
    return fd;
}

int fs_close(int fildes) {
    // DEBUG:
    fprintf(stderr, "DEBUG: fs_close(%d) called\n", fildes);

    if (fildes < 0 || fildes >= MAX_FILE_DESCRIPTOR || !fd_table[fildes].used) {
        fprintf(stderr, "fs_close error: Invalid file descriptor %d.\n", fildes);
        return -1;
    }

    int file_index = fd_table[fildes].file;
    fd_table[fildes].used = false;

    file_table[file_index].fd_count--;
    if (file_table[file_index].fd_count < 0)
        file_table[file_index].fd_count = 0; // safety check

    fprintf(stderr, "DEBUG: fs_close - fd %d closed\n", fildes);
    return 0;
}

int fs_create(char *name) {
    // DEBUG:
    fprintf(stderr, "DEBUG: fs_create('%s') called\n", (name ? name : "NULL"));

    //file name length
    if (!name || strlen(name) > MAX_FILENAME_LEN) {
        fprintf(stderr, "fs_create error: Invalid or too long name.\n");
        return -1;
    }

    if (find_file_index(name) != -1) {
        fprintf(stderr, "fs_create error: File '%s' already exists.\n", name);
        return -1;
    }

    int empty_slot = -1;
    for (int i = 0; i < MAX_FILE; i++) {
        if (!file_table[i].used) {
            empty_slot = i;
            break;
        }
    }

    if (empty_slot == -1) {
        fprintf(stderr, "fs_create error: Maximum number of files reached.\n");
        return -1;
    }

    file_table[empty_slot].used = true;
    strncpy(file_table[empty_slot].name, name, MAX_FILENAME_LEN);
    file_table[empty_slot].name[MAX_FILENAME_LEN] = '\0';
    file_table[empty_slot].size = 0;
    file_table[empty_slot].head = FAT_EOF;
    file_table[empty_slot].num_blocks = 0;
    file_table[empty_slot].fd_count = 0;
    fs_super.dir_len++;

    fprintf(stderr, "DEBUG: fs_create - '%s' created at index %d\n", name, empty_slot);
    return 0;
}

int fs_delete(char *name) {
    // DEBUG:
    fprintf(stderr, "DEBUG: fs_delete('%s') called\n", (name ? name : "NULL"));

    if (name == NULL) {
        fprintf(stderr, "fs_delete error: Invalid file name.\n");
        return -1;
    }

    int file_index = find_file_index(name);
    if (file_index == -1) {
        fprintf(stderr, "fs_delete error: File '%s' not found.\n", name);
        return -1;
    }

    if (file_table[file_index].fd_count > 0) {
        fprintf(stderr, "fs_delete error: File '%s' is currently open.\n", name);
        return -1;
    }

    if (file_table[file_index].head != FAT_EOF) {
        free_blocks(file_table[file_index].head);
    }

    memset(&file_table[file_index], 0, sizeof(FileInfo));
    file_table[file_index].used = false;
    fs_super.dir_len--;

    fprintf(stderr, "DEBUG: fs_delete - '%s' deleted\n", name);
    return 0;
}

int fs_read(int fildes, void *buf, size_t nbyte) {
    // DEBUG:
    fprintf(stderr, "DEBUG: fs_read(fd=%d, nbyte=%zu) called\n", fildes, nbyte);

    if (fildes < 0 || fildes >= MAX_FILE_DESCRIPTOR || !fd_table[fildes].used || nbyte <= 0) {
        fprintf(stderr, "fs_read error: Invalid fd or nbyte.\n");
        return -1;
    }

    FileDescriptor *fd = &fd_table[fildes];
    FileInfo *file = &file_table[fd->file];

    if (fd->offset >= file->size) {
        fprintf(stderr, "DEBUG: fs_read - EOF reached, returning 0\n");
        return 0;
    }

    int remaining = file->size - fd->offset;
    int to_read = (int)((nbyte > (size_t)remaining) ? remaining : nbyte);
    int read_count = 0;

    while (read_count < to_read) {
        int block_index = (fd->offset / BLOCK_SIZE);
        int block_offset = fd->offset % BLOCK_SIZE;
        
        int disk_block = get_data_block_for_file(file, block_index);
        if (disk_block < 0) {
            fprintf(stderr, "fs_read error: Could not get data block.\n");
            return read_count;
        }

        char block[BLOCK_SIZE];
        if (block_read(disk_block, block) != 0) {
            fprintf(stderr, "fs_read error: block_read failed at block %d\n", disk_block);
            return read_count;
        }

        int bytes_in_block = BLOCK_SIZE - block_offset;
        int can_copy = min(to_read - read_count, bytes_in_block);
        memcpy((char*)buf + read_count, block + block_offset, can_copy);

        fd->offset += can_copy;
        read_count += can_copy;
    }

    fprintf(stderr, "DEBUG: fs_read - read_count=%d\n", read_count);
    return read_count;
}

int fs_write(int fildes, void *buf, size_t nbyte) {
    // DEBUG:
    fprintf(stderr, "DEBUG: fs_write(fd=%d, nbyte=%zu) called\n", fildes, nbyte);

    if (fildes < 0 || fildes >= MAX_FILE_DESCRIPTOR || !fd_table[fildes].used || nbyte == 0) {
        fprintf(stderr, "fs_write error: Invalid fd or nbyte.\n");
        return -1;
    }

    FileDescriptor *fd = &fd_table[fildes];
    FileInfo *file = &file_table[fd->file];
    char *src = (char*)buf;

    size_t write_count = 0;
    size_t remaining = nbyte;

    while (remaining > 0) {
        int block_index = fd->offset / BLOCK_SIZE;
        int block_offset = fd->offset % BLOCK_SIZE;

        int disk_block = get_data_block_for_file(file, block_index);
        if (disk_block < 0) {
            // allocate a new block
            disk_block = allocate_data_block_for_file(file);
            if (disk_block < 0) {
                fprintf(stderr, "fs_write error: No free blocks.\n");
                break;
            }
        }

        char block[BLOCK_SIZE];
        if (block_offset != 0 || remaining < BLOCK_SIZE) {
            if (block_read(disk_block, block) != 0) {
                fprintf(stderr, "fs_write error: block_read failed at block %d\n", disk_block);
                return -1;
            }
        } else {
            memset(block, 0, BLOCK_SIZE);
        }

        int space_in_block = BLOCK_SIZE - block_offset;
        int to_write = (remaining < (size_t)space_in_block) ? (int)remaining : space_in_block;
        memcpy(block + block_offset, src + write_count, to_write);

        if (block_write(disk_block, block) != 0) {
            fprintf(stderr, "fs_write error: block_write failed at block %d\n", disk_block);
            return -1;
        }

        fd->offset += to_write;
        write_count += to_write;
        remaining -= to_write;
    }

    if (fd->offset > file->size) {
        file->size = fd->offset;
    }

    fprintf(stderr, "DEBUG: fs_write - write_count=%zu\n", write_count);
    return (int)write_count;
}

int fs_get_filesize(int fildes) {
    // DEBUG:
    fprintf(stderr, "DEBUG: fs_get_filesize(fd=%d) called\n", fildes);

    if (fildes < 0 || fildes >= MAX_FILE_DESCRIPTOR || !fd_table[fildes].used) {
        fprintf(stderr, "fs_get_filesize error: invalid fd.\n");
        return -1;
    }
    int size = file_table[fd_table[fildes].file].size;
    fprintf(stderr, "DEBUG: fs_get_filesize - size=%d\n", size);
    return size;
}

int fs_lseek(int fildes, off_t offset) {
    // DEBUG:
    fprintf(stderr, "DEBUG: fs_lseek(fd=%d, offset=%jd) called\n", fildes, (intmax_t)offset);

    if (fildes < 0 || fildes >= MAX_FILE_DESCRIPTOR || !fd_table[fildes].used) {
        fprintf(stderr, "fs_lseek error: invalid fd.\n");
        return -1;
    }

    FileInfo *file = &file_table[fd_table[fildes].file];
    if (offset < 0 || offset > file->size) {
        fprintf(stderr, "fs_lseek error: offset out of range.\n");
        return -1;
    }

    fd_table[fildes].offset = (int)offset;
    fprintf(stderr, "DEBUG: fs_lseek - offset set to %d\n", (int)offset);
    return 0;
}

int fs_truncate(int fildes, off_t length) {
    // DEBUG:
    fprintf(stderr, "DEBUG: fs_truncate(fd=%d, length=%jd) called\n", fildes, (intmax_t)length);

    if (fildes < 0 || fildes >= MAX_FILE_DESCRIPTOR || !fd_table[fildes].used || length < 0) {
        fprintf(stderr, "fs_truncate error: invalid parameters.\n");
        return -1;
    }

    FileInfo *file = &file_table[fd_table[fildes].file];
    if (length > file->size) {
        fprintf(stderr, "fs_truncate error: length greater than file size.\n");
        return -1;
    }

    if (length < file->size) {
        int old_num_blocks = file->num_blocks;
        int needed_blocks = (int)((length + BLOCK_SIZE - 1) / BLOCK_SIZE);
        if ((int)length == 0) needed_blocks = 0;

        if (needed_blocks < old_num_blocks) {
            int current = file->head;
            int prev = -1;
            for (int i = 0; i < needed_blocks; i++) {
                prev = current;
                current = fat[current];
            }

            if (current != FAT_EOF) {
                free_blocks(current);
            }

            if (needed_blocks == 0) {
                file->head = FAT_EOF;
            } else {
                fat[prev] = FAT_EOF;
            }

            file->num_blocks = needed_blocks;
        }

        file->size = (int)length;

        for (int i = 0; i < MAX_FILE_DESCRIPTOR; i++) {
            if (fd_table[i].used && fd_table[i].file == fd_table[fildes].file) {
                if (fd_table[i].offset > file->size) {
                    fd_table[i].offset = file->size;
                }
            }
        }
    }

    fprintf(stderr, "DEBUG: fs_truncate - truncated to length=%jd\n", (intmax_t)length);
    return 0;
}

// Helper functions
static int find_file_index(const char *name) {
    // DEBUG:
    fprintf(stderr, "DEBUG: find_file_index('%s') called\n", (name ? name : "NULL"));

    for (int i = 0; i < MAX_FILE; i++) {
        if (file_table[i].used && strcmp(file_table[i].name, name) == 0) {
            fprintf(stderr, "DEBUG: find_file_index - found at index %d\n", i);
            return i;
        }
    }
    fprintf(stderr, "DEBUG: find_file_index - not found\n");
    return -1;
}

static int find_free_fd() {
    // DEBUG:
    fprintf(stderr, "DEBUG: find_free_fd() called\n");

    for (int i = 0; i < MAX_FILE_DESCRIPTOR; i++) {
        if (!fd_table[i].used) {
            fprintf(stderr, "DEBUG: find_free_fd - found free fd %d\n", i);
            return i;
        }
    }
    fprintf(stderr, "DEBUG: find_free_fd - no free fd\n");
    return -1;
}

static int allocate_data_block_for_file(FileInfo *file) {
    // DEBUG:
    fprintf(stderr, "DEBUG: allocate_data_block_for_file() called\n");

    for (int i = 0; i < DATA_BLOCK_COUNT; i++) {
        if (fat[i] == FAT_FREE) {
            fat[i] = FAT_EOF;
            if (file->head == FAT_EOF) {
                file->head = i;
            } else {
                int current = file->head;
                while (fat[current] != FAT_EOF) {
                    current = fat[current];
                }
                fat[current] = i;
            }
            file->num_blocks++;
            int disk_block = fs_super.data_index + i;
            fprintf(stderr, "DEBUG: allocate_data_block_for_file - allocated block %d (fat idx=%d)\n", disk_block, i);
            return disk_block;
        }
    }
    fprintf(stderr, "DEBUG: allocate_data_block_for_file - no free blocks\n");
    return -1;
}

static int get_data_block_for_file(FileInfo *file, int block_index) {
    // DEBUG:
    fprintf(stderr, "DEBUG: get_data_block_for_file(block_index=%d) called\n", block_index);

    if (file->head == FAT_EOF) {
        fprintf(stderr, "DEBUG: get_data_block_for_file - file has no blocks\n");
        return -1;
    }

    int current = file->head;
    for (int i = 0; i < block_index; i++) {
        current = fat[current];
        if (current == FAT_EOF) {
            fprintf(stderr, "DEBUG: get_data_block_for_file - not enough blocks\n");
            return -1;
        }
    }
    int disk_block = fs_super.data_index + current;
    fprintf(stderr, "DEBUG: get_data_block_for_file - returning disk block %d for block_index %d\n", disk_block, block_index);
    return disk_block;
}

static void free_blocks(int start_block) {
    // DEBUG:
    fprintf(stderr, "DEBUG: free_blocks(start_block=%d) called\n", start_block);

    int current = start_block;
    while (current != FAT_EOF) {
        int next = fat[current];
        fat[current] = FAT_FREE;
        current = next;
    }

    fprintf(stderr, "DEBUG: free_blocks - blocks freed\n");
}
