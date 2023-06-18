#include "ast.h"
#include "table.h"

int used_variable;

bool is_global() {
    return local_table.size() == 1;
}

bool is_sys_lib(string name) {
    if(name == "main") return true;
    if(name == "getint") return true;
    if(name == "getch") return true;
    if(name == "getarray") return true;
    if(name == "putint") return true;
    if(name == "putch") return true;
    if(name == "putarray") return true;
    if(name == "starttime") return true;
    if(name == "stoptime") return true;
    return false;
}

string get_symbol_prefix(int level) {
    string prefix = "_" + std::to_string(level);
    for(int i=1; i<=level; i++) {
        prefix += string("_") + to_string(table_level[i]);
    }
    return prefix;
}

Symbol &get_symbol(string name) {
    if (is_sys_lib(name)) return local_table[0].get(name);
    printf("getting: %s\n", name.c_str());
    for(int i=local_table.size()-1; i>=0; i--) {
        string cur_name = get_symbol_prefix(i) + name;
        printf("\t%s\n", cur_name.c_str());
        if(local_table[i].check(cur_name)) {
            return local_table[i].get(cur_name);
        }
    }
    assert(0);
}

void add_variable(string name, bool is_const=0, int value=0, bool is_array=0, bool is_pointer=0, int array_dim=0) {
    Symbol &var = (local_table.end()-1)->add_symbol(name);
    if (is_const) {
        var.const_symbol = true;
        var.value = value;
    }
    if (is_array) {
        var.array_symbol = true;
        var.pointer_symbol = is_pointer;
        var.dim = array_dim;
    }
    printf("adding var: %s\n", name.c_str());
}

void add_function(string name, bool is_void=0) {
    Symbol &fun = (local_table.end()-1)->add_symbol(name);
    if (is_void) {
        fun.void_symbol = true;
    }
    printf("adding func: %s\n", name.c_str());
}

void ConstDeclAST::push_back(ConstDeclBlockAST* const_decl_block) {
    std::move(const_decl_block->name.begin(), const_decl_block->name.end(), std::back_inserter(const_def_list));
}

void ConstDeclAST::update_symbol_table() {
    // printf("update symbol table\n");
    for(int i=0; i<const_def_list.size(); i++)
    {
        ConstDefAST* derived = dynamic_cast<ConstDefAST*>(const_def_list[i].get());
        string name = get_symbol_prefix(local_table.size()-1) + derived->name;
        printf("const_def_list[%d]: %s [%s] = %d\n", i, derived->name.c_str(), name.c_str(), const_def_list[i]->value());
        if (!derived->is_array) {
            add_variable(name, 1, const_def_list[i]->value());
        }
        else {
            auto derived_array = dynamic_cast<ConstDefAST_Array*>(derived);
            auto derived_shape = dynamic_cast<ArrayDefIndexListAST*>(derived_array->shape.get());
            // printf("ADDED ARRAY: %s, SIZE %d\n", name.c_str(), derived_shape->index_list.size());
            add_variable(name, 1, const_def_list[i]->value(), 1, 0, derived_shape->index_list.size());
        }
    }
}

void ConstDeclBlockAST::push_back(ConstDeclBlockAST* const_decl_block) {
    std::move(const_decl_block->name.begin(), const_decl_block->name.end(), std::back_inserter(name));
}

void BlockAST::push_back(BlockBlockAST* block_block) {
    std::move(block_block->block_list.begin(), block_block->block_list.end(), std::back_inserter(stmt_list));
}

void BlockBlockAST::push_back(BlockBlockAST* block_block) {
    std::move(block_block->block_list.begin(), block_block->block_list.end(), std::back_inserter(block_list));
}

void VarDeclAST::push_back(VarDeclBlockAST* var_decl_block) {
    std::move(var_decl_block->name.begin(), var_decl_block->name.end(), std::back_inserter(var_def_list));
}

void VarDeclAST::update_symbol_table() {
    for(int i=0; i<var_def_list.size(); i++)
    {
        VarDefAST* derived = dynamic_cast<VarDefAST*>(var_def_list[i].get());
        string name = get_symbol_prefix(local_table.size()-1) + derived->name;
        printf("var_def_list[%d]: %s [%s]\n", i, derived->name.c_str(), name.c_str());
        if (!derived->is_array) {
            add_variable(name, 0, 0);
        }
        else {
            auto derived_array = dynamic_cast<VarDefAST_Array*>(derived);
            auto derived_shape = dynamic_cast<ArrayDefIndexListAST*>(derived_array->shape.get());
            // printf("ADDED ARRAY: %s, SIZE %d\n", name.c_str(), derived_shape->index_list.size());
            add_variable(name, 0, 0, 1, 0, derived_shape->index_list.size());
        }
    }
}

void VarDeclBlockAST::push_back(VarDeclBlockAST* var_decl_block) {
    std::move(var_decl_block->name.begin(), var_decl_block->name.end(), std::back_inserter(name));
}

void CompUnitAST::push_back(CompUnitAST* comp_unit_block) {
    std::move(comp_unit_block->func_def.begin(), comp_unit_block->func_def.end(), std::back_inserter(func_def));
}

