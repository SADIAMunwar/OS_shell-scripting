#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <fstream>
#include <sstream>
using namespace std;

#define MAX_COMMANDS 5
#define MAX_ARGS 10
#define MAX_TOKENS 50

// Simple error class for throwing errors
class ShellError {
    string message;
public:
    ShellError(string msg) : message(msg) {}
    string getMessage() { return message; } // Easy way to get error message
};

// Run commands with pipes and background option
void run(char* commands[MAX_COMMANDS][MAX_ARGS + 1], int total, bool background) {
    int pipefd[2];
    int input_fd = 0;

    for (int i = 0; i < total; i++) {
        try {
            // Create pipe if not the last command
            if (i < total - 1) {
                if (pipe(pipefd) < 0) {
                    throw ShellError("Arre, pipe toot gaya! 😓");
                }
            }

            pid_t pid = fork();
            if (pid == 0) { // Child process
                if (input_fd != 0) {
                    dup2(input_fd, 0); // Take input from previous pipe
                    close(input_fd);
                }
                if (i < total - 1) {
                    dup2(pipefd[1], 1); // Send output to pipe
                    close(pipefd[0]);
                    close(pipefd[1]);
                }
                execvp(commands[i][0], commands[i]);
                cerr << "Oops, command '" << commands[i][0] << "' went on a vacation! 😜" << endl;
                exit(1);
            } else if (pid > 0) { // Parent process
                if (input_fd != 0) close(input_fd);
                if (i < total - 1) {
                    close(pipefd[1]);
                    input_fd = pipefd[0];
                }
            } else {
                throw ShellError("Fork failed, child very bad! 😅");
            }
        } catch (ShellError& e) {
            cerr << "Error: " << e.getMessage() << endl;
            if (input_fd != 0) close(input_fd);
            if (i < total - 1) {
                close(pipefd[0]);
                close(pipefd[1]);
            }
            exit(1);
        }
    }

    if (input_fd != 0) close(input_fd);

    if (!background) {
        for (int i = 0; i < total; i++) wait(NULL); // Wait for kids to finish
    } else {
        cout << "Chal raha hai background mein, chill karo! 😎" << endl;
    }
}

// Show memory usage for a process (Week 10: Address Space)
void show_memstat(int pid) {
    string path = "/proc/" + to_string(pid) + "/stat";
    ifstream file(path);
    if (!file) {
        cerr << "Nahi mila /proc ke andar PID " << pid << "! 😕" << endl;
        return;
    }
    string line;
    getline(file, line);
    stringstream ss(line);
    string stats[52];
    int i = 0;
    while (ss >> stats[i] && i < 52) i++;
    if (i >= 24) {
        cout << "PID " << pid << " kharcha: " << stats[23] << " kB memory! 💾" << endl;
    } else {
        cerr << "Stat file mein kuch gadbad hai! 😫" << endl;
    }
}

int main() {
    char input[100];
    char* tokens[MAX_TOKENS];
    char* commands[MAX_COMMANDS][MAX_ARGS + 1];

    while (true) {
        try {
            cout << "😎 ShellBhai > ";
            cin.getline(input, 100);

            if (!cin) {
                cin.clear();
                cin.ignore(1000, '\n'); // Simple way to clear input
                throw ShellError("Input padhne mein problem, bhai! 😟");
            }

            // Exit the party
            if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
                cout << "ShellBhai bolta hai: Alvida, dost! 👋" << endl;
                break;
            }

            // Built-in 'cd' to change directory
            if (strncmp(input, "cd ", 3) == 0) {
                char* path = input + 3;
                if (chdir(path) != 0) {
                    cerr << "Error: Jagah nahi mili '" << path << "'! 🏠" << endl;
                }
                continue;
            }

            // Split input into tokens
            int token_count = 0;
            char* token = strtok(input, " ");
            while (token && token_count < MAX_TOKENS) {
                tokens[token_count++] = token;
                token = strtok(NULL, " ");
            }
            if (token_count == 0) continue;

            // Check for background mode
            bool background = false;
            if (strcmp(tokens[token_count - 1], "&") == 0) {
                background = true;
                token_count--;
            }

            // Check for 'memstat' command
            if (token_count == 2 && strcmp(tokens[0], "memstat") == 0) {
                try {
                    int pid = stoi(tokens[1]);
                    show_memstat(pid);
                } catch (...) {
                    cerr << "PID number hi galat daal diya, bhai! 😜" << endl;
                }
                continue;
            }

            // Parse tokens into commands
            int cmd_index = 0, arg_index = 0;
            for (int i = 0; i < token_count; i++) {
                if (strcmp(tokens[i], "|") == 0) {
                    if (arg_index == 0) {
                        throw ShellError("Pipe ke pehle command to daal, yaar! 😣");
                    }
                    commands[cmd_index][arg_index] = NULL;
                    cmd_index++;
                    arg_index = 0;
                    if (cmd_index >= MAX_COMMANDS) {
                        throw ShellError("Itne saare commands? Thodi si shanti rakh! 😬");
                    }
                } else {
                    if (arg_index >= MAX_ARGS) {
                        throw ShellError("Bohot arguments daal diye, bhai! 😵");
                    }
                    commands[cmd_index][arg_index++] = tokens[i];
                }
            }

            if (arg_index == 0) {
                throw ShellError("Command hi nahi diya, kya karu main? 😕");
            }

            commands[cmd_index][arg_index] = NULL;
            cmd_index++;

            // Let's roll the commands!
            run(commands, cmd_index, background);

        } catch (ShellError& e) {
            cerr << "Error: " << e.getMessage() << endl;
            cin.clear();
            cin.ignore(1000, '\n'); // Clear input buffer
        }
    }

    return 0;
}
