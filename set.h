#pragma once
#include <stdbool.h>

#define HASH_BUCKETS 10007

typedef struct HashNode {
  Set *itemset;
  struct HashNode *next;
} HashNode;

typedef struct {
  HashNode **buckets;
} SetHash;

/*
  A Set type implements and ordered dynamic array,
  where each element is unique
  set is a pointer to a memory containing the elements
  size is the number of real elements in the array
  cap is the maximum capacity for the array
*/
typedef struct {
  int *set;
  int size;
  int cap;
} Set;

/*
  This function initializes a sets dynamic memory
  set is a pointer to a Set
  initial_cap initial capacity of the dynamic array, if zero or less 1 is used
  for the capacity the size, cap and set pointers will be overwritten use only
  once per set
*/
void set_init(Set *set, int initial_cap);

/*
  frees the dynamic memory of set
*/
void set_free(Set *set);

/*
  Adds elem to set, if it isn't already in set
  keeps the order correct
  handles capacity increase automatically
  set has to be a valid pointer to a Set
*/
void set_add(Set *set, int elem);

/*
  Initializes and fills uni with the union of a and b
  the dynamic array in uni doesn't have to be initialized before!
  a and b have to be valid pointers to Set
*/
void set_union(const Set *a, const Set *b, Set *uni);

/*
  Copies set to target
  the dynamic array in target doesn't have to be initialized before!
  set has to be a valid pointer to Set
*/
void set_copy(const Set *from, Set *target);

/*
  Similar to an inplace union, every element from "from" will be added to target
  set Both from and target have to be valid pointers to Set
*/
void set_add_all(const Set *from, Set *target);

/*
  Initializes and fills inter with the intersection of a and b
  the dynamic array in inter doesn't have to be initialized before!
  a and b have to be valid pointers to Set
*/
void set_intersect(const Set *a, const Set *b, Set *inter);

/*
  Returns true i sub is a subset of sup
  else returns false
  sub and sup have to be valid pointers to Set
*/
bool is_subset(const Set *sub, const Set *sup);

/*
  returns true if a and b are equal, else false
  a and b have to be valid pointers to Set
*/
bool sets_equal(const Set *a, const Set *b);

/*
  uses lexicographical ordering to compare sets
  a and b have to be valid pointers to Set
  return -1 if a is before b, 1 if after, 0 if they are equal
*/
int compare_sets(const Set *a, const Set *b);

void set_merge_union_sorted(const Set *from, Set *target);