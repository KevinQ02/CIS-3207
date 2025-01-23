#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

void distribute_files(const char* directory, int num_workers, int pipes[][2]);

#endif // FILE_MANAGER_H
