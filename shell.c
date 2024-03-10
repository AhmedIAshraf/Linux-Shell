#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void read_input(char line[], int size);
void shell();
char** parse_input(char input[],char* letter);
void setup_environment();
void execute_shell_bultin(char* type, char* parameter, char** comm);
void execute_command(char** args);
void assignForVariable(char equ[]);
char** evaluate_expression(char** command);
void concat (char** comm);

int ind=0;
char vars[200][100] ;

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
        command = parse_input(line," ");
        
        int builtin = (strncmp(*command,"cd",2)==0 || strncmp(*command,"echo",4)==0 || strncmp(*command,"export",6)==0) ? 1 : 0;
        int evaluated=0;
        if((*(command+1)!=NULL && strstr(*(command+1), "$") != NULL)){
            evaluated=1;
            if (*(*(command+1)+strlen(*(command+1))-1) == '"') 
                *(*(command+1)+strlen(*(command+1))-1) = NULL;
            *(command+1)=evaluate_expression(command);
        } 
        
        switch(builtin){
            case 1:
                execute_shell_bultin(*command,*(command+1),command);
                break;
            case 0:
                execute_command(command);
                break;
        }
    }while (strncmp(*command,"exit\0",5)!=0);
    exit(0);
}

char**  evaluate_expression(char** command){
    char* value = " ";
    char** tokensOfParam = parse_input(*(command+1),"$");
    for (int i=0; tokensOfParam[i]!=NULL ;i++){
        for (int j=0;j<ind;j++){
            if(strcmp(vars[j],tokensOfParam[i])==0){
                value=vars[j+1];
                tokensOfParam[i]=value;
                break;
            }
        }
        // printf("TOP [%d] %s\n",i,tokensOfParam[i]);
    }
    return value;
}

void execute_shell_bultin(char* type, char* parameter, char** comm){
    switch(*(type+1)){ // Checking the second character in the command : d for cd, c for echo and x for expert. 
        case 'd':
            chdir(parameter);
            break;
        case 'c':
            printf(parameter);
            printf("\n");
            break;
        case 'x':
            concat (comm);
            assignForVariable(parameter);
            break;
    }
}

void concat (char** comm){
    int i=2;
    while (*(comm+i)!=NULL){
        *(comm+1) = strcat (*(comm+1),*(comm+i));
        i++;
    }
}

void assignForVariable(char equ[]) {
    // if (strlen(equ)>0 && equ[strlen(equ)-1]=='\n') equ[strlen(equ)-1]='\0';
    char** tokens = parse_input(equ,"=");
    strcpy (vars[ind++],*tokens);
    strcpy (vars[ind],*(tokens+1)); 
    // printf (*(tokens+1));
    ind ++;
    // printf("[%d][Name] = %s\n[%d][Value] = %s\n",ind-2,vars[ind-2],ind-1,vars[ind-1]);
    // printf("[%d][Name] = %s\n[%d][Value] = %s\n",0,vars[0],1,vars[1]);

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
        printf("Error\n");
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

char** parse_input(char input[],char* letter) {
    char** tokens = (char*)malloc(100*sizeof(char));
    if (tokens == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    char* token = strtok(input, letter);
    int i = 0;
    while (token != NULL) {
        tokens[i] = token;
        i++;
        tokens = (char*)realloc(tokens, (i + 1) * sizeof(char));
        if (tokens == NULL) {
            printf("Memory reallocation failed\n");
            exit(1);
        }
        token = strtok(NULL, letter);
    }
    tokens[i] = NULL;
    return tokens;
}
