#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>

#define assert(_e, _c, ...) if(!(_e)) { fprintf(stderr, __VA_ARGS__, __FILE__, __LINE__, _c); exit(_c);}

#define BUFSIZE 1024

enum {
  EXIT_MALLOC,
  EXIT_REALLOC,
  EXIT_FORK,
};


static char *readInput(void);
static void execCommand(char **args);
static void shellLoop(void);

char *readInput(void) {
  int bufsize = BUFSIZE;
  int pos = 0;
  int c;

  char *mes = malloc(sizeof(char) * BUFSIZE);
  assert(mes, EXIT_MALLOC, "%s:%d: malloc, %d\n");
  while((c = getchar()) != '\n') {
    if(pos >= bufsize) {
      bufsize += BUFSIZE;

      mes = realloc(mes, sizeof(char) * bufsize);
      assert(mes, EXIT_REALLOC, "%s:%d: realloc, %d\n");
    }

    mes[pos++] = c;
  }

  mes[pos] = '\0';
  return mes;
}

char **readArgs(char *mes) {
  const char *delim = " \n\t";
  int bufsize = BUFSIZE;
  int pos = 0;

  char **args = malloc(sizeof(char *) * BUFSIZE);
  assert(args, EXIT_MALLOC, "%s:%d: malloc, %d\n");

  char *arg = strtok(mes, delim);
  while(arg) {
    if(pos >= bufsize) {
      bufsize += BUFSIZE;

      args = realloc(args, sizeof(char *) * bufsize);
      assert(args, EXIT_REALLOC, "%s:%d: realloc, %d\n");
    }

    args[pos++] = arg;
    arg = strtok(NULL, delim);
  }

  args[pos] = NULL;
  return args;
}

void execCommand(char **args) {
  if(!args[0]) return;

  char *buf = malloc(sizeof(char) * (strlen(args[0]) + 10));
  char *command = args[0];

  sprintf(buf, "/usr/bin/%s", args[0]);
  args[0] = buf;

  int pid, wpid;
  pid = fork();

  if(pid == 0) {
    if(!strcmp(args[0], "/usr/bin/cd"));
    else if(execv(args[0], args)) {
      fprintf(stderr, "shell: unknown command: %s\n", command);
    }
    exit(0);
  } else if(pid == -1) {
    exit(1);
  } else {
    if(!strcmp(args[0], "/usr/bin/cd")) {
      chdir("~/.");
    }
    int status;
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }
}

void shellLoop(void) {
  while(1) {
    printf("frank $ ");

    char *mes = readInput();
    char **args = readArgs(mes);

    execCommand(args);
    free(mes);
    free(args);
  }
}

int main() {
  printf("\033[2J\033[H");
  shellLoop();
  return 0;
}
