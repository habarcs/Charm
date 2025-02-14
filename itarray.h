#pragma once
#include "set.h"

/*
  stores a pair of itemset and tidset
*/
typedef struct {
  Set itemset;
  Set tidset;
} ITPair;

/*
  implements a dynamic array of ITPairs
  itpairs is a pointer to an array of pairs
  size is the number of pairs in the array
  cap is the capacity of the array
*/
typedef struct {
  ITPair *itpairs;
  int size;
  int cap;
} ITArray;

/*
  initializes the dynamic memory of it array to have initial_cap size
  initial_cap if 0 or less 1 will be used for the capacity
  overwrites size, cap and itpairs
  use only once per ITArray
*/
void itarray_init(ITArray *array, int initial_cap);

/*
  frees the dynamic memory of array
  also frees the itemsets and tidsets of pairs
*/
void itarray_free(ITArray *array);

/*
  compare two itpairs based on the lexicographical ordering of their respective
  itemset useful for ordering an ITArray with qsort
  a and b have to be valid pointers to ITArrays
*/
int compare_itpairs(const void *a, const void *b);

/*
  compare two itpairs based on the support ordering of their respective itemset
  (the size of the tidset) useful for ordering an ITArray with qsort
  a and b have to be valid pointers to ITArrays
*/
int compare_itpairs_support(const void *a, const void *b);

/*
  adds itpair to the array
  array has to be a valid pointer to ITArray
  return the index to the newly added and initialized ITPair
  if the itemset and tidset pointers are NULL
  the itemset and tidset will be initialized, so can be used immediately
  if the itemset and tidset are valid pointers to sets they will be copied to
  the newly created itpair at pos
*/
int itarray_add(ITArray *array, const Set *itemset, const Set *tidset);

/*
  returns true if itpair is already subsumed by another itpair in C
  C has to be a valid pointer to ITArray
  itpair has to be a valid pointer to an itpair

*/
bool itarray_is_itpair_subsumed(ITArray *C, const ITPair *itpair);

/*
  removes all itpairs from C if they are already subsumed by other itpairs in C
  C has to be a valid pointer to ITArray
*/
void itarray_remove_subsumed_sets(ITArray *C);

/*
  removes itpair at position pos from P
  P has to be a valid pointer to ITArray
  also frees the itemset and tidset of the ITPair
*/
void itarray_remove(ITArray *P, int pos);

/*
  replaces every occurence of set it with set with in all itemsets of ITArray P
  only if it is a subset of with
  P has to be a valid pointer to ITArray it and
  with have to be valid pointers to sets
*/
void itarray_replace_with(ITArray *P, const Set *it, const Set *with);

/*
  prints the ITPairs of C to stdout
  if character is true they are printed as if the ints are characters
  C has to be a valid pointer to ITArray
*/
void print_closed_itemsets(ITArray *C, bool character);

/*
  this functions merges from ITArray into to
  both to and from have to be valid pointers to ITArray
*/
void merge_closed_itemsets_into(const ITArray *from, ITArray *to);

/*
  serialize an ITArray to an integer array ready to send.
  data is the ITArray to serialize
  buffer is the output array
  ([number of pairs, number of items in first itemset, items, number of tids in
  first tidset, tids, ...]) bufsize is the size of the output array
*/
void serialize_itarray(const ITArray *data, int **buffer, int *bufsize);

/*
  deserialize a buffer of integers to an ITArray.
  buffer is the integers array
  data is the output ITArray
*/
void deserialize_itarray(int *buffer, ITArray *data);
