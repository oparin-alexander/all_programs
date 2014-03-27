#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>  
#include <stdio.h>


#define NBYTE 1000
#define MAX_PATH_LENGTH 255
int r=0;

int open_file(char* fileName, int mask, int mode)
{
	int fd = open(fileName, mask, mode);
	if (fd<0){
		perror("open_file_error");
		exit(1);
	}
	return fd;
}

void my_copy(int fd1, int fd2)
{
	void* buff= malloc(NBYTE);
	int temp_n=0;
	
	while( ( temp_n=read(fd1, buff, NBYTE) ) != 0 )
	{	
		if (temp_n<0) 
			perror("copy_error");
		write(fd2, buff, temp_n);
	}
	
	free(buff);
}

void error_exit(char* error_text)
{
	printf("%s\n", error_text);
	exit(1);
}

//1-файл в файл, 2-что угодно в дир 
int read_args(int argc, char **argv, int *k)
{
	if(argc<3) error_exit("incorrect input: few arguments");
	
	struct stat buf;
	int begin=1;
	*k=1;
	
	if( strcmp(argv[1], "-r")==0 ) {
		 r=1;
		 begin=2;
	 }
	 else 
		 r=0;
	
	if ( stat(argv[argc-1], &buf)<0) 
	{
		if(argc==3) return 1;
		error_exit("incorrect input: dist path not found");
	}
	if ( S_ISDIR(buf.st_mode) ) {
		
		for(int i=begin; i<argc-1; ++i)
		{
			if ( stat(argv[i], &buf)<0) 
				error_exit("incorrect input: source path not found");
			if ( S_ISDIR(buf.st_mode) && r==0 ) 
				error_exit("incorrect input: source path is dir and not found -r");
		}
		
		*k=2;
	}
	else if(r==1 || argc!=3) 
		error_exit("incorrect input: for cp to file");
		
	return begin;
}

void copy_file(char* file1, char* file2)
{
	struct stat buf;
	
	if( stat(file1, &buf)<0) perror(file1); 
		
	int fd1 = open_file(file1, O_RDONLY, buf.st_mode);
	int fd2 = open_file(file2, O_CREAT|O_TRUNC|O_WRONLY, buf.st_mode);
	
	my_copy(fd1, fd2);
	
	close(fd1);
	close(fd2);
}

void make_cp(char *from_path, char *to_path)
{
	struct stat buf, buf1;
	
	DIR *dir=opendir(from_path);
	
	if(dir==NULL) {
		perror(from_path);
		return;
	}
	
	struct dirent *curr_dir;
	
    while ( (curr_dir = readdir(dir)) != NULL) 
    {	
		if (curr_dir->d_name[0]!='.') {
			strcat(to_path, "/");
			strcat(to_path, curr_dir->d_name); 			
			strcat(from_path, "/");
			strcat(from_path, curr_dir->d_name);	
			
			if (stat(from_path, &buf) < 0)
				perror(curr_dir->d_name);			
			else if( S_ISDIR(buf.st_mode) ) {
				if (stat(to_path, &buf1)<0)
					if( mkdir(to_path, buf.st_mode) )
						perror(to_path);
				make_cp(from_path, to_path);
			}
			else 
				copy_file(from_path, to_path);
			
			int i;
			for(i=strlen(from_path)-1; from_path[i]!='/' && i>0; --i) from_path[i]='\0';
			from_path[i]='\0';
			for(i=strlen(to_path)-1;  to_path[i]!='/' && i>0; --i) to_path[i]='\0';
			to_path[i]='\0';
		}
    }
    
	closedir(dir);
	return;	
}

int main(int argc, char **argv)
{
	int k=0;
	int begin=read_args(argc, argv, &k);
	if (k==1)
		copy_file(argv[argc-2], argv[argc-1]);
	else if (k==2) {
		char temp_str1[MAX_PATH_LENGTH];
		char temp_str2[MAX_PATH_LENGTH];

		struct stat buf, buf1;
		strcpy(temp_str2, argv[argc-1]);
		
		for(int i=begin; i<argc-1; ++i)
		{
			stat(argv[i], &buf);
			strcpy(temp_str1, argv[i]);
			strcpy(temp_str2, argv[argc-1]);
			
			int j=0;
			for( j=strlen(argv[i])-1; argv[i][j]!='/' && j>=0; --j);
			
			strcat(temp_str2, "/");
			strcat(temp_str2, &argv[i][j+1]);
			
			if( S_ISDIR(buf.st_mode) ) {
				if (stat(temp_str2, &buf1)<0)
					if( mkdir(temp_str2, buf.st_mode) )
						perror(temp_str2);
				make_cp (temp_str1, temp_str2);
			}
			else
				copy_file (temp_str1, temp_str2);
		}
	}
	exit(0);
}
