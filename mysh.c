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
char* checkFile(char* command);
void asterisk(char* tok, list_t* param);

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

    if (dest == NULL){

        char* to = getenv("HOME");

        if (chdir(to) != 0){
            perror("cd failed");
        }
        return;
    }

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
    char *oop = "!";
    char *cd = "cd";
    char *piperr = "cannot pipe twice in one command\n";
    char *inerr = "cannot redirect input multiple times in the same command\n";
    char *outerr = "cannot redirect output multiple times in the same command\n";
    char *tmerr = "cannot pipe and redirect io in the same command\n";
    char *filefail = "could not find file\n";
    char *dupfail = "couldn't dup\n";
    char *execfail = "could not execute\n";
    char *comfail = "could not find command\n";
    char *pipefail = "pipe failed";
    char *forkfail = "fork failed";

    int err = 0;
    int p = 0;
    if(argc > 2) {
        printf("too many arguments. failed\n");
        return 1;
    }

    write (STDOUT_FILENO, init, strlen(init));
    int rep = 0;


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

    if(argc == 2){
            int file = open(argv[1], O_RDONLY);
            if (file == -1){
                printf("could not open file");
                return 1;
            }
                
            if (dup2(file, STDIN_FILENO) == -1){
                printf("could not change input");
                return 2;
            }
            
        }

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
        
        char *cmd;

        int z = 0;
        int l = al_length(&commands);
        char* command = al_lookup(&commands, 0);
        char* in = NULL;
        char* out = NULL;
        p = 0;
        int indicator = 0;
        int i = 0;

        
        //write (STDOUT_FILENO, mmsg, strlen(mmsg));
        //write (STDOUT_FILENO, command, strlen(command));
        //write (STDOUT_FILENO, nl, strlen(nl));

        list_t parameters;
        al_init(&parameters, 32);
        //char* parameters[l];
        char* cur;
        
        //write (STDOUT_FILENO, msg, strlen(msg));
        while((cur = al_lookup(&commands, i))){
            //printf("here\n");

            if (!strcmp(cur, "<")){
                //printf("redirect input\n");
                if(in != NULL || indicator != 0){
                        write (STDOUT_FILENO, inerr, strlen(inerr));
                        err = 1;
                        break;
                    }
                indicator = 1;
                i++;
                continue;
            }
            if (!strcmp(cur, ">")){
                //printf("redirect output\n");
                if(out != NULL || indicator != 0){
                        write (STDOUT_FILENO, outerr, strlen(outerr));
                        err = 1;
                        break;
                    }
                indicator = -1;
                i++;
                continue;
            }

            //check for pipe
            if(!strcmp(cur, "|")){
                if(out != NULL || in != NULL || indicator != 0){
                    write (STDOUT_FILENO, tmerr, strlen(tmerr));
                    err = 1;
                }
                p = 1;
                i++;
                break;
            }

            //react to </>
            if (indicator != 0){
                //change input
                if(indicator == 1){
                    //detect illegal input
                    
                    in = cur;
                    indicator = 0;
                    i++;
                    continue;
                }
                //change output
                else if(indicator == -1){
                    //detect illegal input
                    
                    out = cur;
                    indicator = 0;
                    i++;
                    continue;
                }
            }

            //check for glob
            
                if(strchr(cur, '*')){
                    //printf("ast\n");
                    asterisk(cur, &parameters);
                    i++;
                    continue;
                }
            

            //push cur
            al_push(&parameters, cur);
            

            //check for exit
            if (!strcmp(cur, exit)) {
                z =1;
                break;
            }

            //check for pwd
           

            
            //write (STDOUT_FILENO, cur, strlen(cur));
            //write (STDOUT_FILENO, nl, strlen(nl));
            i++;
        }

        //address illegal input
        if (err == 1){
            err =0;
            write (STDOUT_FILENO, oop, strlen(oop));
            continue;
        }
        
        //temp code to confirm parameters  
        char** param = al_data(&parameters);
        int pl = al_length(&parameters);

        /*
        printf("confirm parameters\n");
        for(int i = 0; i < pl; i++){
            printf("%s\n", param[i]);
        }
        */

        //repeat for second command if fork
        if (p == 1){     
            char* c2 = al_lookup(&commands, i);
            list_t p2;
            al_init(&p2, 16);

            //finish parsing input
            while((cur = al_lookup(&commands, i))){
                //printf("here\n");

                //detect input redirect
                if (!strcmp(cur, "<") || !strcmp(cur, ">")){
                    //printf("redirect input\n");
                    write (STDOUT_FILENO, tmerr, strlen(tmerr));
                    err = 1;
                    break;
                }

                //detect illegal input
                if(!strcmp(cur, "|")){
                    write (STDOUT_FILENO, piperr, strlen(piperr));;
                    err = 1;
                    break;
                }

                
                //check for glob
                if(strchr(cur, '*')){
                    asterisk(cur, &parameters);
                    i++;
                    continue;
                }
                

                //push to parameters 2 arraylist
                al_push(&p2, cur);

                //check if exit
                if (!strcmp(cur, exit)) {
                    z =1;
                    break;
                }


                //check if pwd
                

            
                //write (STDOUT_FILENO, cur, strlen(cur));
                //write (STDOUT_FILENO, nl, strlen(nl));
                i++;
            }

            //address illegal input
            if (err == 1){
                err =0;
                write (STDOUT_FILENO, oop, strlen(oop));
                continue;
            }
            //setup variables
            char *cmd2;
            param = al_data(&p2);
            int pl = al_length(&p2);

            //temp parameter confirmation
            printf("confirm 2 parameters\n");
            for(int i = 0; i < pl; i++){
                printf("%s\n", param[i]);
            }

            //now pipe, we have both commands and parameters

            //pipe ends
            int fd[2];
            if(pipe(fd) == -1){
                write(STDOUT_FILENO, pipefail, strlen(pipefail));
            }
            //create process 1
            int pid1 = fork();
            if(pid1 < 0){
                write(STDOUT_FILENO, forkfail, strlen(forkfail));
            }

            //change out and execute process 1
            if (pid1 == 0){
                //child
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);

                if (!strcmp(command, pwd)) {
                    getDir();
                    return 0;
                }

                if(strchr(command, '/') != NULL) strcpy(cmd, command);
                //get command pathif necessary
                else{
                    cmd = checkFile(command);
                    if (cmd == NULL) return 4;
                }
                if (execv (cmd, al_data(&parameters)) == -1){ //execute command
                    return 3;
                }
            }


            if (z ==1) break;
            if (z == 2) continue;


            //create process 2
            int pid2 = fork();
            if(pid2 < 0){
                write(STDOUT_FILENO, forkfail, strlen(forkfail));
            }

            //change input and execute process 2
            if(pid2 == 0){
                //child
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                close(fd[1]);

                if (!strcmp(command, pwd)) {
                    getDir();
                    return 0;
                }
                
                if(strchr(c2, '/') != NULL) strcpy(cmd2, c2);
                //get command path if necessary
                else{
                    cmd2 = checkFile(c2);
                    if (cmd2 == NULL) return 4;
                }
                if (execv (cmd2, al_data(&p2)) == -1){ //execute command
                    return 3;
                }
            }

            //parent
            close(fd[0]);
            close(fd[1]);

            int pid1stat;
            int pid2stat;
            waitpid(pid1, &pid1stat, 0);
            waitpid(pid2, &pid1stat, 0);

            int p2sc = 0;
            int p1sc = 0;

            if(WIFEXITED(pid1stat)){
                p1sc = WEXITSTATUS(pid1stat);
                if (p1sc == 1) write (STDOUT_FILENO, filefail, strlen(filefail));
                if (p1sc == 2) write (STDOUT_FILENO, dupfail, strlen(dupfail));
                if (p1sc == 3) write (STDOUT_FILENO, execfail, strlen(execfail));
                if (p1sc == 4) write (STDOUT_FILENO, comfail, strlen(comfail));

            }

            if(WIFEXITED(pid2stat)){
                p2sc = WEXITSTATUS(pid2stat);
                if (p2sc == 1) write (STDOUT_FILENO, filefail, strlen(filefail));
                if (p2sc == 2) write (STDOUT_FILENO, dupfail, strlen(dupfail));
                if (p2sc == 3) write (STDOUT_FILENO, execfail, strlen(execfail));
                if (p2sc == 4) write (STDOUT_FILENO, comfail, strlen(comfail));

            }

            if (p1sc != 0 || p2sc != 0) write (STDOUT_FILENO, oop, strlen(oop));

            //clean up
            al_destroy(&commands);
            al_destroy(&parameters);
            al_destroy(&p2);
            continue;
        }


        /*
        //temp code to check input/output redirect
        if (in != NULL) printf("final input will be: %s\n", in);
        else printf("no in\n");
        if (out != NULL) printf("final output will be: %s\n", out);
        else printf("no out\n");
        */
      
        if (z ==1) break;
        if (z == 2) continue;
        

        //push NULL end
        al_push(&parameters, NULL);




        //temp call to cd - need to change!!
        if(!strcmp(command, cd)) {
            changeDir(al_lookup(&parameters, 1));
            //printf("changed\n");
            al_destroy(&commands);
            al_destroy(&parameters);
            continue;
        }

        
        
        //create child process
        if(fork() != 0) {   //Parent
            int wstatus;    //Wait for child
            wait(&wstatus);
            if(WIFEXITED(wstatus)){
                int sc = WEXITSTATUS(wstatus);
                if (sc == 1) write (STDOUT_FILENO, filefail, strlen(filefail));
                if (sc == 2) write (STDOUT_FILENO, dupfail, strlen(dupfail));
                if (sc == 3) write (STDOUT_FILENO, execfail, strlen(execfail));
                if (sc == 4) write (STDOUT_FILENO, comfail, strlen(comfail));

                if (sc != 0) write (STDOUT_FILENO, oop, strlen(oop));
            }
        }
        else{
            
            //check if output redirect
            if(out != NULL){
                //int saved = dup(STDOUT_FILENO);
                int file = open(out, O_WRONLY|O_CREAT, 0640);
                if (file == -1){
                    return 1;
                }
                
                if (dup2(file, STDOUT_FILENO) == -1){
                    return 2;
                }
            }

            //check if input redirect
            if(in != NULL){
                //int saved = dup(STDOUT_FILENO);
                int file = open(in, O_RDONLY);
                if (file == -1){
                    return 1;
                }
                
                if (dup2(file, STDIN_FILENO) == -1){
                    return 2;
                }
            }
            if (!strcmp(command, pwd)) {
                getDir();
                return 0;
            }

            //get command path
            if(strchr(command, '/')) {
                printf("pathin\n");
                strcpy(cmd, command);}
            //printf("command path: %s\n", cmd);
            else{
            cmd = checkFile(command);
                if(cmd == NULL) {
                    return 4;
                }
            }
            //execute command
            if (execv (cmd, al_data(&parameters)) == -1){ //execute command

                return 3;
            } 
        }
        
        //clean arraylists
        al_destroy(&commands);
        al_destroy(&parameters);
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


