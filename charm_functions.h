#pragma once
#include "itarray.h"
#include "utils.h"

/*
  main entry function for charm, returns an ITArray that contains the closed
  itemsets transactions is a valid pointer to the list of transactions to
  process num_transactions is the length of the list min_support determines what
  frequency of itemsets is considered as frequent tid_start tells where the
  numbering of transaction ids start from, usefull for MPI implementation
*/
ITArray charm(Set *transactions, int num_transactions, int min_support,
              int tid_start);
