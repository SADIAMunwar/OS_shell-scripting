#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
using namespace std;

#define max_cmd 3
#define max_arg 10

void run(char* commands[max_cmd][max_arg + 1], int total) {
    int pipefd[2];
    int input_fd = 0;

    for (int i = 0; i < total; i++) {
        if (i < total - 1) {
            pipe(pipefd);
        }

        pid_t pid = fork();
        if (pid == 0) {
            // Child
            if (input_fd != 0) {
                dup2(input_fd, 0);
                close(input_fd);
            }

            if (i < total - 1) {
                dup2(pipefd[1], 1);
                close(pipefd[0]);
                close(pipefd[1]);
            }

            execvp(commands[i][0], commands[i]);
            perror("execvp failed");
            exit(1);
        }

        // Parent
        if (input_fd != 0) {
            close(input_fd);
        }

        if (i < total - 1) {
            close(pipefd[1]);
            input_fd = pipefd[0];
        }
    }

    for (int i = 0; i < total; i++) {
        wait(NULL);
    }
}

int main() {
    char input[100];
    char* tokens[20];
    char* commands[max_cmd][max_arg + 1];
while(true){
cout<<"shell>";
cin.getline(input,100);
if(strcmp(input,"q")==0){
cout<<"existing shell "<<endl;
break;
}    // Tokenize input
    int token_count = 0;
    char* token = strtok(input, " ");
    while (token != NULL) {
        tokens[token_count++] = token;
        token = strtok(NULL, " ");
    }

    // Separate commands by '|'
    int cmd_index = 0, arg_index = 0;
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            commands[cmd_index][arg_index] = NULL;
            cmd_index++;
            arg_index = 0;
        } else {
            commands[cmd_index][arg_index++] = tokens[i];
        }
    }
    commands[cmd_index][arg_index] = NULL;
    cmd_index++;

    run(commands, cmd_index);
    }
    return 0;
}

