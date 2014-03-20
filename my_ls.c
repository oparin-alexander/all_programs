#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>  
#include <stdio.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define MAX_DEPTH 2

int r=0, l=0;

int change_dir(char *path)
{
	int suc=chdir(path);
	if (suc<0) 
		perror(path);
	return suc;
}

void read_args(int argc, char**argv)
{
	for (int i=1; i<argc; ++i)
	{
		if(strcmp(argv[i],"-l")==0) l=1;
		else if(strcmp(argv[i],"-r")==0) r=1;
		else if (change_dir(argv[i])<0) _exit(1);
	}
}

void print_mode(int mode)
{
	char str[11]="----------\0";
	if (S_ISDIR(mode)) str[0]='d';
	if (mode & S_IRUSR) str[1]='r';
	if (mode & S_IWUSR) str[2]='w';
	if (mode & S_IXUSR) str[3]='x';
	if (mode & S_IRGRP) str[4]='r';
	if (mode & S_IWGRP) str[5]='w';
	if (mode & S_IXGRP) str[6]='x';
	if (mode & S_IROTH) str[7]='r';
	if (mode & S_IWOTH) str[8]='w';
	if (mode & S_IXOTH) str[9]='x';
	printf("%s ", str);
}

	
char* uid_to_name(uid_t uid){   
	struct passwd  *pw_ptr;
	static char numstr[5];
	if((pw_ptr = getpwuid(uid)) == NULL) {
		sprintf(numstr, "%d", uid);
		return numstr;
	}
	else
		return pw_ptr->pw_name;
}

char* gid_to_name(gid_t gid){
	struct group *getgrgid(gid_t), *grp_ptr;
	static char numstr[5];
	if((grp_ptr = getgrgid(gid)) == NULL) {
		sprintf(numstr, "%d", gid);
		return numstr;
	}
	else    
		return grp_ptr->gr_name;
}

void print(struct dirent *dir, struct stat *buf, int depth)
{
	if (dir->d_name[0]=='.') return;
	for(int i=0; i<depth*4; ++i) printf(" ");
	if(l==1) {
		print_mode(buf->st_mode);
		printf("%d ", (int)buf->st_nlink);
		printf("%s ", uid_to_name(buf->st_uid));
		printf("%s ", gid_to_name(buf->st_gid));
		printf("%.12s ", 4+ctime(&(buf->st_mtime)));
		printf("%ld ", (long)buf->st_size); 
	}
	printf("%s\n", dir->d_name);
	return;
}

//возвращает 0, если не поменялась директория. 1 иначе
int ls( char *dirname, int depth)
{	int k;
	struct stat buf;
	if ( change_dir(dirname)<0 ) return 0; 
	DIR *dir=opendir("./");
	
	if(dir==NULL) {
		perror(dirname);
		return 1;
	}
	
	struct dirent *curr_dir;
	
    while ( (curr_dir = readdir(dir)) != NULL) 
    {
		if (stat(curr_dir->d_name, &buf) < 0) {
			perror(curr_dir->d_name);
			scanf("%d", &k);
		}
		else {
			print(curr_dir, &buf, depth);
			if (r==1 && S_ISDIR(buf.st_mode) && depth<MAX_DEPTH && curr_dir->d_name[0]!='.')
				if (ls(curr_dir->d_name, depth+1)==1)
					if (change_dir("./../")< 0) _exit(1);
		}
    }
    
	closedir(dir);
	return 1;	
}

int main(int argc, char **argv)
{
	read_args( argc, argv);
	ls("./", 0);
	_exit(0);
}
