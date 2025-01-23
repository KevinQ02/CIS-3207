#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>

void decompress_source_file(const char* source, const char* destination);
void create_pipes(int num_workers, int pipes[][2]);
void create_workers(int num_workers, int pipes[][2], pid_t* workers);
void worker_process(int read_fd, int write_fd);
void compress_file(const char* filepath, int write_fd);  
void cleanup_workers(int num_workers, pid_t* workers);
void send_shutdown_signals(int num_workers, int pipes[][2]);
void collect_progress_reports(int num_workers, int pipes[][2]);

#endif // UTILS_H
