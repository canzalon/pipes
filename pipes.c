// ////////////////////////////////////////////////////////////////
//
// Project: Pipes in Unix 
// Author: Christopher Anzalone
// File: pipes.c
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
// Description: 
//
//	The pipes program replicates the behavior of the pipes 
//	command (|) in a unix shell. This command takes the output 
//	of one process, and makes it the input for another. To 
//	achieve this, this C program creates a pipe using the 
//	pipe() function. It then creates two child processes using 
//	fork(). The fork operation is one that creates a copy of 
//	the parent process, creating a child process. It is a 
//	system call in the kernel that is used for process creation 
//	in Unix systems. Child 1 redirects its output from stdout 
//	to the write end of the pipe and executes the command 
//	"ls -al" using the execlp() function. Child 2 redirects its 
//	input from stdin to the read end of the pipe and executes 
//	the "sort -n -k 5" command. The parent process waits for 
//	its children processes to finish executing before it 
//	terminates.
//
// ////////////////////////////////////////////////////////////////

#include <sys/types.h>  
#include <stdio.h>  
#include <unistd.h>  
#include <string.h> 
#include <stdlib.h> 
#include <sys/wait.h>  


#define READ_END 0
#define WRITE_END 1

int main()
{					
	pid_t pid;           
	int fd[2];
	int status;

	/* create pipe */
	if (pipe(fd) == -1)
	{
		fprintf(stderr, "Pipe failed.");
		return 1;
	}

	/* fork first child */
	pid = fork();

	if (pid < 0) 
	{ 
		fprintf(stderr, "Fork call 1 has failed"); 		
		return 1;
	}
	else if (pid == 0) 
	{
		/* close unused side of pipe */
		if ( close(fd[READ_END]) == -1 )
		{
			fprintf(stderr, "close call failed");
			return 1;			
		}

		/* redirect standard output to write-end of pipe */
		if ( dup2(fd[WRITE_END], STDOUT_FILENO) == -1 )
		{
			fprintf(stderr, "dup2 call failed");
			return 1;
		}
		
		/* execute process "ls" with "ls -al" */
		if ( execlp("ls", "ls", "-al", NULL) == -1 )
		{
			fprintf(stderr, "execlp call failed");
			return 1;
		}
	
		/* close write-end of pipe (to avoid hanging) */
	 	if ( close(fd[WRITE_END]) == -1)
		{
			fprintf(stderr, "close call failed");
			return 1;
		}
	}
	else 
	{
		/* fork second child */
		pid = fork();
	
		if (pid < 0) 
		{ 
			fprintf(stderr, "Fork call 2 has failed"); 
			return 1;
		}
		else if (pid == 0) 
		{
			/* close unused side of pipe */
			if ( close(fd[WRITE_END]) == -1 )
			{
				fprintf(stderr, "close call failed");
				return 1;
			}

			/* redirect standard input to read-end of pipe */
			if ( dup2(fd[READ_END], STDIN_FILENO) == -1)
			{
				fprintf(stderr, "dup2 call failed");
				return 1;
			}

			/* execute process "sort" with "sort -n -k 5" */
			if ( execlp("sort", "sort", "-n", "-k", "5", NULL) == -1)
			{
				fprintf(stderr, "execlp call failed");
				return 1;
			}

			/* close read-end of pipe (to avoid hanging) */
			if ( close(fd[READ_END]) == -1 )
			{
				fprintf(stderr, "close call failed");
				return 1;
			}
		}
		else 
		{
			wait(&status);
		}	

	}

	return 0;
}
