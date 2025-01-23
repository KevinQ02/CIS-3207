#include <stdio.h>      
#include <unistd.h>     
#include "utils.h"
#include "file_manager.h"
#include "logger.h"

int main() {
    const char* source = "testfiles.tar.gz";
    const char* destination = "./unpacked_files";
    int num_workers = 4;
    int pipes[num_workers][2];
    pid_t workers[num_workers];

    printf("Starting decompression...\n");
    decompress_source_file(source, destination);
    printf("Decompression done.\n");

    printf("Creating pipes...\n");
    create_pipes(num_workers, pipes);
    printf("Pipes created.\n");

    printf("Creating worker processes...\n");
    create_workers(num_workers, pipes, workers);
    printf("Worker processes created.\n");

    printf("Distributing files...\n");
    distribute_files(destination, num_workers, pipes);
    printf("Files distributed.\n");

    printf("Sending shutdown signals...\n");
    send_shutdown_signals(num_workers, pipes);
    printf("Shutdown signals sent.\n");

    printf("Collecting progress reports...\n");
    collect_progress_reports(num_workers, pipes);
    printf("Progress reports collected.\n");

    printf("Cleaning up worker processes...\n");
    cleanup_workers(num_workers, workers);
    printf("All worker processes cleaned up.\n");

    log_event("All files have been processed.");
    return 0;
}
