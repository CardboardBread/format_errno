#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "data_errno.h"
#include "error_errno.h"

int read_to_buf(struct buf_t *buffer, int input, int *received) {
  // check valid inputs
  if (buffer == NULL || input < 0) {
    printf("buffer: invaarg\n");
    return INVAARG;
  }

  // buffer full case
  if (buffer->inbuf == buffer->len) {
    printf("buffer: full\n");
    return SUCCESS;
  }

  // calculate open space
  char *head = buffer->buf + buffer->inbuf;
  int left = buffer->len - buffer->inbuf;

  // read as much as possible
  int readlen = read(input, head, left);

  // check for error
  if (readlen < 0) {
    printf("buffer: readfail\n");
    return LIBFAIL;
  }

  // increment buffer's written space
  buffer->inbuf += readlen;

  // report how much was read
  if (received != NULL) {
    *received = readlen;
  }

  printf("buffer: read %d\n", readlen);
  return SUCCESS;
}

int find_newline(struct buf_t *buffer, int *location) {
  // check valid arguments
  if (buffer == NULL) {
    printf("newline: invaarg\n");
    return INVAARG;
  }

  printf("\"%.*s\"\n", buffer->inbuf, buffer->buf);

  // loop through unconsumed until newline is found
  int index;
  for (index = buffer->consumed; index < buffer->inbuf; index++) {

    // is a newline here?
    if (buffer->buf[index] == '\n') {

      // record index if possible
      if (location != NULL) {
        *location = index;
      }


      printf("newline: index %d\n", index);
      printf("newline: \"%.*s\"\n", index, buffer->buf);
      return SUCCESS;
    }

  }

  printf("newline: endflst\n");
  return ENDFLST;
}

int assemble_tstr(struct buf_t *buffer, struct str_t *string, int begindex, int endex, int *maximum) {
  // check valid arguments
  if (buffer == NULL || string == NULL || begindex < 0 || endex > buffer->len) {
    printf("tstr: invaarg\n");
    return INVAARG;
  }

  // calc how much space is needed
  int slen = endex - begindex;


  // attempt to malloc a space large enough, plus a terminator
  char *loc = malloc(sizeof(char) * (slen + 1));
  if (loc == NULL) {
    printf("tstr: libfail\n");
    return LIBFAIL;
  }

  // update the maximum if possible
  if (maximum != NULL) {
    if (slen > *maximum) {
      *maximum = slen;
    }
  }

  // copy data into string struct
  string->buf = loc;
  memmove(string->buf, buffer->buf, slen);
  string->buf[slen] = '\0';
  string->len = slen;

  printf("tstr: copy\n");
  return SUCCESS;
}

int find_spaces(struct buf_t *buffer, struct field_t *spaces) {
  // check valid arguments
  if (buffer == NULL || spaces == NULL) {
    printf("space: invaarg\n");
    return INVAARG;
  }

  // reset spaces to invalid
  spaces->first = -1;
  spaces->second = -1;

  // loop through the consumed data
  int i;
  for (i = 0; i < buffer->consumed; i++) {
    // is this index a space?
    if (buffer->buf[i] == ' ') {
      // save the index of the first and second space
      if (spaces->first == -1) {
        // record location of first space
        spaces->first = i;
        continue;
      } else if (spaces->second == -1) {
        // record location of second space, stop looping
        spaces->second = i;
        break;
      } else {
        // in case a third space is encountered
        break;
      }
    }

  }

  // make sure two spaces were found
  if (spaces->first > -1 && spaces->second > -1) {
    printf("space: find %d, %d\n", spaces->first, spaces->second);
    return SUCCESS;
  } else {
    printf("space: endflst\n");
    return ENDFLST;
  }

}

int replace_consumed(struct buf_t *buffer) {
  // check invalid arguments
  if (buffer == NULL) {
    printf("repl: invaarg\n");
    return INVAARG;
  }

  // how much space has not been parsed
  int unconsumed = buffer->inbuf - buffer->consumed;

  // check if nothing should be moved/reset
  if (unconsumed < 1) {
    printf("repl: nothing\n");
    return SUCCESS;
  }

  // set all consumed to zero, plus the newline
  memset(buffer->buf, 0, buffer->consumed);

  // move everything not consumed (after newline) on top of the consumed
  memmove(buffer->buf, buffer->buf + buffer->consumed, unconsumed);

  // set leftover source to zero, including newline
  memset(buffer->buf + unconsumed, 0, buffer->consumed);

  // update buffer tracking fields
  buffer->inbuf -= buffer->consumed;
  buffer->consumed = 0;

  printf("repl: move\n");
  return SUCCESS;
}

int init_const_str(struct str_t *string, const char *src) {
  // check invalid arguments
  if (string == NULL || src == NULL) {
    return INVAARG;
  }

  // find length of input str
  int srclen = strlen(src);

  // malloc space to hold string
  char *dest = malloc(sizeof(char) * (srclen + 1));
  if (dest == NULL) {
    return LIBFAIL;
  }

  // copy input into struct
  string->buf = dest;
  memmove(string->buf, src, srclen);
  string->buf[srclen] = '\0';
  string->len = srclen;

  return SUCCESS;
}

int init_str_struct(struct str_t **target) {
  // check invalid argument
  if (target == NULL) {
    return INVAARG;
  }

  // allocate and initialize struct
  *target = malloc(sizeof(struct str_t));
  (*target)->buf = NULL;
  (*target)->len = 0;

  return SUCCESS;
}

int init_buf_struct(struct buf_t **target, const int size) {
  // check invalid argument
  if (target == NULL) {
    return INVAARG;
  }

  // allocate and initialize struct
  *target = malloc(sizeof(struct buf_t));
  (*target)->buf = malloc(sizeof(char) * size);
  memset((*target)->buf, 0, size);
  (*target)->len = size;
  (*target)->inbuf = 0;
  (*target)->consumed = 0;

  return SUCCESS;
}

int init_field_struct(struct field_t **target) {
  // check invalid argument
  if (target == NULL) {
    return INVAARG;
  }

  // allocate and initialize struct
  *target = malloc(sizeof(struct field_t));
  (*target)->first = -1;
  (*target)->second = -1;

  return SUCCESS;
}
