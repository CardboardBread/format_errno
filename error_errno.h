#ifndef __ERROR_ERRNO_H__
#define __ERROR_ERRNO_H__

#include "data_errno.h"

#define SUCCESS 0 // Success!
#define INVAARG 1 // Invalid Argument(s)
#define INDXDNE 2 // Index does not exist
#define ENDFLST 3 // End of list
#define LIBFAIL 4 // Library call failure

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

int init_error_struct(struct error_t **target);

int init_errlist_struct(struct errlist_t **target);

#endif
