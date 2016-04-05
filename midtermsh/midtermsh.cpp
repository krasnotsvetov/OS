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
    const int bufferSize = 18;
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

void sighandler(int sig, siginfo_t *siginfo, void *context) {
    if (needSendSignal) {
        kill(son_pid, SIGINT);
    }
}


void splitPipe(string s, vector<string>& commands) {

    string command = "";
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] == '|') {
            commands.push_back(command.substr(0, i - 1));
            i++;
            command = "";
        } else {
            command += s[i];
        }

    }
    commands.push_back(command);
}
int main(int argc, char* argv[])
{


    struct sigaction action;
    memset(&action, 0 , sizeof(action));
    action.sa_sigaction = &sighandler;
    action.sa_flags = SA_SIGINFO;
    const int tempBufferSize = 1;
    char tempBuffer[tempBufferSize];

    const int bufferSize = 8192;
    char buffer[bufferSize];

    int realSize;
    while (true) {
        bool needBreak = false;
        write(1, "$\n", 2);
        realSize = 0;
        int t = read(0, tempBuffer, tempBufferSize);

        if (t == 0) {
            break;
        }

        //printNum(1, t);

        while (t > 0) {
            int i;
            for (i = realSize; i < realSize + t; i++) {
                buffer[i] = tempBuffer[i - realSize];
            }
            realSize += t;
            if (buffer[realSize - 1] == '\n') {
                realSize--;
                break;
            }

            t = read(0, tempBuffer, tempBufferSize);
            if (t == 0) {
                needBreak = true;
                break;
            }
        }
        if (needBreak) {
            break;
        }

        string allCommand = "";
        for (int i = 0; i < realSize; i++)
        {
            allCommand += buffer[i];

        }
        vector<string> pipeCommands(0);
        splitPipe(allCommand, pipeCommands);
        int fd_in = 0;
        int fd_out = 1;
        for (int p = 0; p < pipeCommands.size(); p++) {

            printNum(1, p);
            allCommand = pipeCommands[p];
            vector<string> commands(0);
            commands.clear();
            string command = "";
            int amount = 0;
            for (int i = 0; i < allCommand.length(); i++)
            {
                if (allCommand[i] == '\'') {
                amount++;
                } else
                if (allCommand[i] == ' ' && amount % 2 == 0) {
                    commands.push_back(command);
                    command = "";
                } else {
                    command += allCommand[i];
                }

            }
            commands.push_back(command);


            int fd[2];
            pipe(fd);
            if (p != pipeCommands.size() - 1) {
                fd_out = fd[1];
            } else {
                fd_out = 1;
            }

            son_pid = fork();
            int comm_stat;
            if (son_pid == 0) {

                needSendSignal = false;
                    execvp(commands[0].c_str(), (char* const*)commands.data());
                return 0;

            } else {
                needSendSignal = true;
                wait(&son_pid);
                needSendSignal = false;
            }
            fd_in = fd[0];
        }
        //wait(pid);
        //system(buffer);

    }
}
