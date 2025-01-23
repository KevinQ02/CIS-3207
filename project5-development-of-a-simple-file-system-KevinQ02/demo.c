#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "fs_core.h"
#include "fs_file_ops.h"

int main() {
    char *disk_name = "mydisk";

    //(a) Create a disk volume
    assert(make_fs(disk_name) == 0);
    assert(mount_fs(disk_name) == 0);

    //(b) Create a file
    assert(fs_create("test.txt") == 0);

    //(c) Open and Write data to the created file; then close the file
    int fd = fs_open("test.txt");
    assert(fd >= 0);

    char write_data[] = "Hello, File System!";
    int written = fs_write(fd, write_data, strlen(write_data));
    assert(written == (int)strlen(write_data));

    assert(fs_close(fd) == 0);

    //Unmount to simulate ending a session
    assert(umount_fs(disk_name) == 0);

    //Mount again to simulate another process/session
    assert(mount_fs(disk_name) == 0);

    //(d) Another process/thread reads data from the file and verifies it
    fd = fs_open("test.txt");
    assert(fd >= 0);
    //Read from various offsets
    //First read 5 bytes
    char read_buf[128];
    memset(read_buf, 0, sizeof(read_buf));
    assert(fs_read(fd, read_buf, 5) == 5);
    assert(strncmp(read_buf, "Hello", 5) == 0);
    printf("Read first 5 bytes: %s\n", read_buf);

    //Seek to offset 7 (start of "File System!")
    assert(fs_lseek(fd, 7) == 0);
    memset(read_buf, 0, sizeof(read_buf));
    assert(fs_read(fd, read_buf, 4) == 4);
    assert(strncmp(read_buf, "File", 4) == 0);
    printf("Read 'File' at offset 7: %s\n", read_buf);

    //Check entire file by reading from start
    assert(fs_lseek(fd, 0) == 0);
    memset(read_buf, 0, sizeof(read_buf));
    int read_bytes = fs_read(fd, read_buf, sizeof(read_buf));
    assert(read_bytes == (int)strlen(write_data));
    assert(strcmp(read_buf, write_data) == 0);
    printf("Full read: %s\n", read_buf);

    //(e) Copy the file to a new file (different file name)
    assert(fs_create("copy.txt") == 0);
    int fd_copy = fs_open("copy.txt");
    assert(fd_copy >= 0);

    //Rewind original file
    assert(fs_lseek(fd, 0) == 0);

    //Read original fully and write to copy
    memset(read_buf, 0, sizeof(read_buf));
    read_bytes = fs_read(fd, read_buf, sizeof(read_buf));
    assert(read_bytes == (int)strlen(write_data));

    int copy_written = fs_write(fd_copy, read_buf, read_bytes);
    assert(copy_written == read_bytes);

    fs_close(fd);
    fs_close(fd_copy);

    //Delete the original file
    assert(fs_delete("test.txt") == 0);

    //Show that directory updated and space freed by creating another file
    assert(fs_create("another.txt") == 0);
    int fd_another = fs_open("another.txt");
    assert(fd_another >= 0);
    const char *new_data = "This is another file.";
    assert(fs_write(fd_another, (void*)new_data, strlen(new_data)) == (int)strlen(new_data));
    fs_close(fd_another);

    //Unmount final time
    assert(umount_fs(disk_name) == 0);

    printf("Demonstration completed successfully.\n");
    return 0;
}
