#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

const int sleep_factor=100;

void handler(int sign) 
{
		int pid;
		int stat;
		pid = waitpid(-1, &stat, WNOHANG);
		if (pid>0)
			exit(1);			
}

int cpu_limit(int argc, char** argv, int ratio)
{
	int cpid=fork();
    
    if(cpid==0) {
		if (execv(argv[2], argv+2)<0) {
			perror("exec");
			exit(1);
		}
        exit(0);
    }
    else if(cpid==-1) {
		perror("fork");
		exit (1);
	}
	
	while(1)
	{
		kill(cpid, SIGSTOP);
		usleep((100-ratio)*sleep_factor);
		kill(cpid, SIGCONT);
		usleep((ratio)*sleep_factor);
	}	
}

int main(int argc, char** argv)
 {
	char** endptr=NULL;
	unsigned long ratio = strtoul(argv[1], endptr, 10);
	
	if (endptr!=NULL) {
		printf("limit_error\n");
		exit(1);
	} 
	 
	if (argc<3) {
		printf("Too few argaments\n");
		exit(1);
	}
  
	if (signal(SIGCHLD, handler)==SIG_ERR)
		perror("SIGCHLD: ");
	
	cpu_limit(argc, argv, ratio);
	
	exit(0);
}
