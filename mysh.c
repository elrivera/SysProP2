#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "arraylist.h"
#include "tok.h"

int main(int argc, char **argv){
    printf("Welcome to MyShell!\n");
    
    while (1) {
        printf("mysh> ");
        list_t commands = parse(STDIN_FILENO);
        //do stuff//
    }
}

list_t parse(int fd){
    list_t commands;
    al_init(&commands, 8);

    char* tok;

    tok_init(fd);

    while((tok = next_tok())){
        al_push(&commands, strdup(tok));
        free(tok);
    }
    
    return commands;
}