#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linux/limits.h>
#include <sys/wait.h>
#include <unistd.h>

#define assert(_e, _c) if(!(_e)) { \
  fprintf(stderr, "Exit code: %d - %s:%d", _c, __FILE__, __LINE__); \
  exit(_c);\
}

#define BUFSIZE 8192

enum {
  EXIT_MALLOC,
  EXIT_REALLOC,
  EXIT_FORK,
  EXIT_WAITPID,
};

static char *readInput(void);
static void execCommand(char **args);
static int otherCommands(char **args);
static void generateStatus(void);
static void shellLoop(void);

static char *
readInput(void) {
  int bufsize = BUFSIZE;
  int pos = 0;
  int c;

  char *mes = malloc(sizeof(char) * BUFSIZE);
  assert(mes, EXIT_MALLOC);
  while((c = getchar()) != '\n') {
    if(pos >= bufsize) {
      bufsize += BUFSIZE;

      mes = realloc(mes, sizeof(char) * bufsize);
      assert(mes, EXIT_REALLOC);
    }

    mes[pos++] = c;
  }

  mes[pos] = '\0';
  return mes;
}

static char **
readArgs(char *mes) {
  const char *delim = " \n\t";
  int bufsize = BUFSIZE;
  int pos = 0;

  char **args = malloc(sizeof(char *) * BUFSIZE);
  assert(args, EXIT_MALLOC);

  char *arg = strtok(mes, delim);
  while(arg) {
    if(pos >= bufsize) {
      bufsize += BUFSIZE;

      args = realloc(args, sizeof(char *) * bufsize);
      assert(args, EXIT_REALLOC);
    }

    args[pos++] = arg;
    arg = strtok(NULL, delim);
  }

  args[pos] = NULL;
  return args;
}

static int
otherCommands(char **args) {
  if(!strcmp(args[0], "/usr/bin/cd")) {
    chdir(args[1]);
    return 0;
  } else if(!strcmp(args[0], "/usr/bin/echo")) {
    /* check if any of the args[i] starts with $
       if yes getenv(args[i]) without the $      */
  }

  return -1;
}

static void
execCommand(char **args) {
  if(!args[0]) return;

  char *buf = malloc(sizeof(char) * (strlen(args[0]) + strlen("/usr/bin/")));
  char *command = args[0];

  int pid, wpid;
  int status;

  /* needs upgrade - reading from env */
  sprintf(buf, "/usr/bin/%s", args[0]);
  args[0] = buf;

  if((pid = fork()) == 0) {
    if(otherCommands(args) == -1 && execv(args[0], args) == -1)
      fprintf(stderr, "shell: unkown command: %s\n", command);

    exit(0);
  } else if(pid == -1) {
    assert(NULL, EXIT_FORK);
  } else {
    otherCommands(args);
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
      assert(wpid != -1, EXIT_WAITPID);
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  free(buf);
}

static void
generateStatus(void) {
  const char *delim = "/";
  char *cwd = malloc(sizeof(char) * PATH_MAX);
  char *lastDir = NULL;

  assert(cwd, EXIT_MALLOC);
  getcwd(cwd, PATH_MAX);

  char *p = strtok(cwd, delim);
  while(p) {
    lastDir = p;

    p = strtok(NULL, delim);
  }

  printf("%s:%s $ ", getlogin(), lastDir ? lastDir : "/");
  free(cwd);
}

static void
shellLoop(void) {
  while(1) {
    generateStatus();

    char *mes = readInput();
    char **args = readArgs(mes);

    execCommand(args);
    free(mes);
    free(args);
  }
}

int
main() {
  printf("\033[2J\033[H");
  shellLoop();
  return 0;
}
