/*
 * Suprisingly Not Born Again SH
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFSIZE 512

int main(void)
{
	char str[BUFSIZE];
	char *termPID;
	char *out;
	char *logto = NULL;
	int exitCode;
	int i;
	int readed;
	int cnt;
	int fd;
	int val;
	pid_t pid;
	pid_t child_pid;
	
	termPID = (char *)calloc(BUFSIZE, 1);
	out = "This is Surprisingly Not a \"Born Again Shell\", or snbash.\n";
	write(1, out, strlen(out));
	out = "Enter terminal PID to send output to it, or '0' to output to log file.\n";
	write(1, out, strlen(out));
	out = "(for first option open another terminal\n";
	write(1, out, strlen(out));
	out = "and execute 'ps' to find out its PID): ";
	write(1, out, strlen(out));
	read(0, termPID, 10);
	termPID[strlen(termPID)-1] = '\0';
	if(strcmp(termPID, "0") != 0) {
		logto = (char *)calloc(BUFSIZE, 1);
		sprintf(logto, "/proc/%s/fd/1", termPID);
		out = "Sending output to terminal with PID ";
		write(1, out, strlen(out));
		write(1, termPID, strlen(termPID));
		write(1, "\n\n", 2);
	} else {
		out = "Sending output to logfile 'outlog.txt'\n\n";
		write(1, out, strlen(out));
	}
	free(termPID);
	
	
	while (1) {
		//sleep(1);
		i = 0;
		getcwd(str, BUFSIZE);
		strcat(str, ": ");
		write(1, str, strlen(str));
		for (i = 0; i < BUFSIZE; i++)
			str[i] = '\0';
		i = 0;
		
		readed = read(0, str, BUFSIZE);
		if (readed == 0 || strcmp(str, "\n") == 0)
			continue;
		else if (strcmp(str, "exit\n") == 0)
			break;
	
		for (i = strlen(str)-1; i >= 0; i--) {
			if (isalnum(str[i]) || ispunct(str[i]))
				break;
			if (str[i] == ' ')
				str[i] = '\0';
			else if (str[i] == '\n')
				str[i] = '\0';
		}
	
		cnt = 0;
		i = 0;
		do {
			if (str[i] == ' ')
				cnt++;
			i++;
		} while (str[i-1] != '\0');
		char *args[cnt+2];
		i = 0;
		args[i] = strtok(str, " ");
		i++;
		while (args[i-1] != NULL) {
			args[i] = strtok(NULL, " ");
			i++;
		}
		
		if (strcmp(args[0], "cd") == 0) {
			chdir(args[1]);
			continue;
		}
	
		pid = fork();
		switch (pid) {
		case -1:
			printf("Forking failed.\n");
			perror(NULL);
			break;
		case 0:
			if(logto == NULL) {
				fd = open("outlog.txt", O_RDWR|O_APPEND|O_CREAT, 0600);
			} else {
				fd = open(logto, O_RDWR|O_CREAT);
			}
			dup2(fd, 1);
			close(fd);
			write(1, "\n", 1);
			for (i = 0; i < cnt+1; i++) {
				write(1, args[i], strlen(args[i]));
				write(1, " ", 1);
			}
			write(1, ":\n", 2);
			execvp(args[0], args);
			exitCode = 0;
			break;
		default:
			exitCode = 1;
			break;
		}
	
		if (pid != 0) {
			child_pid = wait(&val);
			if (WIFEXITED(val)) {
				printf("---Operation finished, ");
				printf("code %d---\n", WEXITSTATUS(val));
			} else {
				printf("Child terminated abnormally.\n");
				perror(NULL);
			}
		}
	}
	exit(exitCode);
}
