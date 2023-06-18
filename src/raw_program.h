#ifndef __RAW_PROGRAM_H__
#define __RAW_PROGRAM_H__
#include <memory>
#include <string>
#include <iostream>
#include <assert.h>
#include <koopa.h>
#include <vector>
#include <map>

using namespace std;

typedef unsigned long long ull;

static string last_reg;
static int last_delta_p;
static int delta_in_stack_frame;
static int current_stack_frame_size;
const int params_in_reg = 8;

// From stack_manager.cpp
extern map<string, int> stack_global_table;
extern map<string, vector<int> > stack_array_shape;
extern map<ull, bool> vis;
extern map<ull, int> func_ret_p;
extern int last_dim;
extern string last_name;
int get_remaining_size(string array, int dim);

// From instruction.cpp
string lw(string reg, int offset, string base_reg, string comment="");
string sw(string reg, int offset, string base_reg, string comment="");
string add(string des, string reg1, string reg2);
string addi(string des, string reg1, int imm12);
string li(string des, int imm);
string la(string des, string label);
string mul(string rd, string rs1, string rs2);

// From raw_program.cpp
int get_stack_frame_size(const koopa_raw_slice_t &slice);
int get_stack_frame_size(const koopa_raw_value_t &value, bool entrance = false);
int get_stack_frame_size(const koopa_raw_return_t &statement);
int get_stack_frame_size(const koopa_raw_integer_t &integer);
int get_stack_frame_size(const koopa_raw_binary_t &binary);
int get_stack_frame_size(const koopa_raw_branch_t &branch);
int get_stack_frame_size(const koopa_raw_basic_block_t &block);
int get_stack_frame_size(const koopa_raw_jump_t &jump);
int get_stack_frame_size(const koopa_raw_call_t &call);
int get_stack_frame_size(const koopa_raw_get_elem_ptr_t &get_elem_ptr);
int get_stack_frame_size(const koopa_raw_get_ptr_t &get_ptr);
int get_stack_frame_size(const koopa_raw_load_t &load);
int get_stack_frame_size(const koopa_raw_store_t &store);
int get_allocate_size(const koopa_raw_value_t &value, bool do_allocate);
string dfs_raw(const koopa_raw_slice_t &slice);
string dfs_raw(const koopa_raw_function_t &func);
string dfs_raw(const koopa_raw_basic_block_t &block);
string dfs_raw(const koopa_raw_value_t &value, bool entrance = false);
string dfs_raw(const koopa_raw_program_t &program);
string dfs_raw(const koopa_raw_return_t &statement);
string dfs_raw(const koopa_raw_integer_t &integer);
string dfs_raw(const koopa_raw_binary_t &binary);
string dfs_raw(const koopa_raw_branch_t &branch);
string dfs_raw(const koopa_raw_jump_t &jump);
string dfs_raw(const koopa_raw_store_t &store);
string dfs_raw(const koopa_raw_load_t &load);
string dfs_raw(const koopa_raw_call_t &call);
string dfs_raw(const koopa_raw_get_elem_ptr_t &get_elem_ptr);
string dfs_raw(const koopa_raw_get_ptr_t &get_ptr);
string allocate(const koopa_raw_value_t &value);
string ir_to_raw_program(string ir);

#endif