char* checkFile(char* command){
    struct stat sfile;
    static char ret[1024];
    //printf("checkfile!\n");
    char one[128];
    char two[128];
    char three[128];
    char four[128];
    char five[128];
    char six[128];

    strcpy(one, "/usr/local/sbin/");
    strcat(one, command);

    strcpy(two, "/usr/local/bin/");
    strcat(two, command);

    strcpy(three, "/usr/sbin/");
    strcat(three, command);

    strcpy(four, "/usr/bin/");
    strcat(four, command);

    strcpy(five, "/sbin/");
    strcat(five, command);

    strcpy(six, "/bin/");
    strcat(six, command);

    if(stat(one, &sfile) == 0){
        //printf("checkfile!1\n");
        strcpy(ret, "/usr/local/sbin/");
        strcat(ret, command);
        return ret;
    }

    else if(stat(two, &sfile) == 0){
        //printf("checkfile!2\n");
        strcpy(ret, "/usr/local/bin/");
        strcat(ret, command);
        return ret;
    }

    else if(stat(three, &sfile) == 0){
        //printf("checkfile!3\n");
        strcpy(ret, "/usr/sbin/");
        strcat(ret, command);
        return ret;
    }
    

    else if(stat(four, &sfile) == 0){
        //printf("checkfile!4\n");
        strcpy(ret, "/usr/bin/");
        strcat(ret, command);
        return ret;
    }

    else if(stat(five, &sfile) == 0){
        //printf("checkfile!5\n");
        strcpy(ret, "/sbin/");
        strcat(ret, command);
        return ret;
    }

    else if(stat(six, &sfile) == 0){
        //printf("checkfile!6\n");
        strcpy(ret, "/bin/");
        strcat(ret, command);
        return ret;
    }
    
    else {
        //printf("return null!\n");
        return NULL;
    }
}


