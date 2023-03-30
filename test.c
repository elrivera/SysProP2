#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "arraylist.h"
#include "tok.h"

/*
    dealing with bad syntax, e.g., foo < < bar 
    -> print an error message and skip to the next \name

    dealing with nonsense, e.g., foo > bar | baz
        where should the output for foo go?
            (most shell will sent it to bar and send nothing to baz)

consider edge cases and weird scenarios
    we discussed combining redirection and pipes

    program1 < input | program2 > output
        fine!

    program1 < input > output1 | program2
        problem! does output of program1 go to output1 or the pipe?
        should we just not allow this and report an error?

    program1 | program2 < input

    where are wildcards allowed?

    can you use a wildcard in the first word?
        ./m*sh
        yes!

    can you use a wildcard in a redirection?

echo test > foo*
yes, but only if the pattern matches exactly one file!
(you don't need to handle this in the assignment)

*/

int main (int argc, char **argv){

    //does the shell continue 

    return;
}