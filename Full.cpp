#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <limits>

using namespace std;

#define MAX_COMMANDS 5
#define MAX_ARGS 10
#define MAX_TOKENS 50

// Custom error handling class
class ShellException {
    string msg;
public:
    ShellException(const string& m) : msg(m) {}
    const char* what() const noexcept { return msg.c_str(); }
};

// Executes multiple piped commands with optional background execution
void run(char* commands[MAX_COMMANDS][MAX_ARGS + 1], int total, bool background) {
    int pipefd[2];
    int input_fd = 0;

    for (int i = 0; i < total; i++) {
        try {
            if (i < total - 1) {
                if (pipe(pipefd) < 0) {
                    throw ShellException("Pipe creation failed");
                }
            }

            pid_t pid = fork();
            if (pid == 0) {
                // Child process

                // Redirect input from previous pipe if needed
                if (input_fd != 0) {
                    dup2(input_fd, 0);
                    close(input_fd);
                }

                // Redirect output to pipe if not the last command
                if (i < total - 1) {
                    dup2(pipefd[1], 1);
                    close(pipefd[0]);
                    close(pipefd[1]);
                }

                // Attempt to execute the command
                execvp(commands[i][0], commands[i]);

                // If execvp fails
                cerr << "Error: Command not found: " << commands[i][0] << endl;
                exit(1);
            } else if (pid > 0) {
                // Parent process

                // Close input if set
                if (input_fd != 0) {
                    close(input_fd);
                }

                // Set input for next command
                if (i < total - 1) {
                    close(pipefd[1]);
                    input_fd = pipefd[0];
                }
            } else {
                throw ShellException("Fork failed");
            }
        } catch (const ShellException& e) {
            cerr << "Error: " << e.what() << endl;
            if (input_fd != 0) close(input_fd);
            if (i < total - 1) {
                close(pipefd[0]);
                close(pipefd[1]);
            }
            exit(1);
        }
    }

    if (input_fd != 0) {
        close(input_fd);
    }

    if (!background) {
        for (int i = 0; i < total; i++) {
            wait(NULL);
        }
    }
}

int main() {
    char input[100];
    char* tokens[MAX_TOKENS];
    char* commands[MAX_COMMANDS][MAX_ARGS + 1];

    while (true) {
        try {
            cout << "Shell> ";
            cin.getline(input, 100);

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw ShellException("Input reading failed");
            }

            // Exit shell
            if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
                cout << "Exiting shell" << endl;
                break;
            }

            // Built-in 'cd'
            if (strncmp(input, "cd ", 3) == 0) {
                char* path = input + 3;
                if (chdir(path) != 0) {
                    cerr << "Error: cd failed" << endl;
                }
                continue;
            }

            // Tokenize input
            int token_count = 0;
            char* token = strtok(input, " ");
            while (token != NULL && token_count < MAX_TOKENS) {
                tokens[token_count++] = token;
                token = strtok(NULL, " ");
            }

            if (token_count == 0) continue;

            // Check for background execution
            bool background = false;
            if (strcmp(tokens[token_count - 1], "&") == 0) {
                background = true;
                token_count--;
            }

            // Parse commands from tokens
            int cmd_index = 0;
            int arg_index = 0;
            for (int i = 0; i < token_count; i++) {
                if (strcmp(tokens[i], "|") == 0) {
                    if (arg_index == 0) {
                        throw ShellException("Empty command in pipeline");
                    }
                    commands[cmd_index][arg_index] = NULL;
                    cmd_index++;
                    arg_index = 0;
                } else {
                    if (arg_index >= MAX_ARGS) {
                        throw ShellException("Too many arguments");
                    }
                    commands[cmd_index][arg_index++] = tokens[i];
                }
            }

            if (arg_index == 0) {
                throw ShellException("Empty command");
            }

            commands[cmd_index][arg_index] = NULL;
            cmd_index++;

            // Run the parsed command(s)
            run(commands, cmd_index, background);

        } catch (const ShellException& e) {
            cerr << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    return 0;
}
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <limits>