void asterisk(char* tok, list_t* param){
    const char *ast = "*";
    
    if(strcmp(tok, "*")){
        //printf("in\n");
        struct dirent *de;  // Pointer for directory entry
        list_t parts;
        char* p[2];
        al_init(&parts, 8);

        char* nt = strtok(tok, ast);
        p[0] = nt;
        if(tok[0] == '*'){
            p[1] = p[0];
            p[0] = NULL;
        }
        else{
            char* nt = strtok(NULL, ast);
            p[1] = nt;
        }

        //printf("in: %s\n", p[0]);
        //printf("out: %s\n", p[1]);

        al_push(&parts, NULL);
        //printf("parts in \n");
        // opendir() returns a pointer of DIR type. 
        DIR *dr = opendir(".");
  
        if (dr == NULL){  // opendir returns NULL if couldn't open directory
            //printf("Could not open current directory" );
            //return 0;
        }
  
        while ((de = readdir(dr)) != NULL){
            //printf("%s\n", de->d_name);
            
            int chf = 1;
            int chb = 1;
            
            if(p[0] != NULL){
                for(int i = 0; i < strlen(p[0]); i++){
                    if(p[0][i] != de->d_name[i]) chf = 0;
                }
            }

            if(p[1] != NULL){
                for(int i = 0; i < strlen(p[1]); i++){
                    if(p[1][i] != de->d_name[strlen(de->d_name) -1 - i]) chf = 0;
                }
            }

            if(chf == 1 && chb == 1) al_push(param, de->d_name);


            /*
            int i = 0;
            char* temp = al_lookup(&parts, i);
            while(temp != NULL){
                if(strstr(de->d_name, temp)){
                    i++;
                    temp = al_lookup(&parts, i);
                }
                else break;
            }
            if(temp == NULL) {
                al_push(param, de->d_name);
                printf("pushed: %s\n", de->d_name);
            }
            */
        }       
  
        closedir(dr);  
    }

    else{
        struct dirent *de;  // Pointer for directory entry
  
        // opendir() returns a pointer of DIR type. 
        DIR *dr = opendir(".");
  
        if (dr == NULL){  // opendir returns NULL if couldn't open directory
            printf("Could not open current directory" );
            //return 0;
        }
  
        while ((de = readdir(dr)) != NULL)
            printf("%s\n", de->d_name);
  
        closedir(dr);
    }
    /*
    DIR *d;
    struct dirent *dir;
    if(!strcmp(tok, "*")){
        printf("cmp\n");
        d = opendir(".");
        if(d){
            while(dir != NULL){
                if (dir->d_type == DT_REG){
                    printf("%s\n", dir->d_name);
                    al_push(param, dir->d_name);
                }
                dir = readdir(d);
            }
            closedir(d);
        }
    }
    return;
    
    char prior[strlen(tok)];
    char sub[strlen(tok)];
    int i = 0;
    int j = 0;
    while(tok[i] != '*'){
        prior[j] = tok[i];

    }
    
    */
}


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