#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 80

void sequential_mode(char arguments[MAX_LINE/2+1], int style);
void parallel_mode(char arguments[MAX_LINE/2+1], int style);
void execution(char arguments[MAX_LINE/2+1], pid_t pid_, int style);
int file_verification(FILE *file_);

int main(int _argc, char* _argv[]) {

    FILE *file;
    char args[MAX_LINE/2 + 1], save_args[MAX_LINE/2 + 1], *aux_args;
    int validador_style = 0, validador_file = 0;
    int should_run = 1, count_args = 0;
    pid_t pid;

    if(_argc >= 2) {
        file = fopen(_argv[1], "r");

        // TRATAMENTO DE ERRO
        if(file == NULL) {
            fprintf(stderr, "Arquivo não abriu corretamente ou não existe!\n");
            return 1;
        }

        validador_file = file_verification(file);
        if(validador_file == 1) {
            fprintf(stderr, "Arquivo não apresenta o comando exit para finalizar!\n");
            return 1;
        }
        fclose(file);

        file = fopen(_argv[1], "r");

        while(fgets(args, 100, file) != NULL)
        {
            for(int i = 0; i < strlen(args); i++) if(args[i] == '\n') args[i] = 0;
            
            if(validador_style == 0)
            {
                if(strcmp(args, "!!") == 0 && count_args != 0) strcpy(args, save_args);
                
                if(strcmp(args, "!!") == 0 && count_args == 0) fprintf(stderr, "NO COMMANDS FOUND!\n");
                else {
                    if(strlen(args) >= MAX_LINE/2) {
                        fprintf(stderr, "Limited of caracters Reached\n");
                        memset(stdin,0,MAX_LINE);
                    }
                    else {
                        if(strcmp(args, "style parallel") == 0) validador_style = 1;
                        else if(strcmp(args, "Exit") == 0 || strcmp(args, "exit") == 0) return 0;
                        else {
                            strcpy(save_args, args);
                            sequential_mode(args, validador_style);
                            count_args++;
                        }
                    }
                }
            }
            else 
            {   
                if(strcmp(args, "!!") == 0 && count_args != 0) strcpy(args, save_args);
                
                if(strcmp(args, "!!") == 0 && count_args == 0) fprintf(stderr, "NO COMMANDS FOUND!\n");
                else {
                    if(strlen(args) >= MAX_LINE/2) {
                        fprintf(stderr, "Limited of caracters Reached\n");
                        memset(stdin,0,MAX_LINE);
                    }
                    else {
                        if(strcmp(args, "style sequential") == 0) validador_style = 0;
                        else if(strcmp(args, "Exit") == 0 || strcmp(args, "exit") == 0) return 0;
                        else {
                            strcpy(save_args, args);
                            parallel_mode(args, validador_style);
                            count_args++;
                        }
                    }
                }
            }
        }

        fclose(file);
    }
    else // INTERATIVO 
    {
        while(should_run)
        {
            if(validador_style == 0) {

                printf("rjhxa seq> ");
                fgets(args, (MAX_LINE/2)+1, stdin);
                fflush(stdout);

                // REMOVE \n
                for(int i = 0; i< strlen(args); i++) if(args[i] == '\n') args[i] = 0;

                if(strcmp(args, "!!") == 0 && count_args != 0) strcpy(args, save_args);
                
                if(strcmp(args, "!!") == 0 && count_args == 0) fprintf(stderr, "NO COMMANDS FOUND!\n");
                else {
                    if(strlen(args) >= MAX_LINE/2) {
                        fprintf(stderr, "Limited of caracters Reached\n");
                        memset(stdin,0,MAX_LINE);
                    }
                    else {
                        if(strcmp(args, "style parallel") == 0) validador_style = 1;
                        else if(strcmp(args, "Exit") == 0 || strcmp(args, "exit") == 0) return 0;
                        else {
                            strcpy(save_args, args);
                            sequential_mode(args, validador_style);
                            count_args++;
                        }
                    }
                }
            }
            else {
                printf("rjhxa par> ");
                fgets(args, 100, stdin);
                fflush(stdout);

                // REMOVE \n
                for(int i = 0; i< strlen(args); i++) if(args[i] == '\n') args[i] = 0;
                
                if(strcmp(args, "!!") == 0 && count_args != 0) strcpy(args, save_args);
                
                if(strcmp(args, "!!") == 0 && count_args == 0) fprintf(stderr, "NO COMMANDS FOUND!\n");
                else {
                    if(strlen(args) >= MAX_LINE/2) {
                        fprintf(stderr, "Limited of caracters Reached\n");
                        memset(stdin,0,MAX_LINE);
                    }
                    else {
                        if(strcmp(args, "style sequential") == 0) validador_style = 0;
                        else if(strcmp(args, "Exit") == 0 || strcmp(args, "exit") == 0) return 0;
                        else {
                            strcpy(save_args, args);
                            parallel_mode(args, validador_style);
                            count_args++;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

void sequential_mode(char arguments[MAX_LINE/2+1], int style) {

    pid_t pid;
    int count = 0;
    char *aux_args, aux_execution[MAX_LINE/2+1];

    aux_args = strtok(arguments, ";");
    while(aux_args != NULL)
    {
        strcpy(aux_execution, aux_args);
        count++;
        aux_args = strtok(NULL, ";");
        execution(aux_execution, pid, style);
        memset(aux_execution, 0, MAX_LINE/2+1);
    }

    free(aux_args);
}

void parallel_mode(char arguments[MAX_LINE/2+1], int style) {

    pid_t pid;
    int count = 0;
    char *aux_args, aux_execution[MAX_LINE/2+1];

    aux_args = strtok(arguments, ";");
    while(aux_args != NULL)
    {
        strcpy(aux_execution, aux_args);
        count++;
        aux_args = strtok(NULL, ";");
        execution(aux_execution, pid, style);
        memset(aux_execution, 0, MAX_LINE/2+1);
    }

    for(int i = 0; i < count; i++) {
        wait(NULL);
    }

    free(aux_args);
}

void execution(char arguments[MAX_LINE/2+1], pid_t pid_, int style) {

    char *aux_spaces, aux_test_args[MAX_LINE/2+1], *aux_test;
    char **list_args; 
    int x = 0, count = 0;

    if(strlen(arguments) != 0) 
    {
        strcpy(aux_test_args, arguments);
        aux_test = strtok(aux_test_args, " ");
        while(aux_test != NULL)
        {
            count++;
            aux_test = strtok(NULL, " ");
        }

        count++;
        list_args = malloc(count*sizeof(char **));

        for(int i = 0; i < count; i++) list_args[i] = malloc(MAX_LINE/2+1*sizeof(char *));

        aux_spaces = strtok(arguments, " ");
        while(aux_spaces != NULL)
        {
            //if(count == 0) strcpy(first_args, aux_spaces);
            strcpy(list_args[x], aux_spaces); // AQUI
            x++;
            aux_spaces = strtok(NULL, " ");
        }

        list_args[x] = 0;

        pid_ = fork();
        if(pid_ < 0) fprintf(stderr, "Fork failed");
        else if(pid_ == 0) { // CHILD PROCESS
            if(execvp(list_args[0], list_args) == -1) fprintf(stderr, "COMAND INVALID\n");
        }
        else { // PARENT PROCESS

            if(style == 0) wait(NULL);
        }

        for(int i = 0; i < x; i++) {
            free(list_args[i]);
        }
        free(list_args);
    }
}

int file_verification(FILE *file_) {

    char aux[100];
    int count_exit = 0, validador_return = 0;

    while(fgets(aux, 100, file_) != NULL) {
        for(int i = 0; i < strlen(aux); i++) if(aux[i] == '\n') aux[i] = 0;

        if(strcmp(aux, "Exit") == 0 || strcmp(aux, "exit") == 0) count_exit++;
    }

    if(count_exit == 0) validador_return = 1;

    return validador_return;
}