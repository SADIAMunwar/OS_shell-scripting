#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;

int main() {
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    int pipefd1[2], pipefd2[2];
    if (pipe(pipefd1) < 0 || pipe(pipefd2) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // First child: ls
        close(pipefd1[0]); // Close unused read end
        close(pipefd2[0]);
        close(pipefd2[1]);

        dup2(pipefd1[1], STDOUT_FILENO);
        close(pipefd1[1]);

        execlp("ls", "ls", NULL);
        perror("execlp ls");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Second child: sort
        close(pipefd1[1]); // Close unused write end
        close(pipefd2[0]); // Close unused read end

        dup2(pipefd1[0], STDIN_FILENO);
        dup2(pipefd2[1], STDOUT_FILENO);
        close(pipefd1[0]);
        close(pipefd2[1]);

        execlp("sort", "sort", NULL);
        perror("execlp sort");
        exit(1);
    }

    pid_t pid3 = fork();
    if (pid3 == 0) {
        // Third child: wc
        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[1]); // Close unused write end

        dup2(pipefd2[0], STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        close(pipefd2[0]);
        close(fd);

        execlp("wc", "wc", NULL);
        perror("execlp wc");
        exit(1);
    }

    // Parent: close all pipe ends
    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);
    close(fd);

    // Wait for all child processes
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}

