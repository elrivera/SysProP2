#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "tok.h"
 
#ifndef BUFSIZE
#define BUFSIZE 128
#endif
static char buf[BUFSIZE];
static unsigned pos;
static unsigned bytes;
static int closed;
static int input;

void tok_init(int fd){

    input = fd;
    pos = 0;
    bytes = 0;
    closed = 0;
}

char *next_tok(void){

    if (closed) return NULL;
    // skip whitespace
    
    while (1) {
    // ensure we have a char to read
    if (pos == bytes) {
            //printf("read\n");
            bytes = read(STDIN_FILENO, buf, BUFSIZE);
            //printf("red\n");
            if (bytes < 1) {
                closed = 1;
                return NULL;
            }
            pos = 0;
        }
        if (buf[pos] == '\n') return NULL;
        if (!isspace(buf[pos])) break;
        ++pos;
    }

    // start reading a command
    int start = pos;
    char *tok = NULL;
    int toklen = 0;
    
    do {
        //printf("readcom\n");
        
        ++pos;

        if(buf[pos] == '\n') {
            if(pos > start){
                //pos--;
                break;
            }
        }

        if (pos == bytes) {
            printf("ref\n");
            // refresh the buffer
            bytes = read(STDIN_FILENO, buf, BUFSIZE);
            if (bytes < 1) {
                closed = 1;
                break;
            }

            // save word so far
            int fraglen = pos - start;
            tok = realloc(tok, toklen + fraglen + 1);
            memcpy(tok + toklen, buf + start, fraglen);
            toklen += fraglen;

            pos = 0;
            start = 0;
        }
    } while (!isspace(buf[pos]));

    // grab the word from the current buffer
    // (Note: start == pos if we refreshed the buffer and got a space first.)
    if (start < pos) {
        int fraglen = pos - start;
        tok = realloc(tok, toklen + fraglen + 1);
        memcpy(tok + toklen, buf + start, fraglen);
        toklen += fraglen;
    }

    if (tok) {
        tok[toklen] = '\0';
    }
    return tok;
}