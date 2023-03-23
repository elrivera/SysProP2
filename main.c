#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "arraylist.h"
#include "tok.h"

int main(){
    //arraylist tests
    list_t commands;
    al_init(&commands, 8);

    char* str = "hello";
    char* str2 = "helloo";
    char* str3 = "hellooo";
    char* res;

    char* one = "one";

    

    al_push(&commands, str);
    al_push(&commands, str2);
    al_push(&commands, str3);
    res = al_lookup(&commands, 0);
    printf("%s\n", res);
    al_insert(&commands, 0, one);
    res = al_lookup(&commands, 0);
    printf("%s\n", res);
    res = al_pop(&commands);
    res = al_pop(&commands);
    //if(al_pop(res, &commands)){
        printf("%s\n", res);
    //}
    //else printf("fail\n");

    return EXIT_SUCCESS;
}