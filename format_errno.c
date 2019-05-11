#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ERRLEN 256

char execname[] = "errno";
char *execargs[] = {"errno", "-l", NULL};

// data structure for holding errors is organized in a Linked list
// this format allows an arbitrary length of errors from errno
// all fields are on the heap
struct error_struct {
  char *name;
  char *number;
  char *description;
  struct error_struct *next;
};
typedef struct error_struct Error;

// goes to the bottom of the list of error structs, mallocs a new space and
// copies the values of the given error into the heap space
int append(Error new, Error *root) {
  if (root == NULL) {
    return -1;
  }

  while (root->next != NULL) {
    root = root->next;
  }

  root->next = malloc(sizeof(Error));
  *(root->next) = new;

  return 0;
}

// assumes buffer has size ERRLEN
int read_to_buf(int fd, char *buffer, int *filled) {
  if (fd < 0 || buffer == NULL || filled == NULL) {
    return -1;
  }

  if (*filled >= ERRLEN) {
    return 0;
  }

  char *head = buffer + *filled;
  int left = ERRLEN - *filled;

  int nbytes = read(fd, head, left);
  //if (nbytes == EOF) return -1;
  if (nbytes > 0) {
    *filled += nbytes;
    //left -= nbytes;
    //head += nbytes;
    return nbytes;
  } else if (nbytes == 0) {
    return 0;
  } else {
    return -1;
  }

  return -1;
}

int find_newline(const char *buffer, int size) {
  if (buffer == NULL || size < 1) {
    return -1;
  }

  int index;
  for (index = 0; index < size; index++) {
    if (buffer[index] == '\n') {
      return index;
    }
  }

  return -1;
}

int assemble_error(char *buffer, int size, Error *err, int *maxname, int *maxnum, int *maxdesc) {

  // find the first two spaces in the line
  int findex = -1;
  int sindex = -1;
  for (int i = 0; i < size; i++) {
    if (buffer[i] == ' ') {
      if (findex == -1) {
        findex = i;
      } else if (sindex == -1) {
        sindex = i;
      } else {
        break;
      }
    }
  }

  // copy from the start to the first space into a separate string
  char errname[ERRLEN];
  strncpy(errname, buffer + 0, findex);
  errname[findex] = '\0';
  int fnamelen = strlen(errname);

  if (fnamelen > *maxname) *maxname = fnamelen;

  // copy from the first space to the second into a separate string
  char errnum[ERRLEN];
  int numlen = (sindex - findex) - 1; // the amount of characters between
  strncpy(errnum, buffer + findex + 1, numlen);
  errnum[numlen] = '\0';
  int fnumlen = strlen(errnum);

  if (fnumlen > *maxnum) *maxnum = fnumlen;

  // copy from the second space to the end into a separate string
  char errdesc[ERRLEN];
  int desclen = (size - 1) - sindex;
  strncpy(errdesc, buffer + sindex + 1, desclen);
  errdesc[desclen] = '\0';
  int fdesclen = strlen(errdesc);

  if (fdesclen > *maxdesc) *maxdesc = fdesclen;

  // make a heap string that holds exactly the string we've read
  int data_namelen = sizeof(char) * fnamelen + 1;
  char *nname = malloc(data_namelen);
  strncpy(nname, errname, data_namelen);

  int data_numlen = sizeof(char) * fnumlen + 1;
  char *nnum = malloc(data_numlen);
  strncpy(nnum, errnum, data_numlen);

  int data_desclen = sizeof(char) * fdesclen + 1;
  char *ndesc = malloc(data_desclen);
  strncpy(ndesc, errdesc, data_desclen);

  err->name = nname;
  err->number = nnum;
  err->description = ndesc;
  err->next = NULL;

  return 0;
}

int main() {

  // make pipe for capturing errno output
  int stdfd[2];
  pipe(stdfd);

  // fork process to run "errno -l"
  switch(fork()) {
    case 0:
      // replace stdout with pipe
      close(stdfd[0]);
      dup2(stdfd[1], STDOUT_FILENO);
      close(stdfd[1]);

      dup2(stdfd[1], STDERR_FILENO);

      execvp(execname, execargs);
      exit(1);
    default:
      //close(stdfd[0]);
      close(stdfd[1]);

      // wait for child to exit
      wait(NULL);
      break;
  }

  // tracking the largest strings of each category
  int maxname = 0;
  int maxnum = 0;
  int maxdesc = 0;

  // dummy head node
  Error *root = malloc(sizeof(Error));
  root->name = "NAME";
  root->number = "NUM";
  root->description = "DESCRIPTION";
  root->next = NULL;

  // tracked buffer
  char errline[ERRLEN];
  int inbuf = 0;
  int consumed = 0;

  while (read_to_buf(stdfd[0], errline, &inbuf) > 0) {

    consumed = find_newline(errline + consumed, inbuf);

    Error nerr;
    assemble_error(errline, consumed, &nerr, &maxname, &maxnum, &maxdesc);

    // put this at the bottom of the list
    append(nerr, root);

    memset(errline, 0, consumed);
    memmove(errline, errline + consumed + 1, inbuf - consumed);

    inbuf = inbuf - consumed;
    consumed = 0;
  }

  // iterate through each element, print it out with nice spacing
  Error *cur;
  for (cur = root; cur->next != NULL; cur = cur->next) {
    printf("%*s ", maxname, cur->name);
    printf("%-*s ", maxnum, cur->number);
    printf("%-*s\n", maxdesc, cur->description);
  }

  return 0;
}
