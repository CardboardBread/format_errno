#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ERRLEN 256

// data structure for holding errors is organized in a Linked list
// this format allows an arbitrary length of errors from errno
typedef struct Errors { // all fields are on the heap
  char *name;
  char *number;
  char *description;
  struct Errors *next;
} Error;

// goes to the bottom of the list of error structs, mallocs a new space and
// copies the values of the given error into the heap space
int append(Error new, Error *root) {
  while (root->next != NULL) {
    root = root->next;
  }

  root->next = malloc(sizeof(Error));
  *(root->next) = new;

  return 0;
}

int main() {

  int maxname = 0;
  int maxnum = 0;
  int maxdesc = 0;

  Error *root = malloc(sizeof(Error)); // dummy head node
  root->name = "NAME";
  root->number = "NUM";
  root->description = "DESCRIPTION\n";
  root->next = NULL;

  char errline[ERRLEN];
  while (fgets(errline, ERRLEN, stdin) != NULL) { // read until error or EOF
    int linelen = strlen(errline);

    // find the first two spaces in the line
    int findex = -1;
    int sindex = -1;
    for (int i = 0; i < ERRLEN; i++) {
      if (errline[i] == ' ') {
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
    strncpy(errname, &errline[0], findex);
    errname[findex] = '\0';
    int fnamelen = strlen(errname);

    if (fnamelen > maxname) maxname = fnamelen;

    // copy from the first space to the second into a separate string
    char errnum[ERRLEN];
    int numlen = (sindex - findex) - 1; // the amount of characters between
    strncpy(errnum, &errline[findex + 1], numlen);
    errnum[numlen] = '\0';
    int fnumlen = strlen(errnum);

    if (fnumlen > maxnum) maxnum = fnumlen;

    // copy from the second space to the end into a separate string
    char errdesc[ERRLEN];
    int desclen = (linelen - 1) - sindex;
    strncpy(errdesc, &errline[sindex + 1], desclen);
    errdesc[desclen] = '\0';
    int fdesclen = strlen(errdesc);

    if (fdesclen > maxdesc) maxdesc = fdesclen;

    // make a heap string that holds exactly the string we've read
    char *nname = malloc(sizeof(char) * fnamelen + 1);
    strcpy(nname, errname);

    char *nnum = malloc(sizeof(char) * fnumlen + 1);
    strcpy(nnum, errnum);

    char *ndesc = malloc(sizeof(char) * fdesclen + 1);
    strcpy(ndesc, errdesc);

    // make a struct and copy the pointers in
    Error nerr;
    nerr.name = nname;
    nerr.number = nnum;
    nerr.description = ndesc;
    nerr.next = NULL;

    // put this at the bottom of the list
    append(nerr, root);
  }

  // iterate through each element, print it out with nice spacing
  Error *cur;
  for (cur = root; cur->next != NULL; cur = cur->next) {
    printf("%*s ", maxname, cur->name);
    printf("%-*s ", maxnum, cur->number);
    printf("%s", cur->description);
  }

  return 0;
}