using namespace std;

#define MAX_COMMANDS 5
#define MAX_ARGS 10
#define MAX_TOKENS 50

// Custom error handling class
class ShellException {
    string msg;
public:
    ShellException(const string& m) : msg(m) {}
    const char* what() const noexcept { return msg.c_str(); }
};

// Executes multiple piped commands with optional background execution
void run(char* commands[MAX_COMMANDS][MAX_ARGS + 1], int total, bool background) {
    int pipefd[2];
    int input_fd = 0;

    for (int i = 0; i < total; i++) {
        try {
            if (i < total - 1) {
                if (pipe(pipefd) < 0) {
                    throw ShellException("Pipe creation failed");
                }
            }

            pid_t pid = fork();
            if (pid == 0) {
                // Child process

                // Redirect input from previous pipe if needed
                if (input_fd != 0) {
                    dup2(input_fd, 0);
                    close(input_fd);
                }

                // Redirect output to pipe if not the last command
                if (i < total - 1) {
                    dup2(pipefd[1], 1);
                    close(pipefd[0]);
                    close(pipefd[1]);
                }

                // Attempt to execute the command
                execvp(commands[i][0], commands[i]);

                // If execvp fails
                cerr << "Error: Command not found: " << commands[i][0] << endl;
                exit(1);
            } else if (pid > 0) {
                // Parent process

                // Close input if set
                if (input_fd != 0) {
                    close(input_fd);
                }

                // Set input for next command
                if (i < total - 1) {
                    close(pipefd[1]);
                    input_fd = pipefd[0];
                }
            } else {
                throw ShellException("Fork failed");
            }
        } catch (const ShellException& e) {
            cerr << "Error: " << e.what() << endl;
            if (input_fd != 0) close(input_fd);
            if (i < total - 1) {
                close(pipefd[0]);
                close(pipefd[1]);
            }
            exit(1);
        }
    }

    if (input_fd != 0) {
        close(input_fd);
    }

    if (!background) {
        for (int i = 0; i < total; i++) {
            wait(NULL);
        }
    }
}

int main() {
    char input[100];
    char* tokens[MAX_TOKENS];
    char* commands[MAX_COMMANDS][MAX_ARGS + 1];

    while (true) {
        try {
            cout << "Shell> ";
            cin.getline(input, 100);

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw ShellException("Input reading failed");
            }

            // Exit shell
            if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
                cout << "Exiting shell" << endl;
                break;
            }

            // Built-in 'cd'
            if (strncmp(input, "cd ", 3) == 0) {
                char* path = input + 3;
                if (chdir(path) != 0) {
                    cerr << "Error: cd failed" << endl;
                }
                continue;
            }

            // Tokenize input
            int token_count = 0;
            char* token = strtok(input, " ");
            while (token != NULL && token_count < MAX_TOKENS) {
                tokens[token_count++] = token;
                token = strtok(NULL, " ");
            }

            if (token_count == 0) continue;

            // Check for background execution
            bool background = false;
            if (strcmp(tokens[token_count - 1], "&") == 0) {
                background = true;
                token_count--;
            }

            // Parse commands from tokens
            int cmd_index = 0;
            int arg_index = 0;
            for (int i = 0; i < token_count; i++) {
                if (strcmp(tokens[i], "|") == 0) {
                    if (arg_index == 0) {
                        throw ShellException("Empty command in pipeline");
                    }
                    commands[cmd_index][arg_index] = NULL;
                    cmd_index++;
                    arg_index = 0;
                } else {
                    if (arg_index >= MAX_ARGS) {
                        throw ShellException("Too many arguments");
                    }
                    commands[cmd_index][arg_index++] = tokens[i];
                }
            }

            if (arg_index == 0) {
                throw ShellException("Empty command");
            }

            commands[cmd_index][arg_index] = NULL;
            cmd_index++;

            // Run the parsed command(s)
            run(commands, cmd_index, background);

        } catch (const ShellException& e) {
            cerr << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    return 0;
}
