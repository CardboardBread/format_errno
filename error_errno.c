#include <stdlib.h>
#include "error_errno.h"
#include "data_errno.h"

// adds an error struct to the end of the given linked list of errors
// assumes the given list is null terminated, fails otherwise
// can be used to concatenate two lists
int append_err(struct errlist_t *list, struct error_t *new) {
  // checking valid arguments
  if (head == NULL || new == NULL) {
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
  for (curr = list->head; curr->next != NULL; curr = curr->next) {

    // reached end of circle
    if (curr->next == list->head) {
      new->next = list->head;
      break;
    }
  }

  // place new struct at end
  curr->next = new;

  list->len++;
  return SUCCESS;
}

// adds an error struct to the given index in a given linked list of errors
// works with circular lists as well as null terminated lists
// TODO: can you edit a pointer you got from a const pointer?
int add_err(struct errlist_t *list, struct error_t *new, const int index) {
  // checking valid arguments
  if (head == NULL || new == NULL || index < 0) {
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
  for (curr = list->head; curr->next != NULL && track < index - 1; curr = curr->next) {
    track++;

    // reached end of circle
    if (curr->next == list->head) {
      return ENDFLST;
    }
  }

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

  list->len++;
  return SUCCESS;
}

// removes an error struct from the given index from the given linked list of errors
// places the removed error in the given pointer ret, if it is non-null
// works with circular and null terminated lists
int rem_err(struct errlist_t *head, struct error_t **ret, const int index) {
  // checking valid inputs
  if (head == NULL || list->head == NULL || index < 0) {
    return INVAARG;
  }

  // first index case
  struct error_t sav;
  if (index == 0) {
    sav = list->head;

    list->head = sav->next;

    // can we save the removed?
    if (ret != NULL) {
      *ret = sav;
    }

    list->len--;
    return SUCCESS;
  }

  // loop to node before target
  int track = 0;
  struct error_t *curr;
  for (curr = list->head; curr->next != NULL && track < index - 1; curr = curr->next) {
    track++;

    // reached end of circle
    if (curr->next == list->head) {
      return ENDFLST;
    }
  }

  // did we stop before the target?
  struct error_t out;
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

  list->len--;
  return SUCCESS;
}

// replaces an error at a given index with a given error
// places the replaced error in the given pointer old, if it is non-null
int repl_err(struct errlist_t *head, struct error_t *new, struct error_t **old, const int index) {
  // check valid inputs
  if (list == NULL || list->head == NULL || new == NULL || index < 0) {
    return INVAARG;
  }

  // first index case
  struct error_t sav;
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
  for (curr = head; curr->next != NULL && track < index - 1; curr = curr->next) {
    track++;

    // reached end of circle
    if (curr->next == list->head) {
      return ENDFLST;
    }
  }

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

// scans and parses a buffer, assembling it into the given address
int assemble_error(struct buf_t *buffer, struct error_t *error) {
  // check valid arguments
  if (buffer == NULL || error == NULL) {
    return INVAARG;
  }

  // find the first two spaces
  int spaces[2];
  if (find_splitter(buffer, spaces) > 0) {
    // fail
  }
  return SUCCESS;
}
