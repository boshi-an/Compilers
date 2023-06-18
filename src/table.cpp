#include "table.h"

SymbolTable global_table;
vector<SymbolTable> local_table;
vector<int> table_level;
int block_return;
int if_cnt;
vector<int> while_stack;