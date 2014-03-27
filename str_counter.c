#include <string.h>
#include <malloc.h>

#include <errno.h>
#include <stdlib.h> 
#include <unistd.h>
#include <stdio.h>

#define NBYTE 1000

int main(int argc, char**argv)
{
	if( argc<2 ) {
		printf("incorrect input\n");
		exit(1);
	}
	
	int pipefd[2];
	
	if (pipe(pipefd)<0) {
		perror("pipe");
        exit(1);
	}
	
	int cpid=fork();
	
	if( cpid==0 ) {
		close(pipefd[0]);

		if( dup2(pipefd[1], 1)< 0 ) {
			perror("dup2");
			exit(1);
		}

		if (execv(argv[1], argv+1)<0) {
			perror("execvp");
			exit(1);
		}
		close(pipefd[1]);
		exit(0);
	}
	else if( cpid<0 ) {               
		perror("fork");
		exit(1);
	}
	
	close(pipefd[1]);
	char buff [NBYTE];
	int num_str=0, temp_n;
	while ((temp_n=read(pipefd[0], buff, NBYTE))!=0)
	{
			write (1, buff, temp_n);
			for(int i=0; i<temp_n; ++i)
				if(buff[i]=='\n') ++num_str;
	}

	printf("-----------\nnum_of_str: %d\n", num_str);

	exit(0);
}
