#ifndef __DATA_ERRNO_H__
#define __DATA_ERRNO_H__

struct str_t {
  char *buf;
  int len;
};

struct buf_t {
  char *buf;
  int len;
  int inbuf;
  int consumed;
};

int read_to_buf(struct buf_t *buffer, int input, int *received);

int find_newline(struct buf_t *buffer, int *location);

int assemble_tstr(struct buf_t *buffer, struct str_t *string, int begindex, int endex, int *maximum);

int find_splitter(struct buf_t *buffer, int spaces[2]);

int replace_consumed(struct buf_t *buffer);

#endif
