/*
* Suprisingly Not Born Again SH
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	pid_t terminalPid;
	int exitCode;
	int i = 0;
	
	terminalPid = fork();
	if(terminalPid == -1)
	{
		printf("Opening log terminal failed. Stopping...\n");
		perror(NULL);
		exit(2);
	}
	else if(terminalPid == 0)
	{
		execlp("gnome-terminal", "gnome-terminal", NULL);
	}
	
	char *exec_args[argc];
	for(; i < argc-1; i++)
	{
		exec_args[i] = argv[i+1];
		
	}
	exec_args[i] = NULL;
	pid = fork();
	switch(pid)
	{
		case -1:
			printf("Forking failed.\n");
			perror(NULL);
			break;
		case 0:
			execvp(exec_args[0], exec_args);
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
		printf("\nChild has finished\n");
		if(WIFEXITED(stat_val))
		{
			printf("Child exited with code %d\n", WEXITSTATUS(stat_val));
		}
		else
		{
			printf("Child terminated abnormally. o.O\n");
			perror(NULL);
		}
	}
	
	exit(exitCode);
}
