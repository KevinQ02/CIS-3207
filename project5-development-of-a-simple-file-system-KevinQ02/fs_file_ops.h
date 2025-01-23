#ifndef FS_FILE_OPS_H
#define FS_FILE_OPS_H

#include <stddef.h>  
#include <sys/types.h>  
#include "fs_core.h"
//File System Functions

int fs_open(char *name);
int fs_close(int fildes);
int fs_create(char *name);
int fs_delete(char *name);
int fs_read(int fildes, void *buf, size_t nbyte);
int fs_write(int fildes, void *buf, size_t nbyte);
int fs_get_filesize(int fildes);
int fs_lseek(int fildes, off_t offset);
int fs_truncate(int fildes, off_t length);

#endif
