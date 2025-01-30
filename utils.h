#pragma once
#include <stdbool.h>

/******************** CONSTANTS ********************/
#define MAX_LINE_LENGTH 400
#define MAX_TRANSACTIONS 10000

/******************** STRUCTS ********************/

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
int compare_itpairs(const void *a, const void *b);
void add_itemset_if_not_subsumed(ITArray *C, ITPair itpair);
void remove_subsumed_sets(ITArray *C);
void remove_itpair(ITArray *P, int pos);
void replace_with(ITArray *P, Set it, Set with);
void print_closed_itemsets(ITArray C, bool character);
int char_to_index(char c);
char index_to_char(int i);
int count_lines_in_file(const char *filename);
Set *read_sets_from_file_start_end(const char *filename, int *num_transactions,
                                   int rank, int size, int *partition_size,
                                   int *local_size, bool characters);
Set *read_sets_from_file(const char *filename, int *num_transactions,
                         bool characters);
