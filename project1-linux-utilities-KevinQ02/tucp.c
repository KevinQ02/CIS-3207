#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define BUFFER_SIZE 1024

// Helper function to check if the path is a directory
int is_directory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) {
        return 0;  // Error occurred, not a directory
    }
    return S_ISDIR(statbuf.st_mode);
}

// Function to copy a single file
void copy_file(const char *source, const char *destination) {
    int src_fd, dest_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Open source file for reading
    if ((src_fd = open(source, O_RDONLY)) < 0) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    // Open or create destination file for writing
    if ((dest_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
        perror("Error opening/creating destination file");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    // Copy data from source to destination
    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        if (write(dest_fd, buffer, bytes_read) != bytes_read) {
            perror("Error writing to destination file");
            close(src_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read < 0) {
        perror("Error reading from source file");
    }

    close(src_fd);
    close(dest_fd);
}

// Main function
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <source_file(s)> <destination>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *dest = argv[argc - 1];  // The last argument is the destination

    // If the destination is a directory, copy each source file to the directory
    if (is_directory(dest)) {
        for (int i = 1; i < argc - 1; i++) {
            // Build the destination path (directory + filename)
            char dest_path[BUFFER_SIZE];
            snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, strrchr(argv[i], '/') ? strrchr(argv[i], '/') + 1 : argv[i]);
            copy_file(argv[i], dest_path);
         }
    } else {
        // If the destination is a file, copy only one source file to the destination
        if (argc > 3) {
            fprintf(stderr, "Error: Multiple source files and a single destination file is not allowed.\n");
            exit(EXIT_FAILURE);
        }
        copy_file(argv[1], dest);
    }

    return 0;
}
