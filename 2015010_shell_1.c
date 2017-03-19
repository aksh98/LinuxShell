#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
//#include <unipipc.h>
#define BUFF_SIZE 1024
#define BUFF_TOK 64
#define DELIMITERS " \n\r\t\a"
char *hist[10];
int current =0;
  
char *builtin_str[] = { "cd","help","exit","history"};//,"kill"
//==============================================================
void sighandler(int signum){ printf("\nInvalid! Press enter to continue\n");return;}
//=======================================================
int Exitt(char **args){ return 0;}
//==========================================
int chngDir(char **args)
{
    if (args[1] == NULL) {
       fprintf(stderr, "Expected argument to \"cd\"\n");}
    else if (chdir(args[1]) != 0)
            perror("Error");
    return 1;
}
/*int killl(char **args){
    if(args[1]==NULL){
        fprintf(stderr, "Enter the pid of the process you wish to kill.\n");
        return 1;
    }
    int pid = atoi(args[1]);
    kill(pid,SIGKILL);
    return 1;
}*/
//===========================================================
int Help(char **args)
{
  int i;
  printf("-------------------------------------\n");
  printf("My Linux Shell\n");
  printf("The following commands are built in:\n");
  printf("-------------------------------------\n");
  
  for (i = 0; i < 4; i++) {
    printf("%4d %s\n",i+1,builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}
//=======================================================
int history(char **args)
{
    int i = current;
    int n = 1;
    do {
        if (hist[i]){
            printf("%4d  %s\n", n, hist[i]);
            n++;
        }
    i = (i + 1) % 10;

    } while (i != current);
    return 1;
}

int (*builtin_func[]) (char **) = { &chngDir,&Help,&Exitt,&history};//,&killl
//========================================================
int clear_history(char *hist[])
{
    int i;
    for (i = 0; i < 10; i++) {
        free(hist[i]);
        hist[i] = NULL;
    }
    return 1;
}
//==============================================================
int launch(char **args)
{
    pid_t pid, wpid;
    int status;
    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1)
            perror("ERROR");
        exit(EXIT_FAILURE);
    }
        else if (pid < 0)
            perror("fork_error "); 
        else{
            do{
                wpid = waitpid(pid, &status, WUNTRACED);
            }while (!WIFSIGNALED(status) && !WIFEXITED(status));
    }
    //execvp(args[0],args);
    return 1;
}

//===================================================
int execute(char **args)
{
    int i;
    if (!args[0])
       return 1;
    for (i = 0; i < 5; i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
      }
    }

    return launch(args);
}
//============================================================
char *read_line(void)
{
    int bufsize = BUFF_SIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "Error while allocating space\n");
        exit(EXIT_FAILURE);
    }
    while (1) {
        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else 
            buffer[position] = c;
        position++;
        if (position >= bufsize) {
            bufsize += BUFF_SIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "Error while allocating space\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

//========================================================
char **split_line(char *line)
{
    hist[current]= strdup(line);
    current = (current+1)%10;
    int bufsize = BUFF_TOK, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;
    if (!tokens){
      fprintf(stderr, "Error while allocating space\n");
      exit(EXIT_FAILURE);
    }

    token = strtok(line, DELIMITERS);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += BUFF_TOK;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
              fprintf(stderr, "Error while allocating space\n");
              exit(EXIT_FAILURE);
            }
        }

      token = strtok(NULL, DELIMITERS);
    }
    tokens[position] = NULL;
    return tokens;
}

int main(int argc, char **argv)
{
    int i;
    for (i = 0; i < 10; i++)
        hist[i] = NULL;
    signal(SIGINT, sighandler);
    char *line,**args;
    int status;
    do {
        printf("$MyShell-> ");
        line = read_line();
        args = split_line(line);
        status = execute(args);

        free(line);
        free(args);
    } while (status);
    execvp(args[0],args);
    printf("GoodBye ! :) \n");
    return EXIT_SUCCESS;
}

//===============================================================
/*int spawn_proc (int in, int out, struct command *cmd)
{
  pid_t pid;
  if ((pid = fork ()) == 0)
    {
      if (in != 0)
        {
          dup2 (in, 0);
          close (in);
        }

      if (out != 1)
        {
          dup2 (out, 1);
          close (out);
        }
      return execvp (cmd->argv [0], (char * const *)cmd->argv);
    }
  return pid;
}
//=========================================================

int fork_pipes (int n, struct command *cmd)
{
  int i;
  pid_t pid;
  int in, fd [2];
  in = 0;
  for (i = 0; i < n - 1; ++i)
    {
      pipe (fd);
      spawn_proc (in, fd [1], cmd + i);
      close (fd [1]);
      in = fd [0];
    }
  if (in != 0)
    dup2 (in, 0);
  return execvp (cmd [i].argv [0], (char * const *)cmd [i].argv);
}
*/
