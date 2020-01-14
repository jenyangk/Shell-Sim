#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>

/**
 * @brief Tokenize a C string 
 * 
 * @param str - The C string to tokenize 
 * @param delim - The C string containing delimiter character(s) 
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */

void tokenize(char *str, const char *delim, char **argv)
{
    char *token;
    token = strtok(str, delim);
    for (size_t i = 0; token != NULL; ++i)
    {
        argv[i] = token;
        token = strtok(NULL, delim);
    }
}

int cd(char **arguments);
int pwd(char **arguments, int count);
int execute(char **arguments, char **path, int *count);
void signal_callback_handler(int signum);

int main(int argc, char **argv)
{
    // print the string prompt without a newline, before beginning to read
    // tokenize the input, run the command(s), and print the result
    // do this in a loop
    printf("Welcome to Dragonshell.\n");

    //PATH
    char **path = malloc(100 * sizeof(char *));
    char **processes = malloc(20 * sizeof(char *));
    char **arguments = malloc(20 * sizeof(char *));
    // Initial bin files
    path[0] = "/bin/";
    path[1] = "/usr/bin/";

    while (1)
    {
        // Initial Values
        char commands[100];

        // Prompt
        printf("dragonshell > ");

        // Signal
        struct sigaction sa;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        sa.sa_handler = SIG_IGN;
        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGTSTP, &sa, NULL);

        int i = 0;
        char character;
        while ((character = getchar()) != '\n')
        {
            if (character == EOF)
            {
                free(path);
                free(arguments);
                free(processes);
                printf("\ndradonshell: Goodbye!\n");
                return 0;
            }
            commands[i] = character;
            i++;
        }
        commands[i] = '\0';

        if (i == 0)
        {
            memset(arguments, 0, 20 * sizeof(char *));
            memset(processes, 0, 20 * sizeof(char *));
            continue;
        }
        else
        {
            // String array counter
            int countProcess = 0;

            // Check and Seperate for Multiprograms
            tokenize(commands, ";", processes);

            // Start running each process
            while (processes[countProcess] != NULL)
            {
                int count = 0;
                // Tokenize user input
                tokenize(processes[countProcess], " ", arguments);
                // Run user commands
                while (arguments[count] != NULL)
                {
                    // Exiting terminal
                    if (strcmp("exit", arguments[count]) == 0)
                    {
                        printf("dragonshell: Goodbye!\n");
                        free(path);
                        free(processes);
                        free(arguments);
                        return 0;
                    }
                    // Changing Work Directory
                    else if (strcmp("cd", arguments[count]) == 0 && arguments[count + 2] == NULL)
                    {
                        char *args[] = {arguments[count], arguments[++count], NULL};
                        int return_value = cd(args); // run the program with the filename you just read
                        if (return_value == -1)
                        {
                            break;
                        }
                        count++;
                    }
                    // Printing Work Directory
                    else if (strcmp("pwd", arguments[count]) == 0 && arguments[count + 1] == NULL)
                    {
                        pwd(arguments, count);
                        count++;
                    }

                    // Print PATH Environments
                    else if (strcmp("$PATH", arguments[count]) == 0 && arguments[count + 1] == NULL)
                    {
                        int i = 0;
                        printf("Current PATH: ");
                        while (path[i] != NULL)
                        {
                            printf("%s", path[i]);
                            if (path[++i] != NULL)
                            {
                                printf(":");
                            }
                        }
                        printf("\n");
                        count++;
                    }
                    // A2Path Function - Replacing PATH Environments
                    else if (strcmp("a2path", arguments[count]) == 0)
                    {
                        // Check if the user enter "$PATH:"
                        if (arguments[++count] != NULL && strncmp("$PATH:", arguments[count], 6) == 0)
                        {
                            char **paths = malloc(20 * sizeof(char *));
                            char string[50];
                            strcpy(string, arguments[count]);
                            path = calloc(100, sizeof(char *));
                            // Tokenize the new paths
                            tokenize(string, ":", paths);
                            // Add to the path array
                            int i = 1, j = 0;
                            // Replace new path
                            while (paths[i] != NULL)
                            {
                                path[j] = paths[i];
                                i++;
                                j++;
                            }
                            free(paths);
                        }
                        else
                        {
                            printf("dragonshell: Your specified PATH might be wrong.\n");
                        }
                        count++;
                    }
                    // BUG: Catch 'nothing'
                    else if (strcmp("", arguments[count]) == 0)
                    {
                        count++;
                    }
                    // Try programs in /bin/ or /usr/bin/ or other specified
                    else
                    {
                        if (execute(arguments, path, &count) == 0)
                        {
                            // Tried executing on PATHS
                            printf("dragonshell: Command not found!\n");
                        }
                    }
                }
                countProcess++;
                memset(arguments, 0, 20 * sizeof(char *));
            }
        }
        memset(processes, 0, 20 * sizeof(char *));
    }
    free(arguments);
    free(processes);
    free(path);
    return 0;
}