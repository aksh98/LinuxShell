#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main ()
{
		
	system ("xterm -e \"/*complete_path*/shell.c\"");

	return 0;

}
