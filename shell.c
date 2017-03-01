#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#define BUFF_SIZE 1024
#define BUFF_TOK 64
#define DELIMITERS " \n\r\t\a"

char *hist[50];
int current =0;
//==============================================================

void sighandler(int signum){ printf("\nInvalid input! Press enter to continue\n");return;}

//=======================================================

char *builtin_str[] = { "cd","help","exit","history","kill"};

int Exitt(char **args){ return 0;}

//==============================================================

int chngDir(char **args)
{
    if (args[1] == NULL) {
       fprintf(stderr, "Expected argument to \"cd\"\n");}
    else if (chdir(args[1]) != 0)
            perror("Error");
    return 1;
}
//=================================================================
int killl(char **args){
    if(args[1]==NULL){
        fprintf(stderr, "Enter the pid of the process you wish to kill.\n");
        return 1;
    }
    int pid = atoi(args[1]);
    kill(pid,SIGKILL);
    return 1;
}

//====================================================================

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
    i = (i + 1) % 50;

    } while (i != current);
    return 1;
}

//========================================================

int clear_history(char *hist[])
{
    int i;
    for (i = 0; i < 50; i++) {
        free(hist[i]);
        hist[i] = NULL;
    }
    return 1;
}
//===================================================================

int (*builtin_func[]) (char **) = { &chngDir,&Help,&Exitt,&history,&killl};

//==============================================================

int lsh_launch(char **args)
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
    return 1;
}

//===================================================

int lsh_execute(char **args)
{
    int i;
    if (!args[0])
       return 1;
    for (i = 0; i < 5; i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
      }
    }

    return lsh_launch(args);
}

//============================================================

char *lsh_read_line(void)
{
    int position = 0,c;
    int bufsize = BUFF_SIZE;
    char *buffer = malloc(sizeof(char) * bufsize);

    if (!buffer) 
    {
        fprintf(stderr, "Error while allocating space\n");
        exit(EXIT_FAILURE);
    }
    while (1) 
    {
        c = getchar();
        if (c == EOF || c == '\n') 
        {
            buffer[position] = '\0';
            return buffer;
        } 
        else 
            buffer[position] = c;
        position++;
        if (position >= bufsize) 
        {
            bufsize += BUFF_SIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) 
            {
                fprintf(stderr, "Error while allocating space\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

//===============================================================
char **lsh_split_line(char *line)
{
    hist[current]= strdup(line);
    current = (current+1)%50;
    int bufsize = BUFF_TOK, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;
    if (!tokens)
    {
        fprintf(stderr, "Error while allocating space\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, DELIMITERS);
    while (token != NULL) 
    {
        tokens[position] = token;
        position++;

        if (position >= bufsize) 
        {
            bufsize += BUFF_TOK;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) 
            {
                fprintf(stderr, "Error while allocating space\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, DELIMITERS);
    }
    tokens[position] = NULL;
    return tokens;
}
//========================================================

int main(int argc, char **argv)
{
    int i;
    for (i = 0; i < 50; i++)
        hist[i] = NULL;
    signal(SIGINT, sighandler);
    char *line,**args;
    int status;
    do {
        printf("$MyShell-> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);

    } while (status);
    
    printf("GoodBye ! :) \n");
    
    return EXIT_SUCCESS;
}
