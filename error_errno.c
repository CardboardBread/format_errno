#include <stdlib.h>

#include "error_errno.h"
#include "data_errno.h"

// adds an error struct to the end of the given linked list of errors
// list must be null-terminated
// can be used to concatenate two lists
int append_err(struct errlist_t *list, struct error_t *new) {
  // checking valid arguments
  if (list == NULL || new == NULL) {
    return INVAARG;
  }

  // empty list case
  if (list->head == NULL) {
    list->head = new;
    list->len++;
    return SUCCESS;
  }

  // loop until null terminator (end)
  struct error_t *curr;
  for (curr = list->head; curr->next != NULL; curr = curr->next);

  // place new struct at end
  curr->next = new;

  // update tracker field
  list->len++;

  return SUCCESS;
}

// adds an error struct to the given index in a given linked list of errors
// works only null terminated lists
int add_err(struct errlist_t *list, struct error_t *new, const int index) {
  // checking valid arguments
  if (list == NULL || new == NULL || index < 0) {
    return INVAARG;
  }

  // empty list case
  if (list->head == NULL) {
    // are we adding to the beginning?
    if (index == 0) {
      list->head = new;
      list->len++;
      return SUCCESS;
    } else {
      return INDXDNE;
    }
  }

  // loop to node before target
  int track = 0;
  struct error_t *curr;
  for (curr = list->head; curr->next != NULL && track < index - 1; curr = curr->next, track++);

  if (track == index - 1) {
    // insert at index
    struct error_t *sav;
    sav = curr->next;
    curr->next = new;
    new->next = sav;
  } else {
    // index doesn't exist
    return INDXDNE;
  }

  // update tracker field
  list->len++;

  return SUCCESS;
}

// removes an error struct from the given index from the given linked list of errors
// places the removed error in the given pointer ret, if it is non-null
// works with only null terminated lists
int rem_err(struct errlist_t *list, struct error_t **ret, const int index) {
  // checking valid inputs
  if (list == NULL || list->head == NULL || index < 0) {
    return INVAARG;
  }

  // first index case
  struct error_t *sav;
  if (index == 0) {
    sav = list->head;
    list->head = sav->next;

    // can we save the removed?
    if (ret != NULL) {
      *ret = sav;
    }

    // update tracker field
    list->len--;

    return SUCCESS;
  }

  // loop to node before target
  int track = 0;
  struct error_t *curr;
  for (curr = list->head; curr->next != NULL && track < index - 1; curr = curr->next, track++);

  // did we stop before the target?
  struct error_t *out;
  if (track == index - 1) {
    // remove at index
    out = curr->next;

    // does the target exist?
    if (out != NULL) {
      sav = out->next;
      out->next = NULL;
    } else {
      // index doesn't exist
      return INDXDNE;
    }

    curr->next = sav;
  } else {
    // index doesn't exist
    return INDXDNE;
  }

  // can we save the removed?
  if (ret != NULL) {
    *ret = out;
  }

  // update tracker field
  list->len--;

  return SUCCESS;
}

// replaces an error at a given index with a given error
// places the replaced error in the given pointer old, if it is non-null
int repl_err(struct errlist_t *list, struct error_t *new, struct error_t **old, const int index) {
  // check valid inputs
  if (list == NULL || list->head == NULL || new == NULL || index < 0) {
    return INVAARG;
  }

  // first index case
  struct error_t *sav;
  if (index == 0) {
    sav = list->head;
    list->head = new;
    new->next = sav->next;
    sav->next = NULL;

    // can we save the removed?
    if (old != NULL) {
      *old = sav;
    }

    return SUCCESS;
  }

  // loop to node before target
  int track = 0;
  struct error_t *curr;
  for (curr = list->head; curr->next != NULL && track < index - 1; curr = curr->next, track++);

  // did we stop before the target?
  if (track == index - 1) {
    sav = curr->next;

    // does the target exist?
    if (sav != NULL) {
      curr->next = new;
      new->next = sav->next;
      sav->next = NULL;
    } else {
      // index doesn't exist
      return INDXDNE;
    }
  }

  // can we save the removed?
  if (old != NULL) {
    *old = sav;
  }

  return SUCCESS;
}

int init_error_struct(struct error_t **target) {
  // check invalid argument
  if (target == NULL) {
    return INVAARG;
  }

  // allocate and initialize struct
  *target = malloc(sizeof(struct error_t));
  if (*target == NULL) {
    return LIBFAIL;
  }

  (*target)->next = NULL;

  // initialize sub-structs
  (*target)->name.buf = NULL;
  (*target)->name.len = 0;

  (*target)->num.buf = NULL;
  (*target)->num.len = 0;

  (*target)->desc.buf = NULL;
  (*target)->desc.len = 0;

  return SUCCESS;
}

int init_errlist_struct(struct errlist_t **target) {
  // check invalid argument
  if (target == NULL) {
    return INVAARG;
  }

  // allocate and initialize struct
  *target = malloc(sizeof(struct errlist_t));
  if (*target == NULL) {
    return LIBFAIL;
  }
  
  (*target)->head = NULL;
  (*target)->len = 0;

  return SUCCESS;
}
