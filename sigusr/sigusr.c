#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
bool catch = false;
static void handler(int signum, siginfo_t *info, void *context) {
	if (!catch) {
		catch = true;
		printf("SIGUSR%d from %d\n", signum == SIGUSR1 ? 1 : 2, info->si_pid);
	}
}
 

int main(int argc, char* argv[]) {        
	struct sigaction action;
        sigemptyset(&action.sa_mask);
        sigaddset(&action.sa_mask, SIGUSR1);
        sigaddset(&action.sa_mask, SIGUSR2);
   	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = &handler;

	sigaction(SIGUSR1, &action, NULL);
	sigaction(SIGUSR2, &action, NULL);
	sleep(10);
	if (!catch) {
		printf("No signals were caught\n");
	}
	return 0;
}
