#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

#define MAX_ARGS 10

void execute_command(char* args[], bool background) {
    pid_t pid = fork();
    if (pid < 0) {
        cerr << "Error: Fork failed" << endl;
        return;
    } else if (pid == 0) {
        execvp(args[0], args);
        cerr << "Error: Command '" << args[0] << "' not found" << endl;
        exit(1);
    } else {
        if (!background) {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                cerr << "Error: Command exited with status " << WEXITSTATUS(status) << endl;
            }
        } else {
            cout << "Running in background (PID: " << pid << ")" << endl;
        }
    }
}

int main() {
    char input[100];
    char* args[MAX_ARGS + 1];
    int arg_count;
    bool background;

    while (true) {
        cout << "Shell> ";
        cin.getline(input, 100);

        if (strcmp(input, "q") == 0) break;

        arg_count = 0;
        background = false;

        // Tokenize
        char* token = strtok(input, " ");
        while (token != NULL && arg_count < MAX_ARGS) {
            if (strcmp(token, "&") == 0) {
                background = true;
                break;
            }
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        if (arg_count == 0) {
            cerr << "Error: No command entered" << endl;
            continue;
        }

        execute_command(args, background);
    }

    return 0;
}

