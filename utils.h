#pragma once
#include <stdbool.h>

/******************** CONSTANTS ********************/
#define ARRAY_SIZE 20

/******************** STRUCTS ********************/
typedef struct {
  int set[ARRAY_SIZE];
  int size;
} Set;

typedef struct {
  Set itemset;
  Set tidset;
} ITPair;

typedef struct {
  ITPair itpairs[ARRAY_SIZE];
  int size;
} ITArray;

/******************** METHODS ********************/

int compare(const void *a, const void *b);
void out_of_bounds(int i);
Set set_union(Set a, Set b);
Set set_intersect(Set a, Set b);
bool is_subset(Set sub, Set sup);
bool sets_equal(Set a, Set b);
int compare_sets(Set a, Set b);
int compare_itpairs(const void *a, const void *b);
void add_itemset_if_not_subsumed(ITArray *C, ITPair itpair);
void remove_itpair(ITArray *P, int pos);
void replace_with(ITArray *P, Set it, Set with);
void print_closed_itemsets(ITArray C, bool character);
