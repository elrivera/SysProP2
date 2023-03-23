#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "arraylist.h"
#include "tok.h"

//parse through input, split into tokens, and put into arraylist
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

int main(int argc, char *argv[]) { 
    list_t arr = parse(STDIN_FILENO);
    int j = size(&arr);
    for(int i = 0; i < j; i++){
        char* p = al_lookup(&arr, i);
        printf("%s\n", p);
    }
    return EXIT_SUCCESS;
}