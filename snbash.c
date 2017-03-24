/*
* Suprisingly Not Born Again SH
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFSIZE 512

int main(void)
{
	pid_t pid;
	//pid_t terminalPid;
	int exitCode;
	int i = 0;
	
	/*terminalPid = fork();
	if(terminalPid == -1)
	{
		printf("Opening log terminal failed. Stopping.\n");
		perror(NULL);
		exit(2);
	}
	else if(terminalPid == 0)
	{
		execlp("gnome-terminal", "gnome-terminal", NULL);
	}*/
	
	while(1)
	{
		sleep(1);
		char str[BUFSIZE];
		getcwd(str, BUFSIZE);
		strcat(str, ": ");
		write(1, str, strlen(str));
		char argstr[BUFSIZE];
		for(i = 0; i < BUFSIZE; i++)
			argstr[i] = '\0';
		i = 0;
		
		int readed = read(0, argstr, BUFSIZE);
		if(readed == 0 || strcmp(argstr, "\n") == 0)
			continue;
		else if(strcmp(argstr, "exit\n") == 0)
			break;
	
		for(i = strlen(argstr)-1; i >= 0; i--)
		{
			if(isalnum(argstr[i]) || ispunct(argstr[i]))
				break;
			if(argstr[i] == ' ')
			{
				argstr[i] = '\0';
			}
			else if(argstr[i] == '\n')
			{
				argstr[i] = '\0';
			}
		}
	
		int cnt = 0;
		i = 0;
		do
		{
			if(argstr[i] == ' ')
				cnt++;
			i++;
		} while(argstr[i-1] != '\0');
	
		char *args[cnt+3];
		i = 0;
		args[i] = strtok(argstr, " ");
		i++;
		while(args[i-1] != NULL)
		{
			args[i] = strtok(NULL, " ");
			i++;
		}
		
		if(strcmp(args[0], "cd") == 0)
		{
			chdir(args[1]);
			continue;
		}
	
		// That is how I tried to do... Nothing works.
		//char *logto = (char *)calloc(BUFSIZE, 1);
		//sprintf(logto, ">/proc/%d/fd/1", terminalPid);
		//sprintf(logto, ">/dev/pts/6");
		//sprintf(logto, ">>outlog.txt");
		//args[cnt+1] = logto;
		//args[cnt+2] = NULL;
		
		for(i = 0; i < cnt+3; i++)
			printf("arg %d: %s\n", i, args[i]);
		i = 0;

		pid = fork();
		switch(pid)
		{
			case -1:
				printf("Forking failed.\n");
				perror(NULL);
				break;
			case 0:
				execvp(args[0], args);
				exitCode = 0;
				break;
			default:
				exitCode = 1;
				break;
		}
	
		if(pid != 0)
		{
			int stat_val;
			pid_t child_pid;
			child_pid = wait(&stat_val);
			if(WIFEXITED(stat_val))
			{
				printf("Operation finished with code %d\n", WEXITSTATUS(stat_val));
			}
			else
			{
				printf("Child terminated abnormally.\n");
				perror(NULL);
			}
		}
	}
	exit(exitCode);
}
