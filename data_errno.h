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

struct field_t {
  int first;
  int second;
};

int read_to_buf(struct buf_t *buffer, int input, int *received);

int find_newline(struct buf_t *buffer, int *location);

int assemble_tstr(struct buf_t *buffer, struct str_t *string, int begindex, int endex, int *maximum);

int find_spaces(struct buf_t *buffer, struct field_t *spaces);

int replace_consumed(struct buf_t *buffer);

int init_const_str(struct str_t *string, const char *src);

int init_str_struct(struct str_t **target);

int init_buf_struct(struct buf_t **target, const int size);

int init_field_struct(struct field_t **target);

#endif
