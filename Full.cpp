#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <limits>

using namespace std;

// Define maximum limits for commands, arguments, and tokens
#define MAX_COMMANDS 5
#define MAX_ARGS 10
#define MAX_TOKENS 50

// Simple exception class for shell errors
class ShellException {
    string msg;
public:
    ShellException(const string& m) : msg(m) {}
    const char* what() const noexcept { return msg.c_str(); }
};

// Function to execute commands, supporting pipelines and background execution
void run(char* commands[MAX_COMMANDS][MAX_ARGS + 1], int total, bool background) {
    int pipefd[2];  // Pipe file descriptors: [0] for reading, [1] for writing
    int input_fd = 0;  // File descriptor for input (starts as stdin)

    // Loop through each command
    for (int i = 0; i < total; i++) {
        try {
            // Create a pipe if this isn't the last command
            if (i < total - 1) {
                if (pipe(pipefd) < 0) {
                    throw ShellException("Pipe creation failed");
                }
            }

            // Fork a child process
            pid_t pid = fork();
            if (pid == 0) {  // Child process
                // Close stdin/stdout to avoid terminal interference
                if (i == 0 && input_fd == 0) {
                    close(1); // Close stdout in first child if no input redirect
                }
                // If not the first command, redirect input from previous pipe
                if (input_fd != 0) {
                    dup2(input_fd, 0);  // Redirect stdin to input_fd
                    close(input_fd);    // Close the original fd
                }
                // If not the last command, redirect output to pipe
                if (i < total - 1) {
                    dup2(pipefd[1], 1);  // Redirect stdout to pipe write end
                    close(pipefd[0]);    // Close read end (not needed in child)
                    close(pipefd[1]);    // Close write end after redirect
                }
                // Check if command is executable
                if (access(commands[i][0], X_OK) != 0) {
                    throw ShellException("Command not found: " + string(commands[i][0]));
                }
                // Execute the command
                execvp(commands[i][0], commands[i]);
                throw ShellException("Command execution failed: " + string(commands[i][0]));
            } else if (pid > 0) {  // Parent process
                // Close input_fd if it was set from a previous pipe
                if (input_fd != 0) {
                    close(input_fd);
                }
                // If not the last command, set up for next command
                if (i < total - 1) {
                    close(pipefd[1]);    // Close write end in parent
                    input_fd = pipefd[0];  // Save read end for next command
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
            exit(1); // Exit on error in run
        }
    }

    // Close the last input_fd if it exists (from the final pipe)
    if (input_fd != 0) {
        close(input_fd);
    }

    // Wait for all child processes if not background
    if (!background) {
        for (int i = 0; i < total; i++) {
            wait(NULL);  // Wait for each child to finish
        }
    }
}

int main() {
    char input[100];  // Buffer for user input
    char* tokens[MAX_TOKENS];  // Array to store tokenized input
    char* commands[MAX_COMMANDS][MAX_ARGS + 1];  // 2D array for commands and args

    // Main shell loop
    while (true) {
        try {
            cout << "Shell> ";  // Display prompt
            cin.getline(input, 100);  // Read user input
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw ShellException("Input reading failed");
            }

            // Check for termination command
            if (strcmp(input, "quit") == 0) {
                cout << "Exiting shell" << endl;
                break;  // Exit the loop
            }

            // Handle built-in commands
            if (strncmp(input, "cd ", 3) == 0) {
                char* path = input + 3;
                if (chdir(path) != 0) {
                    cerr << "Error: cd failed" << endl;
                }
                continue;
            }
            if (strcmp(input, "exit") == 0) {
                cout << "Exiting shell" << endl;
                break;
            }

            // Tokenize the input
            int token_count = 0;
            char* token = strtok(input, " ");  // Split by spaces
            while (token != NULL && token_count < MAX_TOKENS) {
                tokens[token_count++] = token;
                token = strtok(NULL, " ");
            }

            // Skip empty input
            if (token_count == 0) {
                continue;
            }

            // Check for background execution
            bool background = false;
            if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
                background = true;
                token_count--;  // Remove '&' from tokens
            }

            // Parse commands separated by '|'
            int cmd_index = 0;  // Index for current command
            int arg_index = 0;  // Index for arguments in current command
            for (int i = 0; i < token_count; i++) {
                if (strcmp(tokens[i], "|") == 0) {
                    // Check for empty command before '|'
                    if (arg_index == 0) {
                        throw ShellException("Empty command in pipeline");
                    }
                    commands[cmd_index][arg_index] = NULL;  // Null-terminate args
                    cmd_index++;  // Move to next command
                    arg_index = 0;  // Reset arg index
                } else {
                    // Check argument limit
                    if (arg_index >= MAX_ARGS) {
                        throw ShellException("Too many arguments");
                    }
                    commands[cmd_index][arg_index++] = tokens[i];  // Add argument
                }
            }

            // Check for empty command at the end
            if (arg_index == 0) {
                throw ShellException("Empty command");
            }
            commands[cmd_index][arg_index] = NULL;  // Null-terminate last command
            cmd_index++;  // Total number of commands

            // Execute the parsed commands
            run(commands, cmd_index, background);
        } catch (const ShellException& e) {
            cerr << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue; // Skip to next iteration
        }
    }

    return 0;  // Exit program
}
