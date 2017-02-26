#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define MAX_LEN 1000
#define DELIM " \t\r\n\a"
//-------------------------------------
char** split_line(char* line){
	int buff_size = 64,position = 0;
	char** tokens = malloc(buff_size * sizeof(char*));
	char* token;
	if(!tokens){fprintf(stderr, "Malloc is not able to allocate space\n");
	exit(EXIT_FAILURE);}
	
	token = strtok(line,DELIM);
	
	while(token!= NULL){
		tokens[position] = token;
		token = strtok(NULL,DELIM);
		position++;
	}	
	tokens[position]= NULL;
	return tokens;
}
//--------------------------------------
char *lsh_read_line()
{
  char *line = NULL;
  ssize_t buffer = 0; // have getline allocate a buffer for us
  getline(&line, &buffer, stdin);
  return line;
}
int fork_launch(char** args){
	pid_t pid,wpid;
	pid = fork();
	int status;
	if(pid==0){
		if (execvp(args[0], args) == -1) {
			perror("ERROR");}
  		exit(EXIT_FAILURE);
  	}
  	else if (pid < 0) {
   		perror("ERROR WHILE FORKING");
  	}
  	else {
    // Parent process
    	while (!WIFEXITED(status) && !WIFSIGNALED(status)){
      		wpid = waitpid(pid, &status, WUNTRACED);
    } 
  }
  return 1;
}
int commd_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("errrr");
    }
  }
  return 1;
}
int execute(char **args){
	int i;
	if(args[0]==NULL){return 1;}
	if(strcmp(args[0],"exit")==0)
		return 0;
	if(strcmp(args[0],"cd")==0)
		return (commd_cd)(args);
	return fork_launch(args);
}


