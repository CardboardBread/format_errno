#include <stdio.h>
#include <stdlib.h>

#include "error_errno.h"

#define BUFSIZE 256

char execname[] = "errno";
char *execargs[] = {"errno", "-l", NULL};

int main(int argc, char **argv) {

  int stdfd[2];
  pipe(stdfd);

  switch(fork()) {
    case -1:
      return 1;
    case 0:
      close(stdfd[0]);
      dup2(stdfd[1], STDOUT_FILENO);

      close(stdfd[1]);
      dup2(stdfd[1], STDERR_FILENO);

      execvp(execname, execargs);
      exit(1);
    default:
      close(stdfd[1]);
      wait(NULL);
      break;
  }

  int maxname = 0;
  int maxnum = 0;
  int maxdesc = 0;

  struct error_t headnode;
  headnode->name;
  headnode->num;
  headnode->desc;
  headnode->next = NULL;

  struct buf_t buffer;
  buffer->buf = malloc(sizeof(char) * BUFSIZE);

  int nbytes = 0;
  while (read_to_buf(&buffer, stdfd[0], &nbytes) < 1) {

    // find all newlines in the buffer
    while (find_newline(&buffer, &(buffer->consumed)) == 0) {

      // allocate a new error
      // parse the consumed section of the buffer for the errno string
      // assemble the relevant pieces into the error struct
      // append error struct to the list (tracking the tail might be useful to save time)
      // clear the consumed memory and wait for the next newline
    }

    // allocate new error
    struct error_t *err;
    void *newerr = malloc(sizeof(struct error_t));
    if (newerr != NULL) {
      err = (struct error_t *) newerr;
    } else {
      break;
    }

    // construct buffer into error
    assemble_error(&buffer, err);
  }

  exit(0);
}
