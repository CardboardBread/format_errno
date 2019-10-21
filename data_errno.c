#include <stdlib.h>

#include "data_errno.h"
#include "error_errno.h"

int read_to_buf(struct buf_t *buffer, int input, int *received) {
  // check valid inputs
  if (buffer == NULL || input < 0) {
    return INVAARG;
  }

  // buffer full case
  if (buffer->inbuf == buffer->len) {
    return SUCCESS;
  }

  // calculate open space
  char *head = buffer->buf + buffer->inbuf;
  int left = buffer->len - buffer->inbuf;

  // read as much as possible
  int readlen = read(input, head, left);

  // check for error
  if (readlen < 0) {
    return LIBFAIL;
  }

  // mark buffer as written into
  buffer->inbuf += readlen;

  // report how much was read
  if (received != NULL) {
    *received = readlen;
  }

  return SUCCESS;
}

int find_newline(struct buf_t *buffer, int *location) {
  // check valid arguments
  if (buffer == NULL || location == NULL) {
    return INVAARG;
  }

  // loop until newline is found
  int index;
  for (index = buffer->consumed; index < buffer->inbuf; index++) {

    // is a newline here?
    if (buffer->buf[index] == '\n') {
      *location = index;
      return SUCCESS;
    }
  }

  return ENDFLST;
}

int assemble_tstr(struct buf_t *buffer, struct str_t *string, int begindex, int endex, int *maximum) {
  // check valid arguments
  if (buffer == NULL || string == NULL || begindex < 0 || endex > buffer->len) {
    return INVAARG;
  }

  // calc how much space is needed
  int slen = endex - begindex;

  // attempt to malloc a space large enough
  char *loc = malloc(sizeof(char) * slen);
  if (loc == NULL) {
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

  return SUCCESS;
}

int find_splitter(struct buf_t *buffer, int spaces[2]) {
  // check valid arguments
  if (buffer == NULL || spaces == NULL) {
    return INVAARG;
  }

  // reset spaces to invalid
  spaces[0] = -1;
  spaces[1] = -1;

  // loop through the consumed data
  int i;
  for (i = 0; i < buffer->consumed; i++) {
    // is this index a space?
    if (buffer->buf[i] == ' ') {
      // save the index of the first and second space
      if (spaces[0] == -1) {
        // record location of first space
        spaces[0] = i;
        continue;
      } else if (spaces[1] == -1) {
        // record location of second space, stop looping
        spaces[1] = i;
        break;
      } else {
        break;
      }
    }
  }

  return SUCCESS;
}

int replace_consumed(struct buf_t *buffer) {
  // check invalid arguments
  if (buffer == NULL) {
    return INVAARG;
  }

  // how much space has not been parsed
  int unconsumed = buffer->inbuf - buffer->consumed;

  // check if nothing should be moved/reset
  if (unconsumed < 1) {
    return SUCCESS:
  }

  // set all consumed to zero
  memset(buffer->buf, 0, buffer->consumed);

  // move everything not consumed on top of the consumed
  memmove(buffer->buf, buffer->buf + consumed, unconsumed);

  // move buffer tracking fields
  buffer->inbuf -= buffer->consumed;
  buffer->consumed = 0;

  return SUCCESS;
}
