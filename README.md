# C-Shell
A C program which serves as a shell interface that accepts user commands and then executes each command in a seperate process. My implementation supports input and output redirection, as well as pipes as a form of IPC between pairs of commands. Some system calls used in implementation are
fork(), execvp(), wait(), dup2(), and pipe().

The shell interface gives the user a prompt 'mysh:~$' after which the next command is entered by the user.

The functionalities of the shell which can be used are:
1) '!!' - which is a history feature to allow the user to execute the most recent command entered. For example, if a user enters the command ls -l, they can then execute that command again by entering !! at the prompt. Any command executed in this fashion should be echoed on the user’s screen, and the command should also be placed in the history buffer as the next command. Your

2) '<' or '>' - which are the redirection operators where > redirects the output of a command to a file and < redirects the input to a command from a file.  For example, if a user enters mysh:~$ ls > outFile.txt the output from the ls command will be redirected to the file outFile.txt. Similarly, input can be redirected as well. For example, if the user enters mysh:~$ sort < inFile.txt the file inFile.txt will serve as input to the sort command. Managing the redirection of both input and output will involve using the dup2() function, which duplicates an existing file descriptor to another file descriptor. For example, if fd is a file descriptor to the file outFile.txt, the call dup2(fd, STDOUT FILENO); duplicates fd to standard output (the terminal).

3) '|' - The pipe operator allows the output of one command to serve as input to another command. For example, the following command sequence mysh:~$ ls -l | less has the output of the command ls -l serve as the input to the less command. Both the ls and less commands will run as separate processes and will communicate using the UNIX pipe() function.
