# Project-3-F24  
**A Concurrent File Compression Tool**  

This project showcases how different processes work together to achieve parallel execution while communicating through pipes.  

**Project Overview:**  
The goal of this project is to create a tool that can compress multiple files simultaneously, with each file being handled by a separate process. The main process will distribute files to worker processes, which will then compress them. Each worker will send progress updates back to the main process via pipes.  

**Key Features and Overview:**  
1. **Main Process**:  
    - Spawns multiple worker processes.  
    - Distributes the files to be compressed across worker processes.  
    - Monitors the progress of workers and handles synchronization and work completion.  

2. **Worker Processes**:  
    - Receive work requests from the Main process via a pipe  
    - Each worker compresses a single file at a time.  
    - Communicates the completion status back to the main process using pipes.  

3. **Inter-Process Communication (IPC)**:  
    - The main process communicates with worker processes via pipes.  
    - Pipes are used to send both file information and progress reports.  

4. **Error Handling**:  
    - Workers report errors (e.g., file not found or compression failure) through pipes.  
    - The main process can manage these errors, by logging the error.  

5. **Performance Monitoring**:  
    - As files are compressed in parallel, this can help demonstrate the performance improvements of using multiple processes over a single-threaded approach.  

**Project Description**     
The application you develop begins as a single process (the **Main** process). The **Main** process creates all the necessary pipes for communication and creates the worker processes.    

There are two pipes for communication with each worker from the **Main** process. One pipe is used to send information to the worker. One pipe is used to receive information from the worker.  

Once all the program components are in place, the **Main** process opens the directory containing the source files to be compressed. The **Main** process then gets the name of the first file in the directory and sends the file path to a worker process. The **Main** process continues to acquire file names and pass them to available worker processes. When all files in the directory have been sent to workers for processing, the **Main** process sends a termination file name to each worker.  

**Worker Process Activities**  
There are to be 4 **worker** processes.  
Each **worker**process **executes in a loop**, reading from its input pipe the (path)name of a file to compress, then compresses the file. If an error occurs because of an illegal or non-existent file, an error message is returned via the output pipe to the **Main**process.   

If the file is found, it is processed by compressing it using ***Gzip***. When processing is complete, the worker sends completion status to the **Main**process through the output pipe. The worker then continues its loop, getting the next request for work. Worker termination occurs when it receives a shutdown ‘filename’ through the input pipe. In response to the shutdown request, the worker sends a ‘closing’ reply to the **Main**process through its output pipe, closes its open pipe ends, and terminates.  

**Main Process Activities**  
Some additional Main process activity is described here,  
- Logging of Worker Activity:   
  - The Main process will maintain a compression record for each file consisting of  
    - Name of file to compress  
    - ID of Worker file is sent to  
    - Time request is made to the worker.  
    - Time that Main is notified of completion by the worker. A processing error from the worker is also logged.  
- **Main** will log these compression records to a file.  

The **Main** process will continue to request file compression until it has completed work on all files in the input file directory. Once the request for compression of all files have been sent to worker processes, the **Main** process will send a shutdown file name to each worker process. The **Main** process will then wait for a completion message from each worker process. When all completion messages have been received, the **Main** process will flush and close the log file, close/delete all pipes, and terminate.  

**Source Files**  
You will be given a compressed file (testfiles.tar.gz) that contains the files that your process is to compress. Your **Main** process is to take the path name of this file and ***decompress*** this file into a local directory of your choice using **tar (**e.g. tar -xvcf testfiles.tar.gz) . These resulting files are the input to your program. The output compressed files from your program will be stored in a different directory of your choice.  

**Testing**  
Each file in your output directory should be able to be decompressed using **Gzip** back to the original source file.  

Your program can be tested by performing a content compare between each file in the source directory and a decompression of the corresponding file in your output directory.  

**Requirements**  
Your Main process will decompress the provided compressed tar.gz file to yield data files for your program  

Your Main process will create 4 worker processes.  
The Main process will log the worker activity records to a log file.  

The Main process will create 2 pipes for each of the 4 worker processes: one to send file path names to a worker and the other to receive work status from the worker process.  

The worker threads will compress files sent (via file name) to an output directory.  

**Steps in Building the Project:**  
1.        **Decompressing Source File**: to yield data files for compression by worker processes  
2.        **Process Creation**: Use fork() (in C) to create multiple processes for parallel execution.  
3.        **Pipe Setup**: Create pipes for communication between the main process and the worker processes.  
4.        **File Distribution**: Send the file names or paths from the main process to the workers via the pipes.  
5.        **Compression Task**: Each worker process compresses the assigned file (using the gzip utility)   
6.        **Progress Reporting**: Workers send updates on the compression process through the pipe (e.g., errors or completion status).  
7.        **Main Process Management**: The main process reads the progress reports from the pipes and logs them to a file.  

**Technologies To Use:**  
• **Languages**: C (using fork() and pipes)   
• **Compression Utility**: Gzip , tar  
• **Pipes and Multiprocessing**: Demonstrating how to use pipes for IPC.  
