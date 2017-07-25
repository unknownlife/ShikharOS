#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include<sys/stat.h>
int no_reprint;
pid_t pid;
char* path= "/bin/";
char progpath[20];
#define TRUE 1
#define FALSE !TRUE
#define LIMIT 256
#define MAXLINE 1024
static char* currentDirectory;
int changeDirectory(char * args[]);
void shellPrompt(){

	char hostn[1204] = "";
	gethostname(hostn, sizeof(hostn));
	printf("%s@%s %s > ", getenv("LOGNAME"), hostn, getcwd(currentDirectory, 1024));
}
int changeDirectory(char* args[]){

	if (args[1] == NULL) {
		chdir(getenv("HOME"));
		return 1;
	}


	else{
		if (chdir(args[1]) == -1) {
			printf(" %s: no such directory\n", args[1]);
            return -1;
		}
	}
	return 0;
}
int touch(char* args[])
{
    if(args[1]==NULL)
 { printf("\enter file name ");
    return 0;
 }
 int fd;
char buff[50];
fd=open(args[1],O_CREAT | O_WRONLY,0600);
 if(fd==-1)
 {
 printf("\n failed to create file ");
 return 0;
 }
printf("\n enter text you want to write  : ");
scanf("%s",buff);
int l=strlen(buff);
write(fd,buff,l);
close(fd);
 return 1;
}
int copy(char* args[])
{
 if(args[1]==NULL)
 { printf("\enter file name ");
    return 0;
 }
 int fd1,fd2,l;
char buff[50];
fd1=open(args[1],O_RDONLY);
if(fd1==-1)
 {printf("\n failed to open source file ");
  return 0;
 }
read(fd1,buff,50);
l=strlen(buff);
buff[l]='\0';
close(fd1);

fd2=open(args[2],O_CREAT | O_WRONLY,0600);
 if(fd2==-1)
 {
 printf("\n failed to create file ");
 return 0;
 }


   l=strlen(buff);
  write(fd2,buff,l);
   close(fd2);
   return 1;
}

int commandHandler(char * args[]){
	int i = 0;
	int j = 0;
	int aux;
	int background = 0;

     if(strcmp(args[0],"exit") == 0)
           exit(0);

   else if (strcmp(args[0],"pwd") == 0){

     printf("%s\n", getcwd(currentDirectory, 1024));

         }

      else if (strcmp(args[0],"clear") == 0) system("clear");

	else if (strcmp(args[0],"cd") == 0)
         changeDirectory(args);

     else if (strcmp(args[0],"mkdir") == 0)
      { int ret;

      if(ret=mkdir(args[1],700)!=-1)
        {
          printf("\n Directory created successfully \n");
        }

      }
      else if(strcmp(args[0],"rmdir")==0)
      {   int ret;
           if(ret=rmdir(args[1])!=-1)
        {
          printf("\n directory deleted successfully \n");
        }

       }
      else if(strcmp(args[0],"touch")==0)
     {  touch(args);
     }
     else if(strcmp(args[0],"cp")==0)
      copy(args);

     else
    {

    strcpy(progpath, path);
    strcat(progpath, args[0]);
    for(i=0; i<strlen(progpath); i++){
        if(progpath[i]=='\n'){
            progpath[i]='\0';
        }
    }
    int pid= fork();
    if(pid==0){
        execvp(progpath,args);
        fprintf(stderr, "command cannot be executed\n");

    }else{
        wait(NULL);
       // printf("Child exited\n");
    }



    }



    return 1;
}


// Main method of our shell
int main(int argc, char *argv[]) {
	char line[MAXLINE];
	char * tokens[LIMIT];
	int numTokens;

	printf("\n \n");
        printf(" WELCOME TO SHELL\n ");


	while(TRUE){
		  printf("\n");
		 shellPrompt();


		memset ( line, '\0', MAXLINE );

		fgets(line, MAXLINE, stdin);

		if((tokens[0] = strtok(line," \n\t")) == NULL) continue;

		 numTokens = 1;
		while((tokens[numTokens] = strtok(NULL, " \n\t")) != NULL) numTokens++;

		commandHandler(tokens);

	}

	exit(0);
}
