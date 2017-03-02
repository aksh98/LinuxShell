This is a basic linux shell in which I implemented some Inbuilt functions such as:
  cd
  help
  kill
  history
  exit
It's an simple shell built using fork/exec/wait system calls. The input commands use stdin and output uses stdout, in an infinite while loop.
The file run.c is for running the shell in a new window.
It exits on 'exit' command.
