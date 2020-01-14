#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int execute(char **args, char **path, int *count);

// Returns 0 = Done/Success, -1 = Fail/Error
int forking(char *execPath, char **execArgs, char *filename, int redirect, int background)
{
    int pid;
    char *env[] = {NULL};
    // Create child process
    if ((pid = fork()) == -1)
        perror("fork error");
    else if (pid == 0)
    {
        // Child Process
        int file_desc = 0;
        // Create a file descriptor for stdout
        if (redirect == 1)
        {
            int file_desc = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
            if (file_desc < 0)
            {
                printf("dragonshell: Error opening file.\n");
            }
            dup2(file_desc, 1);
        }
        if (background == 1 && redirect == 0)
        {
            // Throw the output to void
            int file_desc = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
            if (file_desc < 0)
            {
                printf("dragonshell: Error opening file.\n");
            }
            dup2(file_desc, 1);
        }
        if (execve(execPath, execArgs, env) < 0)
        {
            // Exit with code 66 if error.
            exit(66);
        }
        if (redirect == 1)
        {
            close(file_desc);
        }
        exit(0);
    }
    else
    {
        // Parent Process
        if (background == 1)
        {
            // Make it dont print anything
            printf("PID %d is running in the background.\n", pid);
            // Done
            return 0;
        }
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            int exitCode = WEXITSTATUS(status);
            // If child execve has error.
            if (exitCode == 66)
            {
                // Fail
                return -1;
            }
        }
        return 0;
    }
    // Fork fail
    return -1;
}
