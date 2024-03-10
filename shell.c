#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void read_input(char line[], int size);
void shell();
char** parse_input(char input[],char* letter);
void setup_environment();
void execute_shell_bultin(char* type, char* parameter, char** comm);
void execute_command(char** args);
void assignForVariable(char equ[]);
char** evaluate_expression(char** command);
void concat (char** comm);
// void reap_child_zombie(int pid);
void write_to_log_file(char* str);
void on_child_exit();
void register_child_signal();

int ind=0;
char vars[200][100] ;

int main(){
    register_child_signal();
    setup_environment();
    shell();
    return 0;
}
    
void register_child_signal(){
    signal(SIGCHLD,on_child_exit);
}

void on_child_exit(int pid){
    kill(pid, SIGTERM);
    write_to_log_file("Child terminated Successfully.\0");
}

// void reap_child_zombie(int pid){
//     printf("Entered Reap\n");
//     kill(pid, SIGTERM);
//     printf("Terminated\n");
// }

void write_to_log_file(char* str){
    FILE *log = fopen("Observer.txt", "a"); 
    fprintf(log, "%s\n",str);
    fclose (log);
}

void setup_environment(){
    chdir("/home/ahmed");
}

void shell(){
    char** command;
    char line[100];
    read_input(line, sizeof(line));
    command = parse_input(line," ");

    while (*command==NULL||strncmp(*command,"exit\0",5)!=0){
        if (*command==NULL){
            read_input(line, sizeof(line));
            command = parse_input(line," ");
            continue;
        }
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
        read_input(line, sizeof(line));
        command = parse_input(line," ");
    }
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
        char put[50];
        strcpy(put,*(comm+i));
        // printf ("Put = %s\n",put);
        strcat (*(comm+1)," ");
        // printf ("Putting Space = %s\n",*(comm+1));
        strcat (*(comm+1),put);
        // printf("The new value = %s\n",*(comm+1));
        i++;
    }
    
}

void assignForVariable(char equ[]) {

    char** tokens = parse_input(equ,"=");
    strcpy (vars[ind++],*tokens);
    
    if (*(*(tokens+1))=='"'){
        *(*(tokens+1)+strlen(*(tokens+1))-1)= '\0';
        for (int i=0; *(*(tokens+1)+i)!='\0' ;i++){
            *(*(tokens+1)+i) = *(*(tokens+1)+i+1);
        }
    }
    strcpy (vars[ind],*(tokens+1)); 
    ind ++;

}

void execute_command(char** args){
    int pid = fork();
    int wait = 1;
    int status;
    for (int i=0; args[i]!=NULL ; i++){
        if (strcmp(args[i],"&")==0){
            wait=0;
            args[i]=NULL;
        }
    }
    if (pid<=0){
        char** temp = parse_input(*(args+1)," ");
        for (int i=0; *(temp+i)!=NULL ;i++){
            *(args+2+i) = *(temp+i);
        }
        execvp(*args, args);
        printf("Error\n");
        exit(0);
    }
    else{
        if (wait==1) waitpid(pid,&status,0);
    }
}

void read_input(char line[], int size){
    fgets (line, size, stdin);
    if (strlen(line)>0 && line[strlen(line)-1]=='\n') line[strlen(line)-1]='\0';
}

char** parse_input(char input[],char* letter) {
    char** tokens = (char*)malloc(sizeof(char*));
    if (tokens == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    char* token = strtok(input, letter);
    int i = 0;
    while (token != NULL) {
        tokens[i] = token;
        i++;
        tokens = (char*)realloc(tokens, (i + 1) * sizeof(char*));
        if (tokens == NULL) {
            printf("Memory reallocation failed\n");
            exit(1);
        }
        token = strtok(NULL, letter);
    }
    tokens[i] = NULL;
    return tokens;
}
