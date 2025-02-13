#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>


void process_line_input(char* line);


char PATHS[1024] = "/bin";


void error() {
	char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}


int main(int argc, char* argv[]) {

	if(argc == 1) {
		// interactive mode
		printf("wish> ");

		while(1) {
			sleep(0.1);
			
			char *buffer = NULL;
	    	size_t bufsize = 32;

			if(getline(&buffer,&bufsize,stdin) > 0) {
				process_line_input(buffer);
				printf("wish> ");
			}
		}

	} else if(argc == 2) {
		// batch mode

		FILE* fp = fopen(argv[1], "r");

		if(fp == NULL) {
			error();
			exit(1);
		}

		char *buffer = NULL;
    	size_t bufsize = 32;

		while(getline(&buffer,&bufsize,fp) > 0) {
			process_line_input(buffer);
		}

	} else {
		error();
  		exit(1);
	}
	
	return 0;
}


char* remove_trailing_space(char* s) {
	while(isspace(s[0]))
		s++;

	int l = strlen(s);
	while(isspace(s[l-1]))
		s[--l] = '\0';

	return s;
}


int populate_args(char* command, char* args[]) {
	// populate args array and return number of arguments populated in array

	int i = 0;
	char* arg;
	while( (arg = strsep(&command, " \t")) != NULL) {
		arg = remove_trailing_space(arg);
		if(arg[0] != '\0') {
			// printf("populating args %d=%s; command=%s\n", i, arg, command);
			args[i++] = arg;
		}
	}
	args[i] = NULL;

	return i;
}


void get_command_path(char* path, char* command_path, char* args[]) {
	strcpy(command_path, path);
	strcat(command_path, "/");
	strcat(command_path, args[0]);
}


pid_t execute_command(char* command) {
	// command: command with arguments

	// printf("Executing Command = %s\n", command);

	pid_t pid;

	char *cmd, *redir;
	cmd = strsep(&command, ">");
	redir = command;

	if(redir != NULL) {
		// '>' is present

		redir = remove_trailing_space(redir);
		// printf("redir=%s\n", redir);
		
		if(redir[0] == '\0') {
			// no argument
			error();
			return 0;
		}

		if(strpbrk(redir, "> \t") != NULL) {
			// more than one argument or redirection operator
			error();
			return 0;
		}
	}

	char* args[16];
	int nargs = populate_args(cmd, args);
	if(nargs == 0) {
		// no command
		error();
		return 0;
	}

	char *path;
	char *paths = (char*) malloc((1+strlen(PATHS)) * sizeof(char));
	strcpy(paths, PATHS);
	// printf("paths=%s;\n", paths);

	while( (path = strsep(&paths, ";")) != NULL) {
		
		path = remove_trailing_space(path);
		if(path[0] == '\0')
			continue;

		// printf("path=%s;\n", path);
		
		char command_path[1024];
		get_command_path(path, command_path, args);

		if(access(command_path, X_OK) == 0) {
			// printf("Command found on path: %s with args[0]=%s\n", command_path, args[0]);

			pid = fork();
			// printf("fork pid: %d\n", pid);

			if(pid < 0) {
				// fork failed
				error();
				exit(1);
			
			} else if(pid == 0) {
				// execute child process

				if(redir) {
					// printf("redirecting in child process; redir=%s;\n", redir);
					freopen(redir, "w", stdout);
					freopen(redir, "w", stderr);
				}

				execv(command_path, args);
				// printf("execv returned value: %d\n", execv_ret);
				exit(EXIT_FAILURE);	
			
			} else {
				// parent process - return pid
				return pid;
			}
		}

		else {
			// printf("Command not found on path: %s\n", command_path);
		}
	}

	// command not found at any path
	error();
	return 0;
}


void set_path(int nargs, char* args[]) {
	PATHS[0] = '\0';

	for( int i=1; i<nargs; i++) {
		strcat(PATHS, args[i]);
		strcat(PATHS, ";");
	}
}


int handle_builtin_command(char* command_with_args) {
	// command: command with arguments

	char* command = (char*) malloc( (1+strlen(command_with_args)) * sizeof(char));
	char* tofree = command;
	strcpy(command, command_with_args); //copying string for manipulation

	char* args[16];
	int nargs = populate_args(command, args);
	
	char* cmd = args[0];
	// printf("handle_builtin_command command_with_args=%s;cmd=%s;\n", command_with_args, cmd);

	if(strncmp(cmd, "exit", strlen("exit")) == 0) {
		// printf("should exit\n");
		if(nargs > 1) {
			error();
			free(tofree);
			return 1;
		}

		exit(0);
	}

	if(strncmp(cmd, "cd", strlen("cd")) == 0) {
		
		if(nargs != 2) {
			error();
		} else {
			chdir(args[1]);
		}
		
		free(tofree);
		return 1;
	
	} else if(strncmp(cmd, "path", strlen("path")) == 0) {
		set_path(nargs, args);
		free(tofree);
		return 1;

	}

	free(tofree);
	return 0;
}


void process_line_input(char* line) {
	// remove next line character
	line[strcspn(line, "\n\r")] = '\0';
	// printf("line = %s\n", line);

	char* command;
	pid_t pid;

	while( (command = strsep(&line, "&")) != NULL) {
		// printf("before remove_trailing_space command=%s\n", command);
		command = remove_trailing_space(command);

		if(strlen(command) == 0)
			continue;

		// printf("before handle_builtin_command command=%s\n", command);
		if(handle_builtin_command(command)) {
			continue;
		}
		// printf("after handle_builtin_command command=%s\n", command);

		pid = execute_command(command);
	}

	// parent process - wait for all child processes to have completed
	if(pid > 0) {
		int status;
		while (waitpid(-1, &status, 0) != -1) {
			;
		}
	}

}


