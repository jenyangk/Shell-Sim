#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int forking(char *execPath, char **execArg, char *filename, int redirect, int background);
int forkingPipe(char **execArgsA, char **execArgsB, char **execPath);
void tokenize(char *str, const char *delim, char **argv);

int cd(char **args)
{
    int count = 0;
    // Check is the next argument for cd exists
    if (args[++count] != 0)
    {
        if (chdir(args[count]) == -1)
        {
            perror("dragonshell");
            return -1;
        }
    }
    else
    {
        printf("dragonshell: expected argument to \"cd\"\n");
        return -1;
    }
    return 0;
}

int pwd(char **args)
{
    char s[PATH_MAX];
    if (getcwd(s, 100) != NULL)
    {
        printf("%s\n", getcwd(s, sizeof(s)));
    }
    else
    {
        perror("dragonshell");
    }
    return 0;
}

int execute(char **args, char **path, int *count)
{
    // Variables for checking redirection, background process
    int redirect = 0;
    int background = 0;
    int backCounter = 0;

    // Execution Path (e.g. /bin/ls)
    char execPath[100];

    // filename for redirection
    char filename[50];

    // Allocate memory for execution arguments and user defined path command
    char **execArgs = malloc(100 * sizeof(char *));
    char **userDefComm = malloc(100 * sizeof(char *));

    // Count the number of arguments for execve
    int i = 0;

    // Stores the first argument
    char firstArg[50];

    while (args[*count] != NULL)
    {
        // Store the first argument to append to path
        if (i == 0)
        {
            // Memory issue (catches "")
            if (strcmp("", args[*count]) == 0)
            {
                *count = *count + 1;
            }
            strcpy(firstArg, args[*count]);
            strcpy(execPath, firstArg);
            tokenize(firstArg, "/", userDefComm);
            while (userDefComm[backCounter] != NULL)
            {
                backCounter++;
            }
        }
        // Listen for Redirection
        if (strcmp(">", args[*count]) == 0 && args[*count + 1] != NULL)
        {
            redirect = 1;
            // Don't add the redirect and file argument into the argument list
            *count = *count + 1;            // done ">"
            strcpy(filename, args[*count]); // filename = filename in argument
            *count = *count + 1;            // done "filename"
        }
        // Check for amperstamp
        else if (strcmp("&", args[*count]) == 0)
        {
            background = 1;
            *count = *count + 1;
        }
        else
        {
            // Add other arguments for execve
            execArgs[i] = args[*count];
            i++;
            *count = *count + 1;
        }
    }
    // NULL termination for last argument
    execArgs[i] = NULL;

    //USER-DEFINED PATH
    if (backCounter > 1)
    {
        // Correct the first execution argument
        strcpy(execArgs[0], userDefComm[backCounter - 1]);
        memset(userDefComm, 0, 100 * sizeof(char *));
        if (forking(execPath, execArgs, filename, redirect, background) == 0)
        {
            // Only one try
            free(userDefComm);
            free(execArgs);
            return 1;
        }
        // Only one try but FAILED
        free(userDefComm);
        free(execArgs);
        return 0;
    }
    else
    { // Don't need userDefComm
        free(userDefComm);
        // We try different paths for execution
        int j = 0;
        // PATH ENVIRONMENT
        while (path[j] != NULL)
        {
            strcpy(execPath, path[j]);
            strcat(execPath, firstArg);
            j++;
            // Success execution
            if (forking(execPath, execArgs, filename, redirect, background) == 0)
            {
                // DONE (Clear Memory)
                free(execArgs);
                return 1;
            }
            // Fail try another path.
        }
        // All path failed try "./"
        strcpy(execPath, "./");
        strcat(execPath, firstArg);
        // Success execution
        if (forking(execPath, execArgs, filename, redirect, background) == 0)
        {
            // DONE (Clear Memory)
            free(execArgs);
            return 1;
        }
    }
    // Fail (Clear Memory)
    memset(execArgs, 0, 100 * sizeof(char *));
    memset(userDefComm, 0, 100 * sizeof(char *));
    free(userDefComm);
    free(execArgs);
    return 0;
}

void signal_callback_handler(int signum)
{
    printf("\n");
}
