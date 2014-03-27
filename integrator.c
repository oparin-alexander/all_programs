#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


double f( double x) 
{ 
	return x*x+x/4+1; 
}

double simple_integrate(double a, double b, double dx)
{
	double result=0;
	for(double x=a; x<b; x+=dx)
		result+=(f(x) + f(x+dx))/2*dx;
	return result;
}

void multi_integrate(double a, double b, double dx,  int pn)
{
	int pipefd[2];
	double delta=(b-a)/pn;
	
	if (pipe(pipefd)<0) {
		perror("pipe");
        exit(1);
    }
	
	for(int i = 0; i<pn; ++i)
	{
		int cpid =fork();
		if (cpid==0) {
			close(pipefd[0]);
			double res = simple_integrate(a+delta*i, a+delta*(i+1), dx);
			write(pipefd[1], &res, sizeof(double));
			close(pipefd[1]);
			exit(0);
		}
		else if( cpid==-1 ) {               
			perror("fork");
			exit(1);
		}
	}	
	
	close(pipefd[1]);
	double temp_res=0, result=0;
	for(int i=0; i<pn; ++i)
	{
		read( pipefd[0], &temp_res, sizeof(double) );
		result+=temp_res;
	}
	close(pipefd[0]);
	printf("%lf\n", result);
}

int main(int argc, char** argv)
{	
	//ввод из терминала: a b dx num_of_process
	if(argc<5){
		printf("incorrect input: few arguments\n");
		exit(1);
	}
	multi_integrate(atof(argv[1]), atof(argv[2]), atof(argv[3]), atoi(argv[4]));
	exit(0);
}