void FuncFParamsAST::push_back(FuncFParamsAST* func_params_block) {
    std::move(func_params_block->fparam_list.begin(), func_params_block->fparam_list.end(), std::back_inserter(fparam_list));
}

void FuncFParamsAST::update_symbol_table() {
    for(int i=0; i<fparam_list.size(); i++)
    {
        FuncFParamAST* derived = dynamic_cast<FuncFParamAST*>(fparam_list[i].get());
        string name = get_symbol_prefix(local_table.size()-1) + derived->name;
        printf("fparam_list[%d]: %s [%s]\n", i, derived->name.c_str(), name.c_str());
        if (!derived->is_array) {
            add_variable(name, 0, 0);
        }
        else {
            auto derived_array = dynamic_cast<FuncFParamAST_Array*>(derived);
            auto derived_shape = dynamic_cast<ArrayDefIndexListAST*>(derived_array->shape.get());
            // printf("ADDED ARRAY: %s, SIZE %d\n", name.c_str(), derived_shape->index_list.size());
            add_variable(name, 0, 0, 1, 1, derived_shape->index_list.size()+1);
        }
    }
}

void FuncDefAST::update_symbol_table() {
    string name;
    if (!is_sys_lib(ident)) name = get_symbol_prefix(local_table.size()-1) + ident;
    else name = ident;
    printf("func_def: %s [%s]\n", ident.c_str(), name.c_str());
    auto func_type_str = dynamic_cast<FuncTypeAST*>(func_type.get())->type;
    add_function(name, func_type_str == "void");
}

void FuncRParamsAST::push_back(FuncRParamsAST* func_params_block) {
    std::move(func_params_block->rparam_list.begin(), func_params_block->rparam_list.end(), std::back_inserter(rparam_list));
}

void InitValListAST::push_back(InitValListAST* init_val_list_block) {
    std::move(init_val_list_block->init_val_list.begin(), init_val_list_block->init_val_list.end(), std::back_inserter(init_val_list));
}

void ConstInitValListAST::push_back(ConstInitValListAST* const_init_val_list_block) {
    std::move(const_init_val_list_block->const_init_val_list.begin(), const_init_val_list_block->const_init_val_list.end(), std::back_inserter(const_init_val_list));
}

void ArrayDefIndexListAST::push_back(ArrayDefIndexListAST* array_def_index_list_block) {
    std::move(array_def_index_list_block->index_list.begin(), array_def_index_list_block->index_list.end(), std::back_inserter(index_list));
}

void ArrayAccessIndexListAST::push_back(ArrayAccessIndexListAST* array_access_index_list_block) {
    std::move(array_access_index_list_block->index_list.begin(), array_access_index_list_block->index_list.end(), std::back_inserter(index_list));
}

void new_local_table() {
    local_table.push_back(SymbolTable());
    if(local_table.size() > table_level.size()) {
        table_level.push_back(0);
    }
    else {
        table_level[local_table.size()-1]++;
    }
}

void delete_local_table() {
    local_table.pop_back();
}

string access_array(Symbol symbol, std::unique_ptr<BaseAST> &index, int indent=0) {
    ArrayAccessIndexListAST *derived = dynamic_cast<ArrayAccessIndexListAST*>(index.get());
    string str;
    str += derived->prepare_index(indent);
    // for (int i=1; i<derived->index_list.size(); i++) {
    //     printf("%d ", derived->saved_index_list[i]);
    // }
    // printf("\n");
    if (symbol.pointer_symbol) {
        str += indent_space(indent) + "%" + std::to_string(++used_variable) + " = load @" + symbol.name + "\n";
        int last_variable = used_variable;
        if (derived->index_list[0]->is_number()) {
            str += indent_space(indent) + "%" + std::to_string(++used_variable) + " = getptr %" + std::to_string(last_variable) + ", " + std::to_string(derived->index_list[0]->value()) + '\n';
        }
        else {
            str += indent_space(indent) + "%" + std::to_string(++used_variable) + " = getptr %" + std::to_string(last_variable) + ", %" + std::to_string(derived->saved_index_list[0]) + '\n';
        }
    }
    else {
        if (derived->index_list[0]->is_number()) {
            str += indent_space(indent) + "%" + std::to_string(++used_variable) + " = getelemptr @" + symbol.name + ", " + std::to_string(derived->index_list[0]->value()) + '\n';
        }
        else {
            str += indent_space(indent) + "%" + std::to_string(++used_variable) + " = getelemptr @" + symbol.name + ", %" + std::to_string(derived->saved_index_list[0]) + '\n';
        }
    }
    int last_variable = used_variable;
    for (int i=1; i<derived->index_list.size(); i++) {
        if (derived->index_list[i]->is_number()) {
            str += indent_space(indent) + "%" + std::to_string(++used_variable) + " = getelemptr %" + std::to_string(last_variable) + ", " + std::to_string(derived->index_list[i]->value()) + '\n';
        }
        else {
            str += indent_space(indent) + "%" + std::to_string(++used_variable) + " = getelemptr %" + std::to_string(last_variable) + ", %" + std::to_string(derived->saved_index_list[i]) + '\n';
        }
        last_variable = used_variable;
    }
    return str;
}