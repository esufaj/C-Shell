#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>

#define MAX_LINE		80 /* 80 chars per line, per command */

int main(void)
{
    char *args[MAX_LINE/2 + 1];/* command line (of 80) has max of 40 arguments */
    char *reargs[MAX_LINE/2 +1];
    char string[MAX_LINE/2 +1]; /* char array to store commands from the user*/
    char lastCMND[MAX_LINE/2 +1];
    int should_run = 1;
    int numCMNDS = 0; // for history purposes. 
    bool breaking = false; // check whether exit was inputted
   




    while (should_run){

        printf("mysh:~$ ");
        fflush(stdout); // flush standard input
        fgets(string, MAX_LINE/2 +1, stdin); //--------getting user input from the cmmnd line


        //if it does not contain !!
        if(strstr(string, "!!") == NULL){

            strcpy(lastCMND , string); //updating the lastCommand constantly.
            numCMNDS++;

        }
        else if(strstr(string, "!!") != NULL){ //if it contains !! replace input with the last command

            if(numCMNDS == 0){

                printf("No commands in history.\n");

            }
            else{

                strcpy(string , lastCMND);

            }
        }

        int i = 0;  // index of args array
        char *split = strtok(string," \n");

        while(split != NULL){

            args[i] = split;
            split = strtok(NULL, " \n");
            i++;

        }
        args[i] = '\0';


// checking for exit, return, and cd

        if(strcmp(args[0], "exit") == 0){

            should_run = 0;
            breaking = true;
            exit(0);

        }
        if(strcmp(args[0], "return") == 0){

            return 0;

        }
        if(strcmp(args[0], "cd") == 0){

            if(chdir(args[1]) != 0){

                perror("error");

            }
            continue;
        }

        // checking whether or not < or > was seen in the input and setting variables

        int r, fdIN, fdOUT;
        bool signREIN = false, signREOUT = false; 
        char fileIN[64], fileOUT[64];

        for(r = 0; args[r] != NULL; r++){

            if(strcmp(args[r], "<") == 0){

                args[r] = NULL;
                strcpy(fileIN, args[r + 1]);
                args[r+1] = NULL;
                signREIN = true;

            }

        }
        for(r = 0; args[r] != NULL; r++){

            if(strcmp(args[r], ">") == 0){

                args[r] = NULL;
                strcpy(fileOUT, args[r + 1]);
                args[r+1] = NULL;
                signREOUT = true;

            }
        }
        //printf("%s\n", fileOUT);

        // printf("%s\n", args[0]);
        //printf("%s %s %s %s %s %s\n", args[0], args[1], args[2], args[3], args[4], args[5]); 
        //checking output.

        // checking if pipe was seen in the input

        int x;
        bool signPIPE = false;
        char *CMD1[64], *CMD2[64];
        int cmd1i = 0, cmd2i = 0;

        for(x = 0; args[x] != NULL; x++){

            if(strcmp(args[x], "|") == 0){ //CMD2

                signPIPE = true;
                continue;

            }

            if(signPIPE == true){

                CMD2[cmd2i++] = args[x]; //assign cmd2 after we find pipe

            }else {//CMD1

                CMD1[cmd1i++] = args[x];//assign cmd1 before we find pipe

            }
        }
        //null termiante both cmd1 and cmd2
        CMD1[cmd1i] = NULL;
        CMD2[cmd2i] = NULL;

      //  printf("Testing for pipe: %s %s | %s %s \n",CMD1[0], CMD1[1], CMD2[0], CMD2[1]);



// setting execvp, forks, childs, redirections, and pipes as well as some error checking

        if(breaking == false){

            pid_t pid = fork();

            if(pid < 0){

                perror("Failed to fork!!\n");
                return 0;

            }
            else if(pid == 0){

// redirection starts here
                if(signREIN){

                    int fdIN;

                    if((fdIN = open(fileIN, O_RDONLY)) < 0){

                        perror("Could not open input files");
                        exit(0);

                    }
                    dup2(fdIN, STDIN_FILENO);
                    close(fdIN);
                    signREIN = false;
                }

                if(signREOUT) {

                    int fdOUT;

                    if((fdOUT = open(fileOUT, O_CREAT | O_WRONLY | O_TRUNC)) < 0){

                        perror("Could not open output files");
                        exit(0);

                    }
                    dup2(fdOUT, STDOUT_FILENO);
                    close(fdOUT);
                    signREOUT = false;

                }

// piping starts here
                int fileDIS[2];
                pid_t childPID, childPID2;

                if(signPIPE){

                    if(pipe(fileDIS) != 0){

                        perror("failed to create pipe");

                    }
                    if((pid = fork()) == -1){

                        perror("failed to fork");

                    }
                    if(pid == 0){

                        dup2(fileDIS[1],STDOUT_FILENO);
                        close(fileDIS[0]);
                        close(fileDIS[1]);
                        execvp(CMD1[0],CMD1);
                        perror("failed to execute command");

                    } else{
                        
                        dup2(fileDIS[0],STDIN_FILENO);
                        close(fileDIS[0]);
                        close(fileDIS[1]);
                        execvp(CMD2[0],CMD2);
                        perror("failed to execute command");
                        
                    }
                    signPIPE = false;
	                wait(NULL);

                    exit(0);
                }


//execution of regular execvp commands
                execvp(args[0],args);
                perror("execvp");
                _exit(1);

            } else {

                wait(NULL);
                //exit(0);
            }
        }

    }

    return 0;

}
