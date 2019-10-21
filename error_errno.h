#ifndef __ERROR_ERRNO_H__
#define __ERROR_ERRNO_H__

#include "data_errno.h"

#define SUCCESS 0
#define INVAARG 1
#define INDXDNE 2
#define ENDFLST 3
#define LIBFAIL 4

struct str_t {
  char *buf;
  int len;
};

struct error_t {
  struct str_t name;
  struct str_t num;
  struct str_t desc;
  struct error_t *next;
};

struct errlist_t {
  struct error_t *head;
  int len;
};

int append_err(struct errlist_t *list, struct error_t *new);

int add_err(struct errlist_t *list, struct error_t *new, const int index);

int rem_err(struct errlist_t *head, struct error_t **ret, const int index);

int repl_err(struct errlist_t *head, struct error_t *new, struct error_t **old, const int index);

#endif
