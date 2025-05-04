#pragma once
#include "set.h"
#include <stdbool.h>

/******************** CONSTANTS ********************/
#define MAX_LINE_LENGTH 400

/******************** METHODS ********************/

/*
  This function is used to compare elements of ints used by quicksort
*/
int compare(const void *a, const void *b);

/*
  This function encodes a char as an int, for working with character items
*/
int char_to_index(char c);

/*
  This reverses the an encoded char from its int representation
*/
char index_to_char(int i);

/*
  Returns the number of lines of file with name filename
*/
int count_lines_in_file(const char *filename);

/*
  Assigns and reads transactions from file filename for each mpi process
  num_transaction pointer will store the number of transactions read
  partition_size pointer will store how many transaction are stored by each
  process local_size pointer will store how many transaction are stored by the
  specific process, it differs in case the transactions are not dividable rank
  of the mpi process, size the number of processes characters tell if the file
  contains character data instead of integers max_transactions dictate how much
  transactions are read, incase the file is bigger
*/
Set *read_sets_from_file_start_end(const char *filename, int *num_transactions,
                                   int rank, int size, int *partition_size,
                                   int *local_size, bool characters,
                                   int max_transactions);

/*
  Reads up to max_transactions or all from the file filename
  stores the number of transactions read in num_transactions
  characters tell if the file contains character data instead of integers
*/
Set *read_sets_from_file(const char *filename, int *num_transactions,
                         bool characters, int max_transactions);
