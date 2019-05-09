#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_CHAR_LIMIT 1024
#define TRUE 1
#define FALSE 0

pid_t pid;
int command_pipe[2];

int lsh_loop(char* command);
void manage_command(char *args[]);
void execute_program(char *args[], int ampersand);
void manage_io(char *args[], char *in_file, char* out_file, int mode);
void manage_pipe(char *args[]);



void signalHandler_int()
{

}

int main()
{
  char line[MAX_CHAR_LIMIT];

  printf("lsh: Type 'exit' or send EOF to quit the shell\n");
  lsh_loop(line);
}

void manage_io(char *args[], char* in_file, char* out_file, int mode)
{
  int fd;

  pid = fork();

  if (pid == 0)
  {
    switch(mode)
    {
      case 0:
        fd = open(out_file, O_CREAT | O_TRUNC | O_WRONLY, 0755);
        dup2(fd, STDOUT_FILENO);
        close(fd);
      break;
      case 1:
        fd = open(in_file, O_RDONLY, 0600);
        dup2(fd, STDIN_FILENO);
        close(fd);
      break;
      case 2:
        fd = open(out_file, O_CREAT | O_TRUNC | O_WRONLY, 0755);
        dup2(fd, STDERR_FILENO);
        close(fd);
      break;
    }

    if (execvp(args[0], args) == -1)
    {
      kill(getpid(), SIGTERM);
    }
  }

  waitpid(pid, NULL, 0);

}

void manage_pipe(char *args[])
{
  int pipefd[2];
  int pipefd_2[2];

  int shouldEnd = 0;

  int commands = 0;
  char* command[512];

  int i = 0;
  int j = 0;
  int k = 0;
  while(args[i] != NULL)
  {
    if (strcmp(args[i++], "|") == 0)
    {
        commands++;
    }
  }
  commands++;

  i = 0;

  while (args[i] != NULL && shouldEnd != 1)
  {
      k = 0;
      while (strcmp(args[i], "|") != 0)
      {
        command[k] = args[i];
        i++;
        if (args[i] == NULL) {
          shouldEnd = 1;
          k++;
          break;
        }
        k++;
      }

      command[k] = NULL;
      i++;

      if (j % 2 == 1)
      {
        pipe(pipefd);
      } else
      {
        pipe(pipefd_2);
      }

      pid = fork();

      if (pid == -1)
      {
        printf("Cant create a child");
      }
      else if (pid == 0)
      {
        if (j == 0)
        {
          dup2(pipefd_2[1], STDOUT_FILENO);
        }
        else if (j == commands - 1)
        {
          if (commands % 2 == 1)
          {
            dup2(pipefd[0], STDIN_FILENO);
          }
          else
          {
            dup2(pipefd_2[0], STDIN_FILENO);
          }
        }
        else
        {
          if (j % 2 != 0)
          {
            dup2(pipefd_2[0], STDIN_FILENO);
            dup2(pipefd[1], STDOUT_FILENO);
          }
          else
          {
            dup2(pipefd[0], STDIN_FILENO);
            dup2(pipefd_2[1], STDOUT_FILENO);
          }
        }

        if (execvp(command[0], command) == -1)
        {
          kill(getpid(), SIGTERM);
        }
      }

      if (j == 0)
      {
        close(pipefd_2[1]);
      }
      else if (j == commands - 1)
      {
        if (commands % 2 == 1)
        {
          close(pipefd[0]);
        }
        else
        {
          close(pipefd_2[0]);
        }
      }
      else
      {
        if (j % 2 != 0)
        {
          close(pipefd_2[0]);
          close(pipefd[1]);
        }
        else
        {
          close(pipefd[0]);
          close(pipefd_2[1]);
        }

      }

      waitpid(pid, NULL, 0);
      j++;

    }


}

void execute_program(char *args[], int ampersand)
{
  if ((pid = fork()) == -1)
  {
    printf("Cant create a child");
    return;
  }

  if (pid == 0)
  {
    signal(SIGINT, signalHandler_int);
    if (execvp(args[0], args) == -1)
    {
      kill(getpid(), SIGTERM);
    }
  }

  if (ampersand == 0)
    waitpid(pid, NULL, 0);
  else
    printf("Process created!\n");
}

void manage_command(char *args[])
{
  int i = 0;
  int j = 0;
  char *args_stripped[512];
  int ampersand = 0;

  while (args[j] != NULL)
  {
    if ( (strcmp(args[j], ">") == 0) || (strcmp(args[j], "2>") == 0) || (strcmp(args[j], "|") == 0) || (strcmp(args[j], "<") == 0) || (strcmp(args[j], "&") == 0))
    {
      //printf("BREAKING OUT\n");
      break;
    }
    args_stripped[j] = args[j];
    j++;
  }

  //printf("%s\n", args[0]);
  if (strcmp(args[0], "exit") == 0)
    exit(0);
  else if (strcmp(args[0], "clear") == 0)
    system("clear");
  else if (strcmp(args[0], "cd") == 0)
  {
    if (args[1] == NULL)
    {
      chdir(getenv("HOME"));
    }
    else
    {
      if (chdir(args[1]) == -1)
      {
        printf("Sorry, wrong directory\n");
      }
    }
  }
  else
  {
      while (args[i] != NULL && ampersand == 0)
      {
        if(strcmp(args[i], "&") == 0)
        {
          ampersand = 1;
          args[i] = " ";
        }
        else if (strcmp(args[i], "|") == 0)
        {
          manage_pipe(args);
          return;
        }
        else if (strcmp(args[i], "2>") == 0)
        {
          if (args[i+1] == NULL)
          {
            printf("Need input file");
            return;
          }
          manage_io(args_stripped, NULL, args[i+1], 2);
          return;
        }
        else if (strcmp(args[i], ">") == 0)
        {
          if (args[i+1] == NULL)
          {
            printf("Need input file");
            return;
          }
          manage_io(args_stripped, NULL, args[i+1], 0);
          return;
        }
        else if (strcmp(args[i], "<") == 0)
        {
          if (args[i+1] == NULL)
          {
            printf("Need output file");
            return;
          }
          manage_io(args_stripped, args[i+1], NULL, 1);
          return;
        }
        i++;
      }
      args_stripped[i] = NULL;
      execute_program(args_stripped, ampersand);
  }

}

int lsh_loop(char* command)
{
  signal(SIGINT, SIG_IGN);

    char* args[512];

    while(1) {
      char cwd[512];
      getcwd(cwd, sizeof(cwd));
      printf("%s $>", cwd );

      fflush(NULL);

      if(!fgets(command, MAX_CHAR_LIMIT, stdin))
        return 0;

      if((args[0] = strtok(command, " \n\t")) == NULL) continue;

      int i = 0;

      do {
        i++;
      } while ((args[i] = strtok(NULL, " \n\t")) != NULL);

      manage_command(args);
    }
}
