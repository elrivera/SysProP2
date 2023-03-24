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
    printf("Welcome to MyShell!\n");
    
    while (1) {
        write (STDOUT_FILENO, msgg, strlen(msgg));
        /*
        int n;
        n=read (STDIN_FILENO, buf, sizeof(buf));
        write (STDOUT_FILENO, msg, strlen(msg));
        
        write (STDOUT_FILENO, buf, n);
        */
        
        list_t commands = parse(STDIN_FILENO);
        //do stuff//
        int j = size(&commands);
        if (j == 0) continue;
        for(int i = 0; i < j; i++){
            char* p = al_lookup(&commands, i);
            write (STDOUT_FILENO, p, strlen(p));
        }
        
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

list_t parse(int fd){
    list_t commands;
    al_init(&commands, 8);

    //printf("here\n");

    char* tok;
    //printf("4here\n");
    tok_init(fd);
    //printf("5here\n");
    
    while((tok = next_tok())){
        //printf("2here\n");
        printf("%s\n", tok);
        al_push(&commands, strdup(tok));
        free(tok);
    }
    
    printf("3here\n");

    return commands;
}