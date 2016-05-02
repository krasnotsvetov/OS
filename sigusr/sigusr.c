#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
bool catch = false;
int sig;
pid_t t;
static void handler(int signum, siginfo_t *info, void *context) {
	if (!catch) {
		catch = true;
		t = info->si_pid;
		sig = signum;
	}	
}
 

int main(int argc, char* argv[]) {        
	struct sigaction action;
        sigemptyset(&action.sa_mask);
        
   	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = &handler;

	
	int i = 0;
        for (; i < 31; i++)
	{
		if (i != SIGKILL && i!= SIGSTOP && sigaddset(&action.sa_mask, i) == 0  && sigaction(i, &action, NULL) == 0) 
		{

		} else 
		{
			if (i == SIGKILL || i == SIGSTOP) {
				continue;			
			} 
		}
	}
	sleep(10);
	if (!catch) {
		printf("No signals were caught\n");
	} else 
	{
		printf("\n%d from %d\n", sig, t);
	}
	return 0;
}
