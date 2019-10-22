#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "data_errno.h"
#include "error_errno.h"

#define BUFSIZE 256

const char *nameheader = "NAME";
const char *numheader = "NUM";
const char *descheader = "DESCRIPTION";

char execname[] = "errno";
char *execargs[] = {"errno", "-l", NULL};

int main(int argc, char **argv) {

  // create pipe for exec output
  int stdfd[2];
  pipe(stdfd);

  // fork process to exec errno
  switch(fork()) {
    case -1: // error, failed to fork
      return 1;
    case 0: // child

      // replace stdout with pipe
      close(stdfd[0]);
      dup2(stdfd[1], STDOUT_FILENO);

      // replace stderr with pipe
      close(stdfd[1]); // TODO: verify this is needed
      dup2(stdfd[1], STDERR_FILENO);

      // execute `errno -l`
      execvp(execname, execargs);
      exit(1);
    default: // parent

      // wait for child to finish before parsing output
      close(stdfd[1]);
      wait(NULL);
      break;
  }

  // track largest name number and description strings
  int maxname = 0;
  int maxnum = 0;
  int maxdesc = 0;

  // make head of error list
  struct error_t *headnode;
  init_error_struct(&headnode);
  init_const_str(&(headnode->name), nameheader);
  init_const_str(&(headnode->num), numheader);
  init_const_str(&(headnode->desc), descheader);
  headnode->next = NULL;

  // make error list
  struct errlist_t *elist;
  init_errlist_struct(&elist);
  append_err(elist, headnode);

  // setup buffer for input
  struct buf_t *buffer;
  init_buf_struct(&buffer, BUFSIZE);

  // read everything into buffer
  int nbytes = 0;
  while (read_to_buf(buffer, stdfd[0], &nbytes) < 1) {

    // nothing left to read
    if (nbytes == 0) {
      break;
    }

    // find all newlines in the buffer
    struct error_t *err;
    struct field_t spaces;
    while (find_newline(buffer, &(buffer->consumed)) == 0) {

      // increment consumed, since it is given an index, not a length
      buffer->consumed++;

      // allocate a new error
      if (init_error_struct(&err) > 0) {
        break;
      }

      // locate spaces splitting name from num from desc
      if (find_spaces(buffer, &spaces) > 0) {
        // invaarg or endflst
      }

      // copy name segment of errno line into str struct
      if (assemble_tstr(buffer, &(err->name), 0, spaces.first, &maxname) > 0) {
        // invaarg or enomem
      }

      // copy number segment of errno line into str struct
      if (assemble_tstr(buffer, &(err->num), spaces.first + 1, spaces.second, &maxnum) > 0) {
        // invaarg or enomem
      }

      // copy description segment of errno line into str struct
      if (assemble_tstr(buffer, &(err->desc), spaces.second + 1, buffer->consumed - 1, &maxdesc) > 0) {
        // invaarg or enomem
      }

      // append error to list
      if (append_err(elist, err) > 0) {
        // invaarg
      }

      // clear consumed
      if (replace_consumed(buffer) > 0) {
        // invaarg
      }

      // reset working data
      err = NULL;
      spaces.first = -1;
      spaces.second = -1;
    }

  }

  // now that we're done reading and the error list is assembled, start printing!
  // loop through errlist and print with aligns based on the max trackers
  struct error_t *cur;
  for (cur = elist->head; cur != NULL; cur = cur->next) {
    printf("%*.*s ", maxname, cur->name.len, cur->name.buf);
    printf("%-*.*s ", maxnum, cur->num.len, cur->num.buf);
    printf("%-*.*s\n", maxdesc, cur->desc.len, cur->desc.buf);
  }

  // iterate through list of errors, dealloc them all
  //struct error_t *cur;
  struct error_t *next;
  for (cur = elist->head; cur != NULL; cur = next) {
    next = cur->next;
    free(cur->name.buf);
    free(cur->num.buf);
    free(cur->desc.buf);
    free(cur);
    cur = NULL;
  }

  // dealloc global structures
  free(elist);

  free(buffer->buf);
  free(buffer);

  exit(0);
}
