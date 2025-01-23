#ifndef FS_CORE_H
#define FS_CORE_H

#include "disk.h"
#include <stdbool.h>
#include <sys/types.h>

#define MAX_FILENAME_LEN    15
#define MAX_FILE_DESCRIPTOR 32
#define MAX_FILE            64


#define DATA_BLOCK_COUNT 4096 // # of data blocks for FAT
#define FAT_FREE 0
#define FAT_EOF  -1

typedef struct {
    int dir_index;      // Directory start index
    int dir_len;        // Number of files in the directory
    int data_index;     // Data block start index
    
    int fat_length;   // number of blocks the FAT spans
    int fat_start;    // FAT start block index
} SuperBlock;

typedef struct {
    bool used;                     // whether the file is being used
    char name[MAX_FILENAME_LEN+1];   // file name
    int size;                      // file size in bytes
    int head;                      // index of the first data block
    int num_blocks;                // number of data blocks used by the file
    int fd_count;                  // number of open file descriptors for this file
} FileInfo;

typedef struct {
    bool used;          // whether the file descriptor is being used
    int file;           // index to the file_table
    int offset;         // read/write offset in the file
} FileDescriptor;

// Global filesystem structures
extern SuperBlock fs_super;
extern FileInfo file_table[MAX_FILE];
extern FileDescriptor fd_table[MAX_FILE_DESCRIPTOR];
extern int fat[DATA_BLOCK_COUNT]; // In-memory FAT
// Function prototypes for filesystem operations
int make_fs(char *disk_name);
int mount_fs(char *disk_name);
int umount_fs(char *disk_name);



#endif // FS_CORE_H
