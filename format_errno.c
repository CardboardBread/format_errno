#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ERRLEN 256

typedef struct Errors { // all fields are on the heap
  char *name;
  char *number;
  char *description;
  struct Errors *next;
} Error;

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

  Error *root = malloc(sizeof(Error));
  root->name = NULL;
  root->number = NULL;
  root->description = NULL;
  root->next = NULL;

  char errline[ERRLEN];
  while (fgets(errline, ERRLEN, stdin) != NULL) {
    int linelen = strlen(errline);

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

    char errname[ERRLEN];
    strncpy(errname, &errline[0], findex);
    errname[findex] = '\0';
    int fnamelen = strlen(errname);

    if (fnamelen > maxname) maxname = fnamelen;

    char errnum[ERRLEN];
    int numlen = (sindex - findex) - 1; // the amount of characters between
    strncpy(errnum, &errline[findex + 1], numlen);
    errnum[numlen] = '\0';
    int fnumlen = strlen(errnum);

    if (fnumlen > maxnum) maxnum = fnumlen;

    char errdesc[ERRLEN];
    int desclen = (linelen - 1) - sindex;
    strncpy(errdesc, &errline[sindex + 1], desclen);
    errdesc[desclen] = '\0';
    int fdesclen = strlen(errdesc);

    if (fdesclen > maxdesc) maxdesc = fdesclen;

    // make new error struct, append it to root list;
    char *nname = malloc(sizeof(char) * fnamelen + 1);
    strcpy(nname, errname);

    char *nnum = malloc(sizeof(char) * fnumlen + 1);
    strcpy(nnum, errnum);

    char *ndesc = malloc(sizeof(char) * fdesclen + 1);
    strcpy(ndesc, errdesc);

    Error nerr;
    nerr.name = nname;
    nerr.number = nnum;
    nerr.description = ndesc;
    nerr.next = NULL;

    append(nerr, root);
  }

  Error *cur;
  for (cur = root->next; cur->next != NULL; cur = cur->next) {
    printf("%*s ", maxname, cur->name);
    printf("%s ", cur->number);
    printf("%s", cur->description);
  }

  return 0;
}
