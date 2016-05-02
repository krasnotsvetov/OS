#include <sys/wait.h>

#include <stdio.h>

#include <unistd.h>

#include <stdlib.h>

#include <signal.h>
#include <stdio.h>

#include <errno.h>

#include <iostream>

#include <string>
#include <cstring>

#include <vector>

#define BUFFER_SIZE 1024
#define PIPESAMOUNT 1000
#define TOKENSAMOUNT 1000
#define ARGSAMOUNT 1000
#define PROCESSESAMOUNT 1000

using namespace std;

bool needSendSignal = false;
pid_t son_pid;

void swap(char* buffer, int size) {
    int i;
    for (i = 0; i < size / 2; i++) {
        char t = buffer[i];
        buffer[i] = buffer[size - i - 1];
        buffer[size - i -1] = t;

    }
}

int printNum(int fd, int num)
{
    const int bufferSize = 22;
    char buffer[bufferSize];
    int size = 0;
    while (num > 0)
    {

        char t = (char)((int)'0' + num % 10);
        buffer[size] = t;
        size++;
        num /= 10;
    }
    swap(buffer, size);
    buffer[size] = '\n';
    write(fd, buffer, size + 1);
}

pid_t processes[PROCESSESAMOUNT];
int processesCount = 0;



pid_t start (char *cmd, char *args[], int fdIn, int fdOut) {
    pid_t pid = fork();
    if (pid == 0) {
        if (fdIn != STDIN_FILENO) {
            dup2(fdIn, STDIN_FILENO);
            close(fdIn);
        }
        if (fdOut != STDOUT_FILENO) {
            dup2(fdOut, STDOUT_FILENO);
            close(fdOut);
        }
        execvp(cmd, args);
        return -1;
    } else {
        if (fdIn != STDIN_FILENO) {
            close(fdIn);
        }
        if (fdOut != STDOUT_FILENO) {
            close(fdOut);
        }
        return pid;
    }
}


void execute(char* buffer)  {
    int pipefd[PIPESAMOUNT][2];
    char *tokens[TOKENSAMOUNT];
    char *cmd[PROCESSESAMOUNT], *args[PROCESSESAMOUNT][ARGSAMOUNT];
    char *tmp = strtok(buffer, " \n");
    size_t tokenAmount = 0;
    size_t cmdAmount = 1;
    while (tmp != NULL) {
        if (tmp[0] == '|') {

            cmdAmount++;
        }
        tokens[tokenAmount++] = tmp;
        tmp = strtok(NULL, " \n");
    }
    int cmdNow = 0;
    for (int i = 0; i < tokenAmount; i++) {
        cmd[cmdNow] = tokens[i];

        args[cmdNow][0] = tokens[i];
        i++;
        int argNow = 1;
        while (i < tokenAmount && tokens[i][0] != '|') {
            args[cmdNow][argNow++] = tokens[i++];
        }
        args[cmdNow][argNow] = NULL;
        //i++;
        cmdNow++;

    }
    //write(0, cmd[1], 4);
    pid_t task;
    int task_status;
    for (int i = 0; i < cmdAmount; i++) {
        if (i != cmdAmount - 1) {
            pipe(pipefd[i]);
        }
        if (i == 0) {
            if (i == cmdAmount - 1) {
                task = start(cmd[i], args[i], STDIN_FILENO, STDOUT_FILENO);
            }
            else {
                task = start(cmd[i], args[i], STDIN_FILENO, pipefd[i][1]);
            }
        } else if (i == cmdAmount - 1) {
            task = start(cmd[i], args[i], pipefd[i - 1][0], STDOUT_FILENO);
        } else {
            task = start(cmd[i], args[i], pipefd[i - 1][0], pipefd[i][1]);
        }
        if (task != -1) {
            processes[processesCount++] = task;
            waitpid(task, &task_status, 0);
        }
    }
}

int main(int argc, char* argv[])
{
    char buffer[BUFFER_SIZE];
    char command[BUFFER_SIZE];
    write(0, "$ ", 2);
    int rd = 1;
    int done = 0;
    while ((rd = read(STDIN_FILENO, buffer + done, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < done + rd; i++) {
            if (buffer[i] == '\n') {
                memset(command, 0, sizeof(char) * (BUFFER_SIZE));
                memmove(command, buffer, sizeof(char) * i);
                execute(command);
                memmove(buffer, buffer + i + 1, sizeof(char) * (done + rd - 1 - i));
                memset(buffer + (done + rd - 1 - i), 0, sizeof(char) * (BUFFER_SIZE - (done + rd - 1 - i)));

                done = (done + rd - 1 - i);
                rd = 0;
                if (done == 0) {
                    write(0, "$ ", 2);
                }
                break;
            }
        }
        done += rd;
        if (done == BUFFER_SIZE) {
            break;
        }
    }
}
