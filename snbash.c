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
	int cnt;
	int fd;
	pid_t pid;
	pid_t child_pid;
	
	// Print a greeting and receive PID to log into.
	termPID = (char *)calloc(BUFSIZE, 1);
	out = "This is Surprisingly Not a \"Born Again Shell\", or snbash.\n";
	write(1, out, strlen(out));
	out = "Enter terminal PID to send output to it, ";
	write(1, out, strlen(out));
	out = "or '0' to output to log file.\n";
	write(1, out, strlen(out));
	out = "(for first option open another terminal\n";
	write(1, out, strlen(out));
	out = "and execute 'ps' to find out its PID): ";
	write(1, out, strlen(out));
	// If input here is incorrect, it will be catched later.
	read(0, termPID, 10);
	termPID[strlen(termPID)-1] = '\0';
	if (strcmp(termPID, "0") != 0) {
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
	
	// Execute commands while not receiving 'exit'.
	while (1) {
		// Print current working directory.
		i = 0;
		out = "\033[38;5;166m\033[1m";
		getcwd(str, BUFSIZE);
		strcat(str, ":\033[0m ");
		write(1, out, strlen(out));
		write(1, str, strlen(str));
		for (i = 0; i < BUFSIZE; i++)
			str[i] = '\0';
		// Read a command and process 'exit' and empty commands.
		// If command is incorrect, it will be catched later.
		i = read(0, str, BUFSIZE);
		if (i == 0 || strcmp(str, "\n") == 0)
			continue;
		else if (strcmp(str, "exit\n") == 0)
			break;
		// Delete junk symbols from the end of string.
		for (i = strlen(str)-1; i >= 0; i--) {
			if (isalnum(str[i]) || ispunct(str[i]))
				break;
			if (str[i] == ' ')
				str[i] = '\0';
			else if (str[i] == '\n')
				str[i] = '\0';
		}
		// Count arguments and split string into args array.
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
		// Special processing for 'cd' command.
		if (strcmp(args[0], "cd") == 0) {
			if (chdir(args[1]) == -1)
				perror("Failed to perform 'cd'");
			continue;
		}
		// Fork and execute command in the child process.
		pid = fork();
		switch (pid) {
		case -1:
			printf("Forking failed.\n");
			perror(NULL);
			break;
		case 0:
			// Open log file and redirect output to it.
			if (logto == NULL) {
				fd = open("outlog.txt", O_RDWR|O_APPEND|O_CREAT, 0600);
			} else {
				fd = open(logto, O_RDWR|O_CREAT);
			}
			if (fd == -1) {
				out = "Failed to open log stream. Stopping.\n";
				write(1, out, strlen(out));
				perror("Open stream error");
				exit(3);
			}
			dup2(fd, 1);
			close(fd);
			write(1, "\n", 1);
			if (logto != NULL) {
				out = "\033[93m\033[1m";
				write(1, out, strlen(out));
			}
			for (i = 0; i < cnt+1; i++) {
				write(1, args[i], strlen(args[i]));
				write(1, " ", 1);
			}
			if (logto != NULL)
				write(1, ">>\033[0m\n", 7);
			else
				write(1, ">>\n", 3);
			if (execvp(args[0], args) == -1) {
				exitCode = 2;
				perror("Error executing command");
				exit(exitCode);
			}
			break;
		default:
			// Parent waits for child's command completion.
			child_pid = wait(&i);
			if (WIFEXITED(i)) {
				printf("\033[2m--- Operation finished, ");
				printf("code %d ---\033[0m\n", WEXITSTATUS(i));
			} else {
				printf("Child terminated abnormally.\n");
				perror("Error code");
				exitCode = 1;
			}
			break;
		}
	}
	exitCode = 0;
	exit(exitCode);
}
