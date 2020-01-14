# Shell-Sim ( A shell simulator in C)

## Design:

Firstly, the when the program launches, the user will be prompted with a simple greeting to let the user know that the shell has already launched.

The shell is contained within a while loop that runs forever. Since the user can only exit the program when `exit` or `CTRL+D` is entered, the while loop can ensure that the user is always within the shell unless told to exit.



The user will then enter a string of command(s).

Since multiple commands are typically isolated from each other, we can run the commands seperately.

After capturing the user input, `;` will be used as a delimiter to tokenize different commands. This is to allow the user to run multiple processes within a single line After tokenizing, the different commands will be stored in a `char**` array and each command in the array will run individually. In the case that the user only has one commands, then only that command will be ran.

In a single command, the command will be tokenized with `“ ”`, seperating the different  arguments into a `char**` array. To keep track of which argumentsAt several `if/else if` statements, we then check whether the first argument matches `cd, pwd, $PATH, a2path or exit`. If non-matches, we then try to execute the tokenized command. If the execution fails, we let the user know that the command cannot be ran (prompted Command not found!).

Within each of the `if/else if`, we check if the user entered an extra invalid command. For example, if the user enters `cd .. pwd` should not run. For built-in commands, each command will only accept 1 or none number of arguments, if this were to exceed, then the program will not run.



When running the execution command, the entire command will be sent to the `execution` function in `functions.c`. The execution function will then check whether the program has any redirection or sending to background. If redirection is needed, we open a file descriptor to redirect the output of a program. If background is needed, our parent process will not wait for the child to execute finish and might even redirect the output to `/dev/null` so that the message will not be output in `stdout`. After execution, the function will return 1 for success (completed execution) or 0 for fail (fail execution).

-----

## Implemented Features:

### Built-in Commands:

1. `cd` 
   - System call used: `chdir`
   - The `cd` function only takes the `cd` itself and a file/path specified after it (e.g. `cd ..` or `cd folderA/`). It only uses two arguments provided by the user and the rest of the user commands will reiterate through the loop again.
     Thus, if the user enters `cd .. ..`, the shell will run `cd ..` and return `Command not found!` when it encounter `..`. This command is tested by running commands such as `cd ..`,  `cd folder/`, `cd`, `cd non_existent_folder`.
2. `pwd`
   - System call used: none
   - The `pwd` function takes no other argument from the user.
     Typing `pwd` will print the current working directory.
3. Showing the path and `a2path` command
   - System call used: none
   - When the user enters `$PATH`, the shell prints the PATH environments stores in the `PATH` String Array.
   - When the user enters `a2path`, the user must also enter the `$PATH:(followed by the path)` ; if the user did not and entered this portion (`$PATH:`) incorrectly.
4. Exit
   - System call used: none
   - When the user enters `exit`, the `exit` function will free return 0. Thus, finishing the program. When the user enters `CTRL+D`, the `getchar()` will catch it and will break off the `while (1)` loop, ultimately, ending the program.

### Running External Programs

- System call used: `execv()`, `exit()`, `pid()`, `fork()`, `waitpid()`
- The user can either enter a path to a program (e.g. `/usr/bin/whereis`), a program that exists in the path environment (`$PATH`) or a program that exists in the working directory (e.g. `dragonshell`). Each of the commands should returns its respective outputs or run certain programs. The user can also add arguments after specifying the path_to_program/program (e.g. `ls -al`) and it should return specific outputs or run certain a process.
  If the command does not exists within the working directory, specified directory or PATH environment, the command will not run.
- The command is tested using `ls`, `whereis gcc`, `/usr/bin/whereis gcc`, `dragonshell`, `false_command`.

### Output Redirection

- System call used: `open()`, `close()`, `dup()`
- The output redirection command `>` is used with an external program (e.g. `ls`). When the user enters `ls >`, the user has to specify a filename (could be `a` or `a.txt`); if the user does not, the command will return an error.
  Note: This command cannot be used with built-in commands.
- The command is tested using `ls >`, `ls > a`, `ls -al > a`, `whereis gcc > a`

### Piping

This function has not been implemented.

### Multiple Commands

- System call used: none
- The `;` is used to seperate different commands to be run within a single line. The user should put `;` between two seperate commands, for example, `cd .. ; pwd`. The two commands are then run one by one.

### Handling Signals

- System call used: none
- When the user enters `CTRL+C` or `CTRL+Z`, the terminal should print `C^` or `Z^` respectively. Tested by entering `CTRL+C` or `CTRL+D`. Eventhough prompted in the shell, the shell will not catch any of it.

### Putting Jobs in Background

- System call used: `dup()`
- When the user enters `&` at the end of a command, the process will be sent to background and user should be back at the terminal. When sending the process to background, the PID of the child process will be prompt. This command is tested by entering the `&` at the back of a command.

-----

## Issues

Piping has not been implemented.

-----

## Sources

Char * vs Char **:

https://stackoverflow.com/questions/779910/should-i-use-char-argv-or-char-argv-in-c

*By Johannes Schaub - litb, answered at April 23, 2009*

*Edited by Arno van Wyk, June 15, 2018*



Portions of this project are modifications based on work created and shared by the Lab Instructors:

System Call Usages and Makefile:

[*Lab 1*](https://eclass.srv.ualberta.ca/mod/resource/view.php?id=3813244) and [*Lab 2*](https://eclass.srv.ualberta.ca/mod/resource/view.php?id=3824339)



Portions of this project are modifications based on work created and shared by the Wikimedia Project and used according to terms described in the Creative Commons Attribution - ShareAlike License:

String Manipulation:

https://en.wikibooks.org/wiki/C_Programming/String_manipulation

*By Wikibooks, last edited September 15 2019*



FILE/IO:

https://en.wikibooks.org/wiki/C_Programming/Stream_IO

*By Wikibooks, last edited July 4 2019*



Memory Management:

https://en.wikibooks.org/wiki/C_Programming/Memory_management

*By Wikibooks, last edited June 18 2018*

