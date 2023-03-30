#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <dirent.h>
#include "arraylist.h"
#include "tok.h"
 
list_t parse(int fd);
void getDir();
void changeDir(char* dest);

void getDir(){
    char cwd[1024]; // buffer size
    //printf("here\n");
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, "\n");
    write (STDOUT_FILENO, cwd, strlen(cwd));
    // int chdir(char *pathname), returns 0 if successful;
    // char *getcwd(char *buf, size_t size);
}

void changeDir(char* dest){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    char* dir = strcat(cwd, "/");
    char* to = strcat(dir, dest);
    if (chdir(to) != 0){
        perror("cd failed");
    }
}

int main(int argc, char **argv){
    char buf[100];
    char *msgg="mysh> ";
    char *msg="parameters:\n";
    char *mmsg="command: ";
    char *omsg="goodbye!\n";
    char *exit = "exit";
    char *nl = "\n";
    char * s = " ";
    char *init =  "Welcome to MyShell!\n";
    char *pwd = "pwd";
    char *cd = "cd";
    int err = 0;
    write (STDOUT_FILENO, init, strlen(init));


    //batch mode
    // if(argc > 1){

    //     char cmd[100];
    //     char *envp[] = {(char *) "PATH=/bin", 0 };

    //     int z = 0;
    //     int l = al_length(&commands);
    //     char* command = al_lookup(&commands, 0);
    //     char* in = NULL;
    //     char* out = NULL;
    //     int indicator = 0;

    //     return;
    // }

    //interactive Mode
    while (1) {
        if (err != 0) break;

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

        int z = 0;
        int l = al_length(&commands);
        char* command = al_lookup(&commands, 0);
        char* in = NULL;
        char* out = NULL;
        int indicator = 0;

        
        //write (STDOUT_FILENO, mmsg, strlen(mmsg));
        //write (STDOUT_FILENO, command, strlen(command));
        //write (STDOUT_FILENO, nl, strlen(nl));

        list_t parameters;
        al_init(&parameters, 16);
        //char* parameters[l];
        char* cur;
        int i = 0;
        //write (STDOUT_FILENO, msg, strlen(msg));
        while((cur = al_lookup(&commands, i))){
            //printf("here\n");
            if (!strcmp(cur, "<")){
                //printf("redirect input\n");
                indicator = 1;
                i++;
                continue;
            }
            if (!strcmp(cur, ">")){
                //printf("redirect output\n");
                indicator = -1;
                i++;
                continue;
            }
            if (indicator != 0){
                if(indicator == 1){
                    in = cur;
                    indicator = 0;
                    i++;
                    continue;
                }
                else if(indicator == -1){
                    out = cur;
                    indicator = 0;
                    i++;
                    continue;
                }
            }

            al_push(&parameters, cur);
            //parameters[i] = al_lookup(&commands, i);
            if (!strcmp(cur, exit)) {
                z =1;
                break;
            }
            //break for cases?
            if (!strcmp(cur, pwd)) {
                getDir();
                z = 2;
                break;
            }
            
            //write (STDOUT_FILENO, cur, strlen(cur));
            //write (STDOUT_FILENO, nl, strlen(nl));
            i++;
        }

        //confirm parameters//in/out
        
        char** param = al_data(&parameters);
        int pl = al_length(&parameters);
        printf("confirm parameters\n");
        for(int i = 0; i < pl; i++){
            printf("%s\n", param[i]);
        }
        
        if (in != NULL) printf("final input will be: %s\n", in);
        else printf("no in\n");
        if (out != NULL) printf("final output will be: %s\n", out);
        else printf("no out\n");
        
      
        if (z ==1) break;
        if (z == 2) continue;
        //WILL NOT WORK ONCE WE START IMPLEMENTING SUBCOMMANDS
        // -need to figure out how to append NULL to end of command list if encounter another command
        

        //temp call to cd - need to change!!
        al_push(&parameters, NULL);
        if(!strcmp(command, cd)) {
            changeDir(al_lookup(&parameters, 1));
            al_destroy(&commands);
            continue;
        }
        
        if(fork() != 0) {   //Parent
            wait NULL;      //Wait for child
        }
        else{
            
            if(out != NULL){
                //int saved = dup(STDOUT_FILENO);
                int file = open(out, O_WRONLY|O_CREAT, 0640);
                if (file == -1){
                    perror("couldn't open file\n");
                }
                
                if (dup2(file, STDOUT_FILENO) == -1){
                    perror("couldn't dup\n");
                }
            }

            if(in != NULL){
                //int saved = dup(STDOUT_FILENO);
                int file = open(in, O_RDONLY);
                if (file == -1){
                    perror("couldn't open file\n");
                }
                
                if (dup2(file, STDIN_FILENO) == -1){
                    perror("couldn't dup\n");
                }
            }

            strcpy(cmd, "/bin/");
            strcat(cmd, command);
            if (execve (cmd, al_data(&parameters), envp) == -1){ //execute command
                perror("could not execute");
                err = 1;
            } 
        }
        
        al_destroy(&commands);
    }

    if(err != 0) printf("!");
    else write (STDOUT_FILENO, omsg, strlen(omsg));
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
    al_push(&commands, NULL);
    
    return commands;
}