#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void read_input(char line[], int size);
void shell();
char** parse_input(char input[],char* letter);
void setup_environment();
void execute_shell_bultin(char* type, char* parameter);
void execute_command(char** args);
void assignForVariable(char equ[]);
void evaluate_expression(char* param);

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
        
        // printf("%s",(*(command+1)));
        // printf("%c",*(*(command+1)+i));

        
            // *(command+1) = tokensOfParam;
        


        // for (int i=0;(*(command+1)!=NULL)&&i<strlen(*(command+1));i++){
        //     // printf("[%d]\n",i);
        //     if (*(*(command+1)+i)=='$'){
        //         for (int j=0;j<ind;j++){
        //             // printf("%c\n",((command+1)+i+1));
        //             // ((command+1)+i) = '';
        //             if(strcmp(vars[j][0],((command+1)+i+1))==0){
        //             printf ("Entered");
        //             //     ((command+1)+i+1) = vars[j][1];
        //             //     break;
        //             // }else{
        //             //     ((command+1)+i+1) = "";
        //             }
        //         }
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

int evaluate_expression(char** command){
    int evaluated = 0;
    if((*(command+1)!=NULL && strstr(*(command+1), "$") != NULL)){
        char* value = " ";
        char** tokensOfParam = parse_input(*(command+1),"$");
        for (int i=0; tokensOfParam[i]!=NULL ;i++){
            // printf("[%d] %s\n",i,tokensOfParam[i]);
            for (int j=0;j<ind;j++){
                // printf("[%d] %s",i,tokensOfParam[i]);
                // printf(" %s\n",vars[j][0]);
                if(strcmp(vars[j],tokensOfParam[i])==0){
                    // printf ("Yes and == %s\n",vars[j+1]);
                    // printf("[%d][0] = %s\n[%d][1] = %s\n",j+1,vars[j][0],j+1,vars[j][1]);
                    value=vars[j+1];
                    tokensOfParam[i]=value;
                    break;
                }
                // tokensOfParam[i]=value;
            }
            printf("TOP [%d] %s",i,tokensOfParam[i]);
        }
    }
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
    // if (strlen(equ)>0 && equ[strlen(equ)-1]=='\n') equ[strlen(equ)-1]='\0';
    char** tokens = parse_input(equ,"=");
    strcpy (vars[ind++],*tokens);
    strcpy (vars[ind++],*(tokens+1)); 
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
    char** tokens = (char*)malloc(sizeof(char));
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
