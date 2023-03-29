#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include "arraylist.h"
#include "tok.h"
 
list_t parse(int fd);

int main(){
    char buf[100];
    char *msgg="mysh> ";
    char *msg="parameters:\n";
    char *mmsg="command: ";
    char *omsg="goodbye!\n";
    char *exit = "exit";
    char *nl = "\n";
    char * s = " ";
    char *init =  "Welcome to MyShell!\n";
    write (STDOUT_FILENO, init, strlen(init));


    //HAVE TO IMPLEMENT CD & PWD OURSELVES
    void getDir(){
        char cwd[1024]; // buffer size
        getcwd(cwd, sizeof(cwd));

        // int chdir(char *pathname), returns 0 if successful;
        // char *getcwd(char *buf, size_t size);
    }
    
    
    while (1) {
        //write mysh prompt
        write (STDOUT_FILENO, msgg, strlen(msgg));

        /*
        int n;
        n=read (STDIN_FILENO, buf, sizeof(buf));
        write (STDOUT_FILENO, msg, strlen(msg));
        
        write (STDOUT_FILENO, buf, n);
        */
        
        //create arraylist of commands
        list_t commands = parse(STDIN_FILENO);
        
        /*
        //temp code to made sure commands are reaching arraylist
        int j = size(&commands);
        if (j == 0) continue;
        int z;
        for(int i = 0; i < j; i++){
            char* p = al_lookup(&commands, i);
            if (!strcmp(p, q)) {
                z =1;
                break;
            }
            write (STDOUT_FILENO, p, strlen(p));
            write (STDOUT_FILENO, nl, strlen(nl));
        }
        if (z ==1) break;
        */

        //do stuff//
       
        //Now we have to go through the commands and execute them
        //special cases we have to account for:
        //     "|"    -  Sub Command
        //  "<" & ">" -  File redirect
        //     "*"    -  Wildcards
        
        char cmd[100];
        char *envp[] = {(char *) "PATH=/bin", 0 };

        int z;
        int l = al_length(&commands);
        char* command = al_lookup(&commands, 0);
        if (!strcmp(command, exit)) {
                break;
            }
        //write (STDOUT_FILENO, mmsg, strlen(mmsg));
        //write (STDOUT_FILENO, command, strlen(command));
        //write (STDOUT_FILENO, nl, strlen(nl));

        char* parameters[l];
        //write (STDOUT_FILENO, msg, strlen(msg));
        for(int i = 0; i < l; i++){
            parameters[i] = al_lookup(&commands, i);
            if (!strcmp(parameters[i], exit)) {
                z =1;
                break;
            }
            //break for cases?

            //write (STDOUT_FILENO, parameters[i], strlen(parameters[i]));
            //write (STDOUT_FILENO, nl, strlen(nl));
        }
        if (z ==1) break;
        //WILL NOT WORK ONCE WE START IMPLEMENTING SUBCOMMANDS
        // -need to figure out how to append NULL to end of command list if encounter another command
        parameters[l] = NULL;
        
        if(fork() != 0) {   //Parent
            wait NULL;      //Wait for child
        }
        else{
            strcpy(cmd, "/bin/");
            strcat(cmd, command);
            execve (cmd, parameters, envp); //execute command
        }
        

    }

    write (STDOUT_FILENO, omsg, strlen(omsg));
    //return EXIT_SUCCESS;

}

/*
void read_input(){
    int n;
    char buf[1024];
    int buflength = 128;
    while((n = read(STDIN_FILENO, buf, )) > 0)
}
*/

//take input and compose into arrayList
list_t parse(int fd){

    list_t commands;
    al_init(&commands, 16);
    char* tok;
    tok_init(fd);
    
    //call tokenizer and fill arraylist
    while((tok = next_tok())){
        al_push(&commands, strdup(tok));
        free(tok);
    }
    
    return commands;
}