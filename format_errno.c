#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ERRLEN 256

// constantly reads lines from stdin until an EOF
// assumes input is from errno -l
// use would be `errno -l | ./format_errno`
// finds the indexes of the first 2 spaces
// this should split the line into 3 parts, the error macro name,
// the macro's value, and the description of the value
// this program will split the parts into 3 individual strings
// the strings are reconstructed into their original order, with tabs placed
// between the strings to keep everything spaced visible
int main() {

  char errline[ERRLEN];
  while (fgets(errline, ERRLEN, stdin) != NULL) {
    char errname[ERRLEN];
    char errnum[ERRLEN];
    char errdesc[ERRLEN];
    char errfin[ERRLEN];
    int linelen;

    linelen = strlen(errline);

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

    strncpy(errname, &errline[0], findex);
    errname[findex] = '\0';

    int numlen = (sindex - findex) - 1; // the amount of characters between
    strncpy(errnum, &errline[findex + 1], numlen);
    errnum[numlen] = '\0';

    int desclen = (linelen - 1) - sindex;
    strncpy(errdesc, &errline[sindex + 1], desclen);
    errdesc[desclen] = '\0';

    strncpy(errfin, errname, findex + 1);
    strncat(errfin, "\t\t", 2);
    strncat(errfin, errnum, numlen);
    strncat(errfin, "\t\t", 2);
    strncat(errfin, errdesc, desclen);

    //printf("err:%s,%s,%s", errname, errnum, errdesc);
    printf("%s", errfin);
  }

  return 0;
}
