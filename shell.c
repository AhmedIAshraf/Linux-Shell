#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void read_input(char line[], int size);
void shell();
char** parse_input(char command[]);
void setup_environment();
void execute_shell_bultin(char* type, char* parameter);
void execute_command(char** args);
void assignForVariable(char equ[]);
void evaluate_expression(char* param);

int ind=0;
char* vars[100][2] ;

int main(){
    // register_child_signal(on_child_exit());
    setup_environment();
    shell();
    return 0;
}
    
// int on_child_exit(){
//     reap_child_zombie();
//     write_to_log_file("Child terminated");
//     return 0;
// }

void setup_environment(){
    chdir("/home/ahmed");
}

void shell(){
    char** command;
    char line[100];
    do{
        read_input(line, sizeof(line));
        command = parse_input(line);
        int builtin = (strncmp(*command,"cd",2)==0 || strncmp(*command,"echo",4)==0 || strncmp(*command,"export",6)==0) ? 1 : 0;
        
        // printf("%s",(*(command+1)));
        // printf("%c",*(*(command+1)));

        // for (int i=0;i<100;i++){
        //     printf("%c",*(*(command+1)+1));
        //     if (*(*(command+1)+i)=='$'){
        //         // for (int j=0;j<ind;j++){
        //             printf ("Entered");
        //             // printf("%c\n",((command+1)+i+1));
        //             // ((command+1)+i) = '';
        //             // if(strcmp(vars[j][0],((command+1)+i+1))==0){
        //             //     ((command+1)+i+1) = vars[j][1];
        //             //     break;
        //             // }else{
        //             //     ((command+1)+i+1) = "";
        //             // }
        //         // }
        //     }
        // }

        // printf("%s",(*(command+1)));
        
        switch(builtin){
            case 1:
                execute_shell_bultin(*command,*(command+1));
                break;
            case 0:
                execute_command(command);
                break;
        }
    }while (strncmp(*command,"exit\0",5)!=0);
    exit(0);
}

void evaluate_expression(char* param){
}

void execute_shell_bultin(char* type, char* parameter){
    switch(*(type+1)){ // Checking the second character in the command : d for cd, c for echo and x for expert. 
        case 'd':
            chdir(parameter);
            break;
        case 'c':
            printf(parameter);
            break;
        case 'x':
            assignForVariable(parameter);
            break;
    }
}

void assignForVariable(char equ[]) {
    char** tokens = (char*)malloc(sizeof(char));
    vars[ind][0] = strtok(equ,"=");
    vars[ind++][1] = strtok(NULL,"=");
    printf("[%d][0] = %s\n[%d][1] = %s",ind,vars[ind-1][0],ind,vars[ind-1][1]);
}

void execute_command(char** args){
    int pid = fork();
    int wait = 1;
    for (int i=0; args[i]!=NULL ; i++){
        if (strcmp(args[i],"&")==0){
            wait=0;
            args[i]=NULL;
        }
    }
    if (pid<=0){
        execvp(*args, args);
        printf("Error");
        exit(0);
    }
    else{
        if (wait==1) waitpid(&pid);
    }
}

void read_input(char line[], int size){
    fgets (line, size, stdin);
    if (strlen(line)>0 && line[strlen(line)-1]=='\n') line[strlen(line)-1]='\0';
}

char** parse_input(char input[]) {
    char** tokens = (char*)malloc(sizeof(char));
    if (tokens == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    
    char* token = strtok(input, " ");
    int i = 0;
    while (token != NULL) {
        tokens[i] = token;
        i++;
        tokens = (char*)realloc(tokens, (i + 1) * sizeof(char));
        if (tokens == NULL) {
            printf("Memory reallocation failed\n");
            exit(1);
        }
        token = strtok(NULL, " ");
    }
    tokens[i] = NULL;
    return tokens;
}