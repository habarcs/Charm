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
  initial_cap has to be greater than 0
  overwrites size, cap and itpairs
  use only once per ITArray
*/
void itarray_init(ITArray *array, int initial_cap);

/*
  frees the dynamic memory of array
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
*/
void add_itpair(ITArray *array, ITPair itpair);

/*
  adds itpair to the array C, if it isn't subsumed
  C has to be a valid pointer to ITArray
*/
void add_itemset_if_not_subsumed(ITArray *C, ITPair itpair);

/*
  removes all itpairs from C if they are already subsumed by other itpairs in C
  C has to be a valid pointer to ITArray
*/
void remove_subsumed_sets(ITArray *C);

/*
  removes itpair at position pos from P
  P has to be a valid pointer to ITArray
*/
void remove_itpair(ITArray *P, int pos);

/*
  replaces every occurence of set it with set with in all itemsets of ITArray P
  P has to be a valid pointer to ITArray
*/
void replace_with(ITArray *P, Set it, Set with);

/*
  prints the ITPairs of C to stdout
  if character is true they are printed as if the ints are characters
  C has to be a valid pointer to ITArray
*/
void print_closed_itemsets(ITArray *C, bool character);
