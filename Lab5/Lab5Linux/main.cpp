#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstring>
#include <cerrno>

void CreateProcessWithPipes(const char *processName, int readPipe, int writePipe) {
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Fork failed!" << std::endl;
        _exit(1);
    }
    if (pid == 0) {
        if (readPipe != STDIN_FILENO) {
            if (dup2(readPipe, STDIN_FILENO) == -1) {
                perror("dup2 input failed");
                _exit(1);
            }
            close(readPipe);
        }

        if (writePipe != STDOUT_FILENO) {
            if (dup2(writePipe, STDOUT_FILENO) == -1) {
                perror("dup2 output failed");
                _exit(1);
            }
            close(writePipe);
        }

        execlp(processName, processName, nullptr);
        perror("execlp failed");
        _exit(1);
    }
}

int main() {
    int pipe1[2];
    int pipe2[2];
    int pipe3[2];



    int fileIn = open("../input.txt", O_RDONLY);

    if (fileIn < 0) {
        perror("Error opening input.txt");
        return 1;
    }

    if (pipe(pipe1) == -1) {
        std::cerr << "Error creating pipe1" << std::endl;
        return 1;
    }
    CreateProcessWithPipes("./ProcessM", fileIn, pipe1[1]);
    close(fileIn);
    close(pipe1[1]);

    if (pipe(pipe2) == -1) {
        std::cerr << "Error creating pipe2" << std::endl;
        return 1;
    }
    CreateProcessWithPipes("./ProcessA", pipe1[0], pipe2[1]);
    close(pipe1[0]);
    close(pipe2[1]);

    if (pipe(pipe3) == -1) {
        std::cerr << "Error creating pipe3" << std::endl;
        return 1;
    }
    CreateProcessWithPipes("./ProcessP", pipe2[0], pipe3[1]);
    close(pipe2[0]);
    close(pipe3[1]);

    CreateProcessWithPipes("./ProcessS", pipe3[0], STDOUT_FILENO);
    close(pipe3[0]);

    for (int i = 0; i < 4; i++) {
        wait(nullptr);
    }

    return 0;
}
