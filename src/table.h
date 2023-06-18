#ifndef __TABLE_H__
#define __TABLE_H__

#include <memory>
#include <string>
#include <iostream>
#include <assert.h>
#include <vector>
#include <set>
#include "helpers.h"

struct Symbol{
    string name;
    int value;
    bool const_symbol;
    bool void_symbol;
    bool func_symbol;
    bool pointer_symbol;
    bool array_symbol;
    int dim;

    Symbol(string name)
        : name(name), value(0), const_symbol(0), void_symbol(0), func_symbol(0), pointer_symbol(0), array_symbol(0) {}
};

class SymbolTable
{
private:
    vector<Symbol> table;
public:
    Symbol &add_symbol(string name) {
        assert(!check(name));
        table.push_back(Symbol(name));
        return table.back();
    }

    bool check(string name) {
        for (auto i : table) {
            if (i.name == name) {
                return true;
            }
        }
        return false;
    }

    Symbol &get(string name) {
        for (int i=0; i<table.size(); i++) {
            if (table[i].name == name) {
                return table[i];
            }
        }
        assert(0);
    }
};

extern SymbolTable global_table;
extern vector<SymbolTable> local_table;
extern vector<int> table_level;
extern int block_return;
extern int if_cnt;
extern vector<int> while_stack;

#endif