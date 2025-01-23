#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// help
void shell_help() {
    printf("Available commands:\n");
    printf("help - Displays this message\n");
    printf("exit - Exits the shell\n");
    printf("pwd  - Prints the current working directory\n");
    printf("cd   - Changes the current directory to the specified path\n");
}

// exit
void shell_exit() {
    exit(0);
}

// pwd
void shell_pwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd() error");
    }
}

// cd
void shell_cd(char *path) {
    if (path == NULL || strcmp(path, "~") == 0) {
        path = getenv("HOME");
    }
    if (chdir(path) != 0) {
        perror("cd failed");
    }
}

// Execute external commands
void execute_external_command(char *cmd, char **params, int background) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(cmd, params);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        if (!background) {
            waitpid(pid, NULL, 0);
        } else {
            printf("[Running in background] PID: %d\n", pid);
        }
    } else {
        perror("fork failed");
    }
}

// Execute commands with multiple pipes
void execute_with_pipes(char *commands[][100], int num_commands, int background) {
    int pipes[num_commands - 1][2];

    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe failed");
            return;
        }
    }

    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Set up input from previous pipe if not the first command
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            // Set up output to next pipe if not the last command
            if (i < num_commands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close all pipes
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(commands[i][0], commands[i]);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork failed");
            return;
        }
    }

    // Close all pipes in the parent
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all child processes if not running in the background
    if (!background) {
        for (int i = 0; i < num_commands; i++) {
            wait(NULL);
        }
    }
}

// Execute command with redirection or pipes
void execute_command(char *args[], int background) {
    int fd_in = -1, fd_out = -1;
    int pipe_indexes[10];
    int pipe_count = 0;

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            fd_out = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0) {
                perror("open failed");
                return;
            }
            args[i] = NULL;
        } else if (strcmp(args[i], "<") == 0) {
            fd_in = open(args[i + 1], O_RDONLY);
            if (fd_in < 0) {
                perror("open failed");
                return;
            }
            args[i] = NULL;
        } else if (strcmp(args[i], "|") == 0) {
            pipe_indexes[pipe_count++] = i;
            args[i] = NULL;
        }
    }

    if (pipe_count > 0) {
        // Handle multiple pipes
        char *commands[pipe_count + 1][100];
        int start = 0;

        for (int i = 0; i <= pipe_count; i++) {
            int j = 0;
            for (int k = start; args[k] != NULL; k++) {
                commands[i][j++] = args[k];
            }
            commands[i][j] = NULL;
            start = pipe_indexes[i] + 1;
        }
        execute_with_pipes(commands, pipe_count + 1, background); // Updated to use execute_with_pipes
    } else {
        // Handle redirection or normal command
        pid_t pid = fork();
        if (pid == 0) {
            if (fd_in != -1) {
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            if (fd_out != -1) {
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            execvp(args[0], args);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            if (!background) {
                waitpid(pid, NULL, 0);
            } else {
                printf("[Running in background] PID: %d\n", pid);
            }
        } else {
            perror("fork failed");
        }
    }
}

int main() {
    char input[1024]; // Buffer for user input
    while (1) {
        printf("myshell> ");
        fflush(stdout); // Ensure the prompt is displayed immediately

        if (!fgets(input, sizeof(input), stdin)) break; // Read a line from stdin
        input[strcspn(input, "\n")] = 0; // Remove newline character

        // Tokenization for handling redirection and pipes
        char *tokens[100];
        int num_tokens = 0, background = 0;
        char *token = strtok(input, " ");
        while (token) {
            if (strcmp(token, "&") == 0) { // Handle background process
                background = 1;
            } else {
                tokens[num_tokens++] = token;
            }
            token = strtok(NULL, " ");
        }
        tokens[num_tokens] = NULL;

        if (num_tokens == 0) continue; // Skip empty commands

        // Handling built-in commands
        if (strcmp(tokens[0], "exit") == 0) {
            shell_exit();
        } else if (strcmp(tokens[0], "help") == 0) {
            shell_help();
        } else if (strcmp(tokens[0], "pwd") == 0) {
            shell_pwd();
        } else if (strcmp(tokens[0], "cd") == 0) {
            shell_cd(tokens[1]);
        } else {
            // External command execution handling with redirection and pipes
            execute_command(tokens, background); // Updated to call execute_command
        }
    }
    return 0;
}