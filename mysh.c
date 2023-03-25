#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "arraylist.h"
#include "tok.h"
 
list_t parse(int fd);

int main(){
    char buf[100];
    char *msgg="mysh> ";
    char *msg="you wrote:";
    char *omsg="goodbye!\n";
    char *q = "q";
    char *nl = "\n";
    char * s = " ";
    printf("Welcome to MyShell!\n");
    
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
        

        //temp code to made sure commands are reaching arraylist here (they are)
        int j = size(&commands);
        if (j == 0) continue;
        for(int i = 0; i < j; i++){
            char* p = al_lookup(&commands, i);
            write (STDOUT_FILENO, p, strlen(p));
            write (STDOUT_FILENO, s, strlen(s));
        }
        write (STDOUT_FILENO, nl, strlen(nl));

        //do stuff//
    }
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

    //printf("here\n");

    char* tok;
    //printf("4here\n");
    tok_init(fd);
    //printf("5here\n");
    
    //call tokenizer and fill arraylist
    while((tok = next_tok())){
        //printf("2here\n");
        //printf("%s\n", tok);
        al_push(&commands, strdup(tok));
        free(tok);
    }
    
    //printf("3here\n");

    return commands;
}