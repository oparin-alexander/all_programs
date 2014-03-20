#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


double f( double x) 
{ 
	return x*x+x/4+1; 
}

double simple_integrate(double a, double b, double dx, double (*f)(double) )
{
	double result=0;
	for(double x=a; x<b; x+=dx)
		result+=(f(x) + f(x+dx))/2*dx;
	return result;
}

void mp_integrate(double a, double b, double dx, double (*f)(double) , int pn)// pn-process numbers
{
	int pipefd[2];
	double delta=(b-a)/pn;
	
	if (pipe(pipefd) == -1) {
		perror("pipe");
        exit(EXIT_FAILURE);
    }
	
	for(int i = 0; i<pn; ++i)
	{
		int cpid =fork();
		if (cpid==0) {
			close(pipefd[0]);
			double res = simple_integrate(a+delta*i, a+delta*(i+1), dx, f);
			write(pipefd[1], &res, sizeof(res));
			close(pipefd[1]);
			exit(0);
		}
		else if( cpid==-1 ) {               
			perror("fork");
			exit(EXIT_FAILURE);
		}
	}	
	
	close(pipefd[1]);
	double res, res_total=0;
	for(int i=0; i<pn; ++i)
	{
		read( pipefd[0], &res, sizeof(res) );
		res_total+=res;
	}
	close(pipefd[0]);
	printf("%lf", res_total);
}

int main(int argc, char** argv)
{	
	mp_integrate(0,10, 0.0000001, f, 10);
	return 0;
}
