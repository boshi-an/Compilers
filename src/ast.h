#ifndef __AST_H__
#define __AST_H__
#include <memory>
#include <string>
#include <iostream>
#include <assert.h>
#include <algorithm> 
#include <vector>
#include "helpers.h"
#include "table.h"

using namespace std;

// Declare all classes
class BaseAST;
class LibDefAST;
class RootAST;
class CompUnitAST;
class NumberAST;
class StmtAST;
class StmtAST_Return;
class StmtAST_Return_Empty;
class StmtAST_Exp;
class StmtAST_If;
class StmtAST_Block;
class StmtAST_LVal_Exp;
class StmtAST_LValArray_Exp;
class StmtAST_While;
class StmtAST_Break;
class StmtAST_Continue;
class BlockAST;
class BlockBlockAST;
class BlockItemAST;
class BlockItemAST_Decl;
class BlockItemAST_Stmt;
class FuncDefAST;
class FuncTypeAST;
class FuncFParamsAST;
class FuncFParamAST;
class FuncFParamAST_Array;
class FuncRParamsAST;
class ExpAST;
class PrimaryExpAST;
class PrimaryExpAST_Exp;
class PrimaryExpAST_Number;
class PrimaryExpAST_LVal;
class PrimaryExpAST_LValArray;
class UnaryExpAST;
class UnaryExpAST_PrimaryExp;
class UnaryOPAST;
class UnaryExpAST_UnaryOP_UnaryExp;
class UnaryExpAST_Ident;
class MulExpAST;
class MulOPAST;
class MulExpAST_UnaryExp;
class MulExpAST_MulExp_MulOP_UnaryExp;
class AddExpAST;
class AddOPAST;
class AddExpAST_MulExp;
class AddExpAST_AddExp_AddOP_MulExp;
class RelExpAST;
class RelOPAST;
class RelExpAST_AddExp;
class RelExpAST_RelExp_RelOP_AddExp;
class EqExpAST;
class EqOPAST;
class EqExpAST_RelExp;
class EqExpAST_EqExp_EqOP_RelExp;
class LAndExpAST;
class LAndExp_EqExp;
class LAndExpAST_LAndExp_AND_EqExp;
class LOrExpAST;
class LOrExp_LAndExp;
class LOrExp_LOrExp_OR_LAndExp;
class DeclAST;
class DeclAST_ConstDecl;
class DeclAST_VarDecl;
class ConstDeclAST;
class ConstDeclBlockAST;
// class BTypeAST;
class ConstDefAST;
class ConstInitValAST;
class ConstInitValListAST;
class LValAST;
class LValAST_Array;
class ConstExpAST;
class ConstExpAST_Exp;
class VarDeclAST;
class VarDeclBlockAST;
class VarDefAST;
class InitValAST;
class InitValListAST;
class ArrayDefIndexListAST;
class ArrayAccessIndexListAST;

bool is_global();
void new_local_table();
void delete_local_table();
void add_function(string, bool);
Symbol &get_symbol(string);
string access_array(Symbol, std::unique_ptr<BaseAST>&, int);
extern int used_variable;

// 所有 AST 的基类
class BaseAST
{
public:
    virtual ~BaseAST() = default;

    virtual string to_string(int indent=0) {
        return " !to_string() of " + string(typeid(*this).name()) + string(" NOT IMPLEMENTED! ");
    }

    virtual string to_ir(int indent=0) {
        return " !to_ir() of " + string(typeid(*this).name()) + string(" NOT IMPLEMENTED! ");
    }

    virtual int value() {
        printf("ERROR: value() of %s not implemented\n", string(typeid(*this).name()).c_str());
        return 0;
    }

    virtual bool is_number() {
        return false;
    }
};

class LibDefAST : public BaseAST
{
public:
    LibDefAST() {}

    string to_ir(int indent=0) {
        string ret;
        ret += "decl @getint(): i32\n";
        ret += "decl @getch(): i32\n";
        ret += "decl @getarray(*i32): i32\n";
        ret += "decl @putint(i32)\n";
        ret += "decl @putch(i32)\n";
        ret += "decl @putarray(i32, *i32)\n";
        ret += "decl @starttime()\n";
        ret += "decl @stoptime()\n";

        add_function("getint", false);
        add_function("getch", false);
        add_function("getarray", false);
        add_function("putint", true);
        add_function("putch", true);
        add_function("putarray", true);
        add_function("starttime", true);
        add_function("stoptime", true);
        return ret;
    }
};

class RootAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> comp_unit;
    std::unique_ptr<BaseAST> lib_def;

    RootAST(BaseAST* comp_unit_c) : comp_unit(comp_unit_c), lib_def(new LibDefAST()) {}

    string to_string(int indent=0) {
        return comp_unit->to_string(indent);
    }

    string to_ir(int indent=0) {
        new_local_table();
        string ret;
        ret += lib_def->to_ir() + '\n';    // Include libraries
        ret += comp_unit->to_ir(indent);
        delete_local_table();
        return ret;
    }
};

class FuncTypeAST : public BaseAST
{
public:
    std::string type;

    FuncTypeAST(std::string type_c) : type(type_c) {}

    string to_string(int indent=0) {
        return type;
    }

    string to_ir(int indent=0) {
        return type;
    }
};

class NumberAST : public BaseAST
{
public:
    int val;

    NumberAST(int val) : val(val) {}
    NumberAST(string val_c) : val(std::atoi(val_c.c_str())) {}

    string to_string(int indent=0) {
        return std::to_string(val);
    }
    
    string to_ir(int indent=0) {
        string ret;
        ret += indent_space(indent) + "%" + std::to_string(++used_variable) + " = add 0, " + std::to_string(val) + "\n";
        return ret;
    }

    int value() {
        return val;
    }

    bool is_number() {
        return true;
    }
};

class StmtAST : public BaseAST
{
    string to_ir(int indent=0) {
        return string("");
    }
};

class StmtAST_Return : public StmtAST
{
public:
    std::unique_ptr<BaseAST> exp;

    StmtAST_Return(BaseAST* exp_c) : exp(exp_c) {}

    string to_string(int indent=0) {
        string ret;
        ret += indent_space(indent) + "return " + exp->to_string() + ";";
        return ret;
    }

    string to_ir(int indent=0) {
        string ret;
        if (exp->is_number()) {
            ret += indent_space(indent) + string("ret ") + std::to_string(exp->value()) + "\n";
            block_return++;
            return ret;
        }
        else {
            ret += exp->to_ir(indent);
            ret += indent_space(indent) + string("ret ") + "%" + std::to_string(used_variable) + "\n";
            block_return++;
            return ret;
        }
    }
};

class StmtAST_Return_Empty : public StmtAST
{
public:
    StmtAST_Return_Empty() {}

    string to_string(int indent=0) {
        string ret;
        ret += indent_space(indent) + "return ;";
        return ret;
    }

    string to_ir(int indent=0) {
        string ret;
        ret += indent_space(indent) + string("ret ") + "0" + "\n";
        block_return++;
        return ret;
    }
};

class StmtAST_Exp : public StmtAST
{
public:
    std::unique_ptr<BaseAST> exp;

    StmtAST_Exp(BaseAST* exp_c) : exp(exp_c) {}

    string to_string(int indent=0) {
        string ret;
        ret += indent_space(indent) + exp->to_string() + ";";
        return ret;
    }

    string to_ir(int indent=0) {
        string ret;
        ret += exp->to_ir(indent);
        return ret;
    }
};

class StmtAST_If : public StmtAST
{
public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> stmt1, stmt2;
    bool has_else = false;

    StmtAST_If(BaseAST* exp_c, BaseAST* stmt1_c, BaseAST* stmt2_c) : exp(exp_c), stmt1(stmt1_c), stmt2(stmt2_c) {has_else = true;}
    StmtAST_If(BaseAST* exp_c, BaseAST* stmt1_c) : exp(exp_c), stmt1(stmt1_c) {has_else = false;}

    string to_string(int indent=0) {
        string ret;
        ret += indent_space(indent) + "if (" + exp->to_string() + ")\n";
        ret += stmt1->to_string(indent+1);
        if (stmt2) {
            ret += indent_space(indent) + "else\n";
            ret += stmt2->to_string(indent+1);
        }
        return ret;
    }

    string to_ir(int indent=0) {
        string ret;
        int if_label = if_cnt++;
        ret += exp->to_ir(indent);
        block_return = 0;
        ret += indent_space(indent) + string("br ") + "%" + std::to_string(used_variable) + ", %then_" + std::to_string(if_label) + ", %else_" + std::to_string(if_label) + "\n";
        ret += string("\n%then_") + std::to_string(if_label) + ":\n";
        ret += stmt1->to_ir(indent+1);
        if (!block_return) {
            ret += indent_space(indent+1) + string("jump %end_") + std::to_string(if_label) + "\n";
        }
        else block_return = 0;
        ret += string("\n%else_") + std::to_string(if_label) + ":\n";
        if (has_else) {
            ret += stmt2->to_ir(indent+1);
        }
        if (!block_return) {
            ret += indent_space(indent+1) + string("jump %end_") + std::to_string(if_label) + "\n";
        }
        else block_return = 0;
        ret += string("\n%end_") + std::to_string(if_label) + ":\n";
        return ret;
    }
};

class StmtAST_Block : public StmtAST
{
public:
    std::unique_ptr<BaseAST> block;

    StmtAST_Block(BaseAST* block_c) : block(block_c) {}

    string to_string(int indent=0) {
        string ret;
        ret += block->to_string(indent);
        return ret;
    }

    string to_ir(int indent=0) {
        string ret;
        ret += block->to_ir(indent);
        return ret;
    }
};

class BlockAST : public BaseAST
{
public:
    std::vector<std::unique_ptr<BaseAST> > stmt_list;
    SymbolTable symbol_table;

    BlockAST() {}
    BlockAST(BaseAST* stmt_c) {stmt_list.push_back(std::unique_ptr<BaseAST>(stmt_c));}

    void push_back(BlockBlockAST*);

    string to_string(int indent=0) {
        string ret;
        ret += "{\n";
        for(auto const &stmt : stmt_list)
            ret += stmt->to_string(indent+1) + "\n";
        ret += indent_space(indent) + "}";
        return ret;
    }

    string to_ir(int indent=0) {
        new_local_table();
        string ret;
        for(auto const &stmt : stmt_list) {
            if(block_return) break;
            ret += stmt->to_ir(indent) + "\n";
        }
        delete_local_table();
        return ret;
    }
};

class BlockBlockAST : public BaseAST
{
public:
    std::vector<std::unique_ptr<BaseAST> > block_list;

    BlockBlockAST() {}
    BlockBlockAST(BaseAST* block_c) {block_list.push_back(std::unique_ptr<BaseAST>(block_c));}

    void push_back(BlockBlockAST*);

    string to_string(int indent=0) {
        string ret;
        for(auto const &block : block_list)
            ret += block->to_string(indent);
        return ret;
    }

    string to_ir(int indent=0) {
        string ret;
        for(auto const &block : block_list)
            ret += block->to_ir(indent);
        return ret;
    }
};

class BlockItemAST : public BaseAST
{

};

class BlockItemAST_Decl : public BlockItemAST
{
public:
    std::unique_ptr<BaseAST> decl;

    BlockItemAST_Decl(BaseAST* decl_c) : decl(decl_c) {}

    string to_string(int indent=0) {
        return decl->to_string();
    }

    string to_ir(int indent=0) {
        return decl->to_ir(indent);
    }
};

class BlockItemAST_Stmt : public BlockItemAST
{
public:
    std::unique_ptr<BaseAST> stmt;

    BlockItemAST_Stmt(BaseAST* stmt_c) : stmt(stmt_c) {}

    string to_string(int indent=0) {
        return stmt->to_string();
    }

    string to_ir(int indent=0) {
        return stmt->to_ir(indent);
    }
};

class ArrayAccessIndexListAST : public BaseAST
{
public:
    std::vector<std::unique_ptr<BaseAST> > index_list;
    std::vector<int> saved_index_list;

    ArrayAccessIndexListAST() : index_list() {}

    ArrayAccessIndexListAST(BaseAST* index_c) : index_list() {
        index_list.push_back(std::unique_ptr<BaseAST>(index_c));
    }

    void push_back(ArrayAccessIndexListAST*);

    string prepare_index(int indent=0) {
        string ret;
        for (auto& index : index_list) {
            if (!index->is_number()) {
                ret += index->to_ir(indent);
            }
            saved_index_list.push_back(used_variable);
            // ret += indent_space(indent) + "%" + std::to_string(++used_variable) + " = load %" + std::to_string(saved_index_list.back()) + "\n";
        }
        return ret;
    }
};

class ArrayDefIndexListAST : public BaseAST
{
public:
    std::vector<std::unique_ptr<BaseAST> > index_list;

    ArrayDefIndexListAST() : index_list() {}

    ArrayDefIndexListAST(BaseAST* index_c) : index_list() {
        index_list.push_back(std::unique_ptr<BaseAST>(index_c));
    }

    void push_back(ArrayDefIndexListAST*);

    int total_length() {
        int total = 1;
        for (auto& index : index_list) {
            total *= index->value();
        }
        return total;
    }

    string to_string(int indent=0) {
        string str="i32";
        for (auto index=index_list.rbegin(); index!=index_list.rend(); ++index) {
            str = "[" + str + ", " + std::to_string((*index)->value()) + "]";
        }
        return str;
    }

    std::unique_ptr<BaseAST> access_by_1d(int id) {
        std::unique_ptr<ArrayAccessIndexListAST> ret(new ArrayAccessIndexListAST());
        int total = index_list.size();
        for (int i=0; i<total; i++) {
            ret->index_list.push_back(std::unique_ptr<BaseAST>(new NumberAST(id%index_list[total-i-1]->value())));
            id /= index_list[total-i-1]->value();
        }
        std::reverse(ret->index_list.begin(), ret->index_list.end());
        return std::move(ret);
    }
};

class FuncFParamAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> btype;
    std::string name;
    bool is_array;

    FuncFParamAST(BaseAST* btype_c, std::string* name_c)
        : btype(btype_c), name(*name_c), is_array(false) {}
    
    string to_string(int indent=0) {
        string ret;
        ret += "%" + name + ": i32";
        return ret;
    }

    string to_ir(int indent=0) {
        string str;
        Symbol symbol = get_symbol(name);
        str = indent_space(indent) + string("@") + symbol.name + " = alloc i32\n";
        str += indent_space(indent) + string("store %") + name + ", " + string("@") + symbol.name + '\n';
        return str;
    }
};

class FuncFParamAST_Array : public FuncFParamAST
{
public:
    std::unique_ptr<BaseAST> shape;

    FuncFParamAST_Array(BaseAST* btype_c, std::string* name_c)
        : FuncFParamAST(btype_c, name_c), shape(new ArrayDefIndexListAST()) {is_array=true;}

    FuncFParamAST_Array(BaseAST* btype_c, std::string* name_c, BaseAST* shape_c)
        : FuncFParamAST(btype_c, name_c), shape(shape_c) {is_array=true;}
    
    string to_string(int indent=0) {
        string ret;
        ret += "%" + name + ": *" + shape->to_string();
        return ret;
    }
    
    string to_ir(int indent=0) {
        string str;
        Symbol symbol = get_symbol(name);
        str = indent_space(indent) + string("@") + symbol.name + " = alloc *" + shape->to_string() + '\n';
        str += indent_space(indent) + string("store %") + name + ", " + string("@") + symbol.name + '\n';
        return str;
    }
};

class FuncFParamsAST : public BaseAST
{
public:
    std::vector<std::unique_ptr<BaseAST> > fparam_list;

    FuncFParamsAST(BaseAST* func_param_c) {fparam_list.push_back(std::unique_ptr<BaseAST>(func_param_c));}

    void push_back(FuncFParamsAST*);

    void update_symbol_table();

    string to_ir(int indent)
    {
        update_symbol_table();
        string ret;
        for (auto& param_def : fparam_list) {
            ret += param_def->to_ir(indent);
        }
        return ret;
    }
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> func_params;
    std::unique_ptr<BaseAST> block;
    bool have_params;

    FuncDefAST(BaseAST* func_type_c, std::string* ident_c, BaseAST* block_c)
        : func_type(func_type_c), ident(*ident_c), block(block_c), have_params(0) {}
    
    FuncDefAST(BaseAST* func_type_c, std::string* ident_c, BaseAST* func_params_c, BaseAST* block_c)
        : func_type(func_type_c), ident(*ident_c), func_params(func_params_c), block(block_c), have_params(1) {}

    string to_string(int indent=0) {
        return func_type->to_string() + " " + ident + "()" + block->to_string();
    }

    void update_symbol_table();

    string to_ir(int indent=0) {
        update_symbol_table();
        new_local_table();
        auto symbol = get_symbol(ident);
        string ret;
        block_return = 0;
        ret += indent_space(indent) + string("fun ") + string(" @") + symbol.name + "(";
        FuncFParamsAST *derived = dynamic_cast<FuncFParamsAST*>(func_params.get());
        if (have_params) {
            for (auto const &fparam : derived->fparam_list) {
                FuncFParamAST *derived2 = dynamic_cast<FuncFParamAST*>(fparam.get());
                ret += derived2->to_string() + ", ";
                // ret += "%" + derived2->name + ": i32, ";
            }
            ret.pop_back();
            ret.pop_back();
        }
        ret += "): i32 {\n";
        if (symbol.name == "main") {
            ret += indent_space(indent) + "%entry: \n";
        }
        else {
            ret += indent_space(indent) + "%entry_" + std::to_string(++if_cnt) + ": \n";
        }
        if (have_params) ret += derived->to_ir(indent+1) + '\n';
        ret += block->to_ir(indent+1);
        delete_local_table();
        if (func_type->to_string() == "void" || !block_return) {
            ret += indent_space(indent+1) + "ret 0\n";
        }
        ret += indent_space(indent) + "}\n";
        return ret;
    }
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST
{
public:
    // 用智能指针管理对象
    std::vector<std::unique_ptr<BaseAST> > func_def;

    CompUnitAST(BaseAST* func_def_c) {func_def.push_back(std::unique_ptr<BaseAST>(func_def_c));}
    CompUnitAST() {}

    void push_back(CompUnitAST*);

    string to_string(int indent=0) {
        string ret;
        for(auto const &func_def_block : func_def)
            ret += func_def_block->to_string(indent) + "\n";
        return ret;
    }

    string to_ir(int indent=0) {
        // printf("ASD\n");
        string ret = "";
        for(auto const &func_def_block : func_def)
            ret += func_def_block->to_ir(indent);
        // printf("ASD\n");
        return ret;
    }
};

class ExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> lor_exp;

    ExpAST(BaseAST* lor_exp_c) : lor_exp(lor_exp_c) {}

    string to_string(int indent=0) {
        return lor_exp->to_string();
    }

    string to_ir(int indent=0) {
        return lor_exp->to_ir(indent);
    }

    int value() {
        return lor_exp->value();
    }

    bool is_number() {
        return lor_exp->is_number();
    }
};

class PrimaryExpAST : public BaseAST
{
public:
    virtual int value() {
        return 0;
    }
};

class PrimaryExpAST_Exp : public PrimaryExpAST
{
public:
    std::unique_ptr<BaseAST> exp;

    PrimaryExpAST_Exp(BaseAST* exp_c) : exp(exp_c) {}

    string to_string(int indent=0) {
        return string("(") + exp->to_string() + string(")");
    }

    string to_ir(int indent=0) {
        return exp->to_ir(indent);
    }

    int value() {
        return exp->value();
    }

    bool is_number() {
        return exp->is_number();
    }
};

class PrimaryExpAST_Number : public PrimaryExpAST
{
public:
    std::unique_ptr<BaseAST> number;

    PrimaryExpAST_Number(BaseAST* number_c) : number(number_c) {}

    string to_string(int indent=0) {
        return number->to_string();
    }

    string to_ir(int indent=0) {
        string ret;
        ret += indent_space(indent) + string("%") + std::to_string(++used_variable) + string(" = add 0, ") + std::to_string(number->value()) + "\n";
        return ret;
    }

    int value() {
        return number->value();
    }

    bool is_number() {
        return true;
    }
};

class UnaryExpAST : public BaseAST
{
public:
    int value() {
        return 0;
    }
};

class UnaryOPAST : public BaseAST
{
public:
    char c;

    UnaryOPAST(char c_c) : c(c_c) {}

    string to_string(int indent=0) {
        return string(1, c);
    }

    string to_ir(int indent=0) {
        return string(1, c);
    }
};

class UnaryExpAST_PrimaryExp : public UnaryExpAST
{
public:
    std::unique_ptr<BaseAST> primary_exp;

    UnaryExpAST_PrimaryExp(BaseAST* primary_exp_c) : primary_exp(primary_exp_c) {}

    string to_string(int indent=0) {
        return primary_exp->to_string();
    }

    string to_ir(int indent=0) {
        return primary_exp->to_ir(indent);
    }

    int value() {
        return primary_exp->value();
    }

    bool is_number() {
        return primary_exp->is_number();
    }
};

class UnaryExpAST_UnaryOP_UnaryExp : public UnaryExpAST
{
public:
    std::unique_ptr<BaseAST> unary_op, unary_exp;

    UnaryExpAST_UnaryOP_UnaryExp(BaseAST* unary_op_c, BaseAST* unary_exp_c) : unary_op(unary_op_c), unary_exp(unary_exp_c) {}

    string to_string(int indent=0) {
        return unary_op->to_string() + unary_exp->to_string();
    }

    string to_ir(int indent=0) {
        string str;
        UnaryOPAST *derived = dynamic_cast<UnaryOPAST*>(unary_op.get());
        if(derived->c == '-')
        {
            str += unary_exp->to_ir(indent);
            auto result = used_variable;
            str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = sub 0, %" + std::to_string(result) + "\n";
        }
        else if(derived->c == '+')
        {
            str += unary_exp->to_ir(indent);
        }
        else if(derived->c == '!')
        {
            str += unary_exp->to_ir(indent);
            auto result = used_variable;
            str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = ne 0, %" + std::to_string(result) + "\n";
            result = used_variable;
            str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = xor 1, %" + std::to_string(result) + "\n";
        }
        else
        {
            assert(false);
        }
        return str;
    }

    int value() {
        UnaryOPAST *derived = dynamic_cast<UnaryOPAST*>(unary_op.get());
        if(derived->c == '-')
        {
            return -unary_exp->value();
        }
        else if(derived->c == '+')
        {
            return unary_exp->value();
        }
        else if(derived->c == '!')
        {
            return !unary_exp->value();
        }
        else
        {
            assert(false);
        }
    }

    bool is_number() {
        return unary_exp->is_number();
    }
};

class FuncRParamsAST : public BaseAST
{
public:
    vector<std::unique_ptr<BaseAST> > rparam_list;
    vector<int> saved_variable;

    FuncRParamsAST(BaseAST* rparam_c) {rparam_list.push_back(std::unique_ptr<BaseAST>(rparam_c));}

    void push_back(FuncRParamsAST*);

    string to_ir(int indent=0) {
        string str;
        for (auto const &i : rparam_list)
        {
            if (!i->is_number()) {
                str += i->to_ir(indent);
            }
            saved_variable.push_back(used_variable);
        }
        return str;
    }

    string to_param_list() {
        string str;
        for (int i=0; i<saved_variable.size(); i++)
        {
            if (rparam_list[i]->is_number()) {
                str += std::to_string(rparam_list[i]->value()) + string(", ");
            }
            else {
                str += string("%") + std::to_string(saved_variable[i]) + string(", ");
            }
        }
        str.pop_back();
        str.pop_back();
        return str;
    }
};

class UnaryExpAST_Ident : public UnaryExpAST
{
public:
    string ident;
    std::unique_ptr<BaseAST> func_rparams;
    bool have_params;

    UnaryExpAST_Ident(string* ident_c) : ident(*ident_c), have_params(0) {}
    UnaryExpAST_Ident(string* ident_c, BaseAST* func_rparams_c) : ident(*ident_c), func_rparams(func_rparams_c), have_params(1) {}

    string to_string(int indent=0) {
        if(func_rparams)
            return ident + string("(") + func_rparams->to_string() + string(")");
        else
            return ident;
    }

    string to_ir(int indent=0) {
        string str;
        auto symbol = get_symbol(ident);
        assert(!symbol.func_symbol);
        // Prepare parameters
        printf("Preparing parameters\n");
        if (have_params) {
            str += func_rparams->to_ir(indent);
        }
        // Call function
        string in_bracket;
        if (have_params) {
            printf("getting rparams\n");
            auto rparams_ast = dynamic_cast<FuncRParamsAST*>(func_rparams.get());
            in_bracket = rparams_ast->to_param_list();
        }
        if (symbol.void_symbol) {
            str += indent_space(indent) + "call @" + symbol.name + "(" + in_bracket + ")\n";
        }
        else {
            str += indent_space(indent) + "%" + std::to_string(++used_variable) + " = call @" + symbol.name + "(" + in_bracket + ")\n";
        }
        return str;
    }
};

class MulExpAST : public BaseAST
{
public:
    int value() {
        return 0;
    }
};

class MulOPAST : public BaseAST
{
public:
    char c;

    MulOPAST(char c_c) : c(c_c) {}

    string to_string(int indent=0) {
        return string(1, c);
    }

    string to_ir(int indent=0) {
        return string(1, c);
    }
};

class MulExpAST_UnaryExp : public MulExpAST
{
public:
    std::unique_ptr<BaseAST> unary_exp;

    MulExpAST_UnaryExp(BaseAST* unary_exp_c) : unary_exp(unary_exp_c) {}

    string to_string(int indent=0) {
        return unary_exp->to_string(indent);
    }

    string to_ir(int indent=0) {
        return unary_exp->to_ir(indent);
    }

    int value() {
        return unary_exp->value();
    }

    bool is_number() {
        return unary_exp->is_number();
    }
};

class MulExpAST_MulExp_MulOP_UnaryExp : public MulExpAST
{
public:
    std::unique_ptr<BaseAST> mul_exp, mul_op, unary_exp;

    MulExpAST_MulExp_MulOP_UnaryExp(BaseAST* mul_exp_c, BaseAST* mul_op_c, BaseAST* unary_exp_c) : mul_exp(mul_exp_c), mul_op(mul_op_c), unary_exp(unary_exp_c) {}

    string to_string(int indent=0) {
        return mul_exp->to_string(indent) + mul_op->to_string() + unary_exp->to_string();
    }

    string to_ir(int indent=0) {
        string str;
        string result1;
        string result2;
        if (mul_exp->is_number()) {
            result1 = std::to_string(mul_exp->value());
        }
        else {
            str += mul_exp->to_ir(indent);
            result1 = "%" + std::to_string(used_variable);
        }
        if (unary_exp->is_number()) {
            result2 = std::to_string(unary_exp->value());
        }
        else {
            str += unary_exp->to_ir(indent);
            result2 = "%" + std::to_string(used_variable);
        }
        str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = ";
        MulOPAST *derived = dynamic_cast<MulOPAST*>(mul_op.get());
        if(derived->c == '*') {
            str += "mul " + result1 + ", " + result2 + "\n";
        }
        else if(derived->c == '/') {
            str += "div " + result1 + ", " + result2 + "\n";
        }
        else if(derived->c == '%') {
            str += "mod " + result1 + ", " + result2 + "\n";
        }
        else {
            assert(false);
        }
        return str;
    }

    int value() {
        MulOPAST *derived = dynamic_cast<MulOPAST*>(mul_op.get());
        if(derived->c == '*') {
            return mul_exp->value() * unary_exp->value();
        }
        else if(derived->c == '/') {
            return mul_exp->value() / unary_exp->value();
        }
        else if(derived->c == '%') {
            return mul_exp->value() % unary_exp->value();
        }
        else {
            assert(false);
        }
    }

    bool is_number() {
        return mul_exp->is_number() && unary_exp->is_number();
    }
};

class AddExpAST : public BaseAST
{
public:
    int value() {
        return 0;
    }
};

class AddOPAST : public BaseAST
{
public:
    char c;

    AddOPAST(char c_c) : c(c_c) {}

    string to_string(int indent=0) {
        return string(1, c);
    }

    string to_ir(int indent=0) {
        return string(1, c);
    }
};

class AddExpAST_MulExp : public AddExpAST
{
public:
    std::unique_ptr<BaseAST> mul_exp;

    AddExpAST_MulExp(BaseAST* mul_exp_c) : mul_exp(mul_exp_c) {}

    string to_string(int indent=0) {
        return mul_exp->to_string(indent);
    }

    string to_ir(int indent=0) {
        return mul_exp->to_ir(indent);
    }

    int value() {
        return mul_exp->value();
    }

    bool is_number() {
        return mul_exp->is_number();
    }
};

class AddExpAST_AddExp_AddOP_MulExp : public AddExpAST
{
public:
    std::unique_ptr<BaseAST> add_exp, add_op, mul_exp;

    AddExpAST_AddExp_AddOP_MulExp(BaseAST* add_exp_c, BaseAST* add_op_c, BaseAST* mul_exp_c) : add_exp(add_exp_c), add_op(add_op_c), mul_exp(mul_exp_c) {}

    string to_string(int indent=0) {
        return add_exp->to_string(indent) + add_op->to_string() + mul_exp->to_string();
    }

    string to_ir(int indent=0) {
        string str;
        string result1;
        string result2;
        if (add_exp->is_number()) {
            result1 = std::to_string(add_exp->value());
        }
        else {
            str += add_exp->to_ir(indent);
            result1 = "%" + std::to_string(used_variable);
        }
        if (mul_exp->is_number()) {
            result2 = std::to_string(mul_exp->value());
        }
        else {
            str += mul_exp->to_ir(indent);
            result2 = "%" + std::to_string(used_variable);
        }
        str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = ";
        AddOPAST *derived = dynamic_cast<AddOPAST*>(add_op.get());
        if(derived->c == '+') {
            str += "add " + result1 + ", " + result2 + "\n";
        }
        else if(derived->c == '-') {
            str += "sub " + result1 + ", " + result2 + "\n";
        }
        else {
            assert(false);
        }
        return str;
    }

    int value() {
        AddOPAST *derived = dynamic_cast<AddOPAST*>(add_op.get());
        if(derived->c == '+') {
            return add_exp->value() + mul_exp->value();
        }
        else if(derived->c == '-') {
            return add_exp->value() - mul_exp->value();
        }
        else {
            assert(false);
        }
    }

    bool is_number() {
        return add_exp->is_number() && mul_exp->is_number();
    }
};

class RelExpAST : public BaseAST
{
public:
    virtual int value() {
        return 0;
    }
};

class RelOPAST : public BaseAST
{
public:
    string c;

    RelOPAST(string c_c) : c(c_c) {}

    string to_string(int indent=0) {
        return c;
    }

    string to_ir(int indent=0) {
        return c;
    }
};

class RelExpAST_AddExp : public RelExpAST
{
public:
    std::unique_ptr<BaseAST> add_exp;

    RelExpAST_AddExp(BaseAST* add_exp_c) : add_exp(add_exp_c) {}

    string to_string(int indent=0) {
        return add_exp->to_string(indent);
    }

    string to_ir(int indent=0) {
        return add_exp->to_ir(indent);
    }

    int value() {
        return add_exp->value();
    }

    bool is_number() {
        return add_exp->is_number();
    }
};

class RelExpAST_RelExp_RelOP_AddExp : public RelExpAST
{
public:
    std::unique_ptr<BaseAST> rel_exp, rel_op, add_exp;

    RelExpAST_RelExp_RelOP_AddExp(BaseAST* rel_exp_c, BaseAST* rel_op_c, BaseAST* add_exp_c) : rel_exp(rel_exp_c), rel_op(rel_op_c), add_exp(add_exp_c) {}

    string to_string(int indent=0) {
        return rel_exp->to_string(indent) + rel_op->to_string() + add_exp->to_string();
    }

    string to_ir(int indent=0) {
        string str;
        string result1;
        string result2;
        if (rel_exp->is_number()) {
            result1 = std::to_string(rel_exp->value());
        }
        else {
            str += rel_exp->to_ir(indent);
            result1 = "%" + std::to_string(used_variable);
        }
        if (add_exp->is_number()) {
            result2 = std::to_string(add_exp->value());
        }
        else {
            str += add_exp->to_ir(indent);
            result2 = "%" + std::to_string(used_variable);
        }
        str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = ";
        RelOPAST *derived = dynamic_cast<RelOPAST*>(rel_op.get());
        if( derived->c == string("<")) {
            str += "lt " + result1 + ", " + result2 + "\n";
        }
        else if(derived->c == string(">")) {
            str += "gt " + result1 + ", " + result2 + "\n";
        }
        else if(derived->c == string("<=")) {
            str += "le " + result1 + ", " + result2 + "\n";
        }
        else if(derived->c == string(">=")) {
            str += "ge " + result1 + ", " + result2 + "\n";
        }
        else {
            assert(false);
        }
        return str;
    }

    int value() {
        RelOPAST *derived = dynamic_cast<RelOPAST*>(rel_op.get());
        if( derived->c == string("<")) {
            return rel_exp->value() < add_exp->value();
        }
        else if(derived->c == string(">")) {
            return rel_exp->value() > add_exp->value();
        }
        else if(derived->c == string("<=")) {
            return rel_exp->value() <= add_exp->value();
        }
        else if(derived->c == string(">=")) {
            return rel_exp->value() >= add_exp->value();
        }
        else {
            assert(false);
        }
    }

    bool is_number() {
        return rel_exp->is_number() && add_exp->is_number();
    }
};

class EqExpAST : public BaseAST
{
public:
    virtual int value() {
        return 0;
    }
};

class EqOPAST : public BaseAST
{
public:
    string c;

    EqOPAST(string c_c) : c(c_c) {}

    string to_string(int indent=0) {
        return c;
    }

    string to_ir(int indent=0) {
        return c;
    }
};

class EqExpAST_RelExp : public EqExpAST
{
public:
    std::unique_ptr<BaseAST> rel_exp;

    EqExpAST_RelExp(BaseAST* rel_exp_c) : rel_exp(rel_exp_c) {}

    string to_string(int indent=0) {
        return rel_exp->to_string(indent);
    }

    string to_ir(int indent=0) {
        return rel_exp->to_ir(indent);
    }

    int value() {
        return rel_exp->value();
    }

    bool is_number() {
        return rel_exp->is_number();
    }
};

class EqExpAST_EqExp_EqOP_RelExp : public EqExpAST
{
public:
    std::unique_ptr<BaseAST> eq_exp, eq_op, rel_exp;

    EqExpAST_EqExp_EqOP_RelExp(BaseAST* eq_exp_c, BaseAST* eq_op_c, BaseAST* rel_exp_c) : eq_exp(eq_exp_c), eq_op(eq_op_c), rel_exp(rel_exp_c) {}

    string to_string(int indent=0) {
        return eq_exp->to_string(indent) + eq_op->to_string() + rel_exp->to_string();
    }

    string to_ir(int indent=0) {
        string str;
        string result1;
        string result2;
        if (eq_exp->is_number()) {
            result1 = std::to_string(eq_exp->value());
        }
        else {
            str += eq_exp->to_ir(indent);
            result1 = "%" + std::to_string(used_variable);
        }
        if (rel_exp->is_number()) {
            result2 = std::to_string(rel_exp->value());
        }
        else {
            str += rel_exp->to_ir(indent);
            result2 = "%" + std::to_string(used_variable);
        }
        str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = ";
        EqOPAST *derived = dynamic_cast<EqOPAST*>(eq_op.get());
        if(derived->c == string("==")) {
            str += "eq " + result1 + ", " + result2 + "\n";
        }
        else if(derived->c == string("!=")) {
            str += "ne " + result1 + ", " + result2 + "\n";
        }
        else {
            assert(false);
        }
        return str;
    }

    int value() {
        EqOPAST *derived = dynamic_cast<EqOPAST*>(eq_op.get());
        if( derived->c == string("==")) {
            return eq_exp->value() == rel_exp->value();
        }
        else if(derived->c == string("!=")) {
            return eq_exp->value() != rel_exp->value();
        }
        else {
            assert(false);
        }
    }

    bool is_number() {
        return eq_exp->is_number() && rel_exp->is_number();
    }
};

class LAndExpAST : public BaseAST
{
public:
    virtual int value() {
        return 0;
    }
};

class LAndExpAST_EqExp : public LAndExpAST
{
public:
    std::unique_ptr<BaseAST> eq_exp;

    LAndExpAST_EqExp(BaseAST* eq_exp_c) : eq_exp(eq_exp_c) {}

    string to_string(int indent=0) {
        return eq_exp->to_string(indent);
    }

    string to_ir(int indent=0) {
        return eq_exp->to_ir(indent);
    }

    int value() {
        return eq_exp->value();
    }

    bool is_number() {
        return eq_exp->is_number();
    }
};

class LAndExpAST_LAndExp_AND_EqExp : public LAndExpAST
{
public:
    std::unique_ptr<BaseAST> l_and_exp, eq_exp;

    LAndExpAST_LAndExp_AND_EqExp(BaseAST* l_and_exp_c, BaseAST* eq_exp_c) : l_and_exp(l_and_exp_c), eq_exp(eq_exp_c) {}

    string to_string(int indent=0) {
        return l_and_exp->to_string() + "&&" + eq_exp->to_string();
    }

    string to_ir(int indent=0) {
        // string str;
        // str += l_and_exp->to_ir(indent);
        // auto result = used_variable;
        // str += eq_exp->to_ir(indent);
        // auto result2 = used_variable;
        // str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = ";
        // str += "ne 0, %" + std::to_string(result) + "\n";
        // auto result3 = used_variable;
        // str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = ";
        // str += "ne 0, %" + std::to_string(result2) + "\n";
        // auto result4 = used_variable;
        // str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = ";
        // str += "and %" + std::to_string(result3) + ", %" + std::to_string(result4) + "\n";
        // return str;

        string str;
        int cur_if = if_cnt++;
        str += l_and_exp->to_ir(indent);
        auto result = used_variable;
        auto save_result = ++used_variable;
        str += indent_space(indent) + "@store_" + std::to_string(save_result) + " = alloc i32\n";
        str += indent_space(indent) + "br %" + std::to_string(result) + ", %other_" + std::to_string(cur_if) + ", %skip_" + std::to_string(cur_if) + "\n";
        
        str += "\n%other_" + std::to_string(cur_if) + ":\n";
        str += eq_exp->to_ir(indent);
        auto result2 = used_variable;
        str += indent_space(indent) + "store %" + std::to_string(result2) + ", @store_" + std::to_string(save_result) + "\n";
        str += indent_space(indent) + "jump %end_" + std::to_string(cur_if) + "\n";

        str += "\n%skip_" + std::to_string(cur_if) + ":\n";
        str += indent_space(indent) + "store %" + std::to_string(result) + ", @store_" + std::to_string(save_result) + "\n";
        str += indent_space(indent) + "jump %end_" + std::to_string(cur_if) + "\n";

        str += "\n%end_" + std::to_string(cur_if) + ":\n";
        str += indent_space(indent) + "%" + std::to_string(++used_variable) + " = load @store_" + std::to_string(save_result) + "\n";
        auto result3 = used_variable;
        str += indent_space(indent) + "%" + std::to_string(++used_variable) + " = ne 0, %" + std::to_string(result3) + "\n";
        return str;
    }

    int value() {
        return l_and_exp->value() && eq_exp->value();
    }

    bool is_number() {
        return l_and_exp->is_number() && eq_exp->is_number();
    }
};

class LOrExpAST : public BaseAST
{
public:
    virtual int value() {
        return 0;
    }
};

class LOrExpAST_LAndExp : public LOrExpAST
{
public:
    std::unique_ptr<LAndExpAST> l_and_exp;

    LOrExpAST_LAndExp(LAndExpAST* l_and_exp_c) : l_and_exp(l_and_exp_c) {}

    string to_string(int indent=0) {
        return l_and_exp->to_string(indent);
    }

    string to_ir(int indent=0) {
        return l_and_exp->to_ir(indent);
    }

    int value() {
        return l_and_exp->value();
    }

    bool is_number() {
        return l_and_exp->is_number();
    }
};

class LOrExpAST_LOrExp_OR_LAndExp : public LOrExpAST
{
public:
    std::unique_ptr<LOrExpAST> l_or_exp;
    std::unique_ptr<LAndExpAST> l_and_exp;

    LOrExpAST_LOrExp_OR_LAndExp(LOrExpAST* l_or_exp_c, LAndExpAST* l_and_exp_c) : l_or_exp(l_or_exp_c), l_and_exp(l_and_exp_c) {}

    string to_string(int indent=0) {
        return l_or_exp->to_string(indent) + "||" + l_and_exp->to_string();
    }

    string to_ir(int indent=0) {
        string str;
        int cur_if = if_cnt++;
        str += l_or_exp->to_ir(indent);
        auto result = used_variable;
        auto save_result = ++used_variable;
        str += indent_space(indent) + "@store_" + std::to_string(save_result) + " = alloc i32\n";
        str += indent_space(indent) + "br %" + std::to_string(result) + ", %skip_" + std::to_string(cur_if) + ", %other_" + std::to_string(cur_if) + "\n";
        
        str += "%other_" + std::to_string(cur_if) + ":\n";
        str += l_and_exp->to_ir(indent);
        auto result2 = used_variable;
        str += indent_space(indent) + "store %" + std::to_string(result2) + ", @store_" + std::to_string(save_result) + "\n";
        str += indent_space(indent) + "jump %end_" + std::to_string(cur_if) + "\n";

        str += "%skip_" + std::to_string(cur_if) + ":\n";
        str += indent_space(indent) + "store %" + std::to_string(result) + ", @store_" + std::to_string(save_result) + "\n";
        str += indent_space(indent) + "jump %end_" + std::to_string(cur_if) + "\n";

        str += "%end_" + std::to_string(cur_if) + ":\n";
        str += indent_space(indent) + "%" + std::to_string(++used_variable) + " = load @store_" + std::to_string(save_result) + "\n";
        auto result3 = used_variable;
        str += indent_space(indent) + "%" + std::to_string(++used_variable) + " = ne 0, %" + std::to_string(result3) + "\n";
        return str;
    }

    int value() {
        return l_or_exp->value() || l_and_exp->value();
    }

    bool is_number() {
        return l_or_exp->is_number() && l_and_exp->is_number();
    }
};

class DeclAST : public BaseAST
{

};

class DeclAST_ConstDecl : public DeclAST
{
public:
    std::unique_ptr<BaseAST> const_decl;

    DeclAST_ConstDecl(BaseAST* const_decl_c) : const_decl(const_decl_c) {}

    string to_string(int indent=0) {
        return const_decl->to_string(indent);
    }

    string to_ir(int indent=0) {
        return const_decl->to_ir(indent);
    }
};

class DeclAST_VarDecl : public DeclAST
{
public:
    std::unique_ptr<BaseAST> var_decl;

    DeclAST_VarDecl(BaseAST* var_decl_c) : var_decl(var_decl_c) {}

    string to_string(int indent=0) {
        return var_decl->to_string(indent);
    }

    string to_ir(int indent=0) {
        return var_decl->to_ir(indent);
    }
};

class ConstDeclBlockAST : public BaseAST
{
public:
    std::vector<std::unique_ptr<BaseAST> > name;

    ConstDeclBlockAST() : name() {}

    ConstDeclBlockAST(BaseAST* name_c) {
        name.push_back(std::unique_ptr<BaseAST>(name_c));
    }

    void push_back(ConstDeclBlockAST*);
};

class ConstDeclAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> btype;
    std::vector<std::unique_ptr<BaseAST> > const_def_list;

    ConstDeclAST(BaseAST* btype_c, BaseAST* const_def_c) : btype(btype_c) {
        const_def_list.push_back(std::unique_ptr<BaseAST>(const_def_c));
    }

    void push_back(ConstDeclBlockAST*);

    void update_symbol_table();

    string to_string(int indent=0) {
        string str;
        str += btype->to_string(indent);
        for (auto& const_def : const_def_list) {
            str += const_def->to_string(indent);
        }
        return str;
    }

    string to_ir(int indent=0) {
        update_symbol_table();
        string str;
        for (auto& const_def : const_def_list) {
            str += const_def->to_ir(indent);
        }
        return str;
    }
};

// class BTypeAST : public BaseAST
// {
// public:
//     string type;

//     BTypeAST(string type_c) : type(type_c) {}

//     string to_string(int indent=0) {
//         return type;
//     }

//     string to_ir(int indent=0) {
//         return type;
//     }
// };

class ConstExpAST : public BaseAST
{
public:
    virtual int value() {
        return 0;
    }
};

class ConstExpAST_Exp : public ConstExpAST
{
public:
    std::unique_ptr<ExpAST> exp;

    ConstExpAST_Exp(ExpAST* exp_c) : exp(exp_c) {}

    string to_string(int indent=0) {
        return exp->to_string(indent);
    }

    string to_ir(int indent=0) {
        return exp->to_ir(indent);
    }

    int value() {
        return exp->value();
    }

    bool is_number() {
        return exp->is_number();
    }
};

class ConstInitValAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> const_exp;

    ConstInitValAST(BaseAST* const_exp_c) : const_exp(const_exp_c) {}

    int value() {
        return const_exp->value();
    }

    bool is_number() {
        return const_exp->is_number();
    }
};

class ConstInitValListAST : public BaseAST
{
public:
    std::vector<std::unique_ptr<BaseAST> > const_init_val_list;
    bool is_element = false;

    ConstInitValListAST() : const_init_val_list() {}

    ConstInitValListAST(BaseAST* const_init_val_c) : is_element(false) {
        const_init_val_list.push_back(std::unique_ptr<BaseAST>(const_init_val_c));
    }

    ConstInitValListAST(BaseAST* const_init_val_c, bool is_element_c) : is_element(is_element_c) {
        const_init_val_list.push_back(std::unique_ptr<BaseAST>(const_init_val_c));
    }

    void push_back(ConstInitValListAST*);

    void regularize(std::vector<std::unique_ptr<BaseAST> >::iterator index_list_begin, std::vector<std::unique_ptr<BaseAST> >::iterator index_list_end) {
        std::vector<std::unique_ptr<BaseAST> > new_const_init_val_list;

        for (auto &const_init_val : const_init_val_list) {
            auto derived_const_init_val = dynamic_cast<ConstInitValListAST*>(const_init_val.get());
            auto derived_init_val = dynamic_cast<ConstInitValListAST*>(const_init_val.get());
            if (derived_init_val->is_element) {
                new_const_init_val_list.push_back(std::move(derived_const_init_val->const_init_val_list[0]));
            }
            else {
                // ArrayDefIndexListAST* remain_shape = new ArrayDefIndexListAST();
                int now_size = new_const_init_val_list.size();
                int remain_shape_len = 0;
                auto mid = index_list_end;
                while (now_size % (*(mid-1))->value() == 0 && (mid-1) != index_list_begin) {
                    now_size /= (*(mid-1))->value();
                    mid--;
                    remain_shape_len++;
                }
                assert(remain_shape_len > 0);
                derived_init_val->regularize(mid, index_list_end);
                std::move(derived_init_val->const_init_val_list.begin(), derived_init_val->const_init_val_list.end(), std::back_inserter(new_const_init_val_list));
            }
        }
        // for (auto &x : new_const_init_val_list) {
        //     printf("%d\n", x->value());
        // }
        int target_size = 1;
        for (auto ptr = index_list_begin; ptr != index_list_end; ptr++) {
            target_size *= (*ptr)->value();
        }
        assert(new_const_init_val_list.size() <= target_size);
        for (int i=new_const_init_val_list.size(); i<target_size; i++) {
            new_const_init_val_list.push_back(std::unique_ptr<BaseAST>(new NumberAST(0)));
        }
        const_init_val_list = std::move(new_const_init_val_list);
    }

    string _to_initval_list_recursive(vector<std::unique_ptr<BaseAST> >::iterator l, std::unique_ptr<BaseAST>& shape, int dim, int& used_size) {
        string str = "{";
        auto shape_exp = dynamic_cast<ArrayDefIndexListAST*>(shape.get());
        if (dim == shape_exp->index_list.size()-1) {
            for (int i=0; i<shape_exp->index_list[dim]->value(); i++) {
                str += (*(l+used_size+i))->to_string();
                if (i != shape_exp->index_list[dim]->value()-1) {
                    str += ", ";
                }
            }
            used_size += shape_exp->index_list[dim]->value();
            str += "}";
            return str;
        }
        else {
            int my_used_size = 0;
            for (int i=0; i<shape_exp->index_list[dim]->value(); i++) {
                str += _to_initval_list_recursive(l+used_size, shape, dim+1, my_used_size);
                if (i != shape_exp->index_list[dim]->value()-1) {
                    str += ", ";
                }
            }
            used_size += my_used_size;
            str += "}";
            return str;
        }
    }

    string to_initval_list(std::unique_ptr<BaseAST> &shape) {
        int my_used_size = 0;
        return _to_initval_list_recursive(const_init_val_list.begin(), shape, 0, my_used_size);
    }

    // string to_initval_list(std::unique_ptr<BaseAST> &shape) {
    //     string str = "{";
    //     ArrayDefIndexListAST* const_exp = dynamic_cast<ArrayDefIndexListAST*>(shape.get());
    //     for (int i=0; i<const_exp->total_length(); i++) {
    //         if (i<const_init_val_list.size()) {
    //             str += const_init_val_list[i]->to_string() + ", ";
    //         }
    //         else {
    //             str += "0, ";
    //         }
    //     }
    //     str.pop_back();
    //     str.pop_back();
    //     str += "}";
    //     return str;
    // }
};

class ConstDefAST : public BaseAST
{
public:
    string name;
    std::unique_ptr<BaseAST> const_init_val;
    bool is_array;

    ConstDefAST(std::string* name_c, BaseAST* const_init_val_c) : name(*name_c), const_init_val(const_init_val_c) {is_array=false;}

    string to_ir(int indent=0) {
        Symbol symbol = get_symbol(name);
        string str;
        if (is_global()) {
            str += "global @" + symbol.name + " = alloc i32, " + std::to_string(const_init_val->value()) + "\n";
        }
        else {
            str += indent_space(indent) + string("@") + symbol.name + " = add 0, ";
            str += indent_space(indent) + std::to_string(const_init_val->value()) + "\n";
        }
        return str;
    }

    int value() {
        return const_init_val->value();
    }
};

class ConstDefAST_Array : public ConstDefAST
{
public:
    std::unique_ptr<BaseAST> shape;

    ConstDefAST_Array(std::string* name_c, BaseAST* shape_c, BaseAST* const_init_val_c) : ConstDefAST(name_c, const_init_val_c), shape(shape_c) {is_array=true;}

    string to_ir(int indent=0) {
        Symbol symbol = get_symbol(name);
        string str;
        auto derived_const_init_val = dynamic_cast<ConstInitValListAST*>(const_init_val.get());
        auto index_list_begin = dynamic_cast<ArrayDefIndexListAST*>(shape.get())->index_list.begin();
        auto index_list_end = dynamic_cast<ArrayDefIndexListAST*>(shape.get())->index_list.end();
        auto const_shape_exp = dynamic_cast<ArrayDefIndexListAST*>(shape.get());
        derived_const_init_val->regularize(index_list_begin, index_list_end);
        if (is_global()) {
            str += indent_space(indent) + "global @" + symbol.name + " = alloc " + shape->to_string() + ", ";
            str += derived_const_init_val->to_initval_list(shape) + "\n";
        }
        else {
            str += indent_space(indent) + "@" + symbol.name + " = alloc " + shape->to_string()+ "\n";
            for (int i=0; i<const_shape_exp->total_length(); i++) {
                // index = ArrayAccessIndexListAST(i);
                // std::unique_ptr<BaseAST> index(new ExpAST(new NumberAST(i)));
                auto index = const_shape_exp->access_by_1d(i);
                str += access_array(symbol.name, index, indent);
                str += indent_space(indent) + "store " + std::to_string(derived_const_init_val->const_init_val_list[i]->value()) + ", %" + std::to_string(used_variable) + "\n";
            }
            // for (int i=0; i<derived_const_init_val->const_init_val_list.size(); i++) {
            //     str += indent_space(indent) + "store " + derived_const_init_val->const_init_val_list[i]->to_string() + ", ";
            //     str += indent_space(indent) + "@" + symbol.name + ", " + std::to_string(i) + "\n";
            // }
            // str += indent_space(indent) + "store " + derived_const_init_val->to_initval_list(shape) + ", @" + symbol.name + "\n";
        }
        return str;
    }
};

class LValAST : public BaseAST
{
public:
    string name;

    LValAST(string* name_c) : name(*name_c) {}

    string to_ir(int indent=0) {
        string str;
        auto sym = get_symbol(name);
        if (sym.const_symbol) {
            str = indent_space(indent) + string("%") + std::to_string(++used_variable) + " = add 0, " + std::to_string(sym.value) + '\n';
        }
        else if (sym.array_symbol && !sym.pointer_symbol) {
            str = indent_space(indent) + string("%") + std::to_string(++used_variable) + " = getelemptr @"  + sym.name + ", 0\n";
        }
        else if (sym.array_symbol && sym.pointer_symbol) {
            str = indent_space(indent) + string("%") + std::to_string(++used_variable) + " = load @" + sym.name + '\n';
            int last_variable = used_variable;
            str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = getptr %" + std::to_string(last_variable) + ", 0\n";
        }
        else {
            str = indent_space(indent) + string("%") + std::to_string(++used_variable) + " = load @" + sym.name + '\n';
        }
        return str;
    }

    int value() {
        return get_symbol(name).value;
    }
};

class LValAST_Array : public LValAST
{
public:
    std::unique_ptr<BaseAST> index;
    LValAST_Array(string* name_c, BaseAST* shape_c) : LValAST(name_c), index(shape_c) {}

    string to_ir(int indent=0) {
        string str;
        auto derived_index = dynamic_cast<ArrayAccessIndexListAST*>(index.get());
        auto sym = get_symbol(name);

        str += access_array(sym, index, indent);
        int last_variable = used_variable;
        if (!sym.array_symbol) {
            str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = load " + string("%") + std::to_string(last_variable) + '\n';
        }
        else if (sym.array_symbol && derived_index->index_list.size() == sym.dim) {
            str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = load " + string("%") + std::to_string(last_variable) + '\n';
        }
        else {
            str += indent_space(indent) + string("%") + std::to_string(++used_variable) + " = getelemptr " + string("%") + std::to_string(last_variable) + ", 0\n";
        }
        return str;
    }
};

class StmtAST_LVal_Exp : public StmtAST
{
public:
    std::unique_ptr<BaseAST> lval;
    std::unique_ptr<BaseAST> exp;

    StmtAST_LVal_Exp(BaseAST* lval_c, BaseAST* exp_c) : lval(lval_c), exp(exp_c) {}

    string to_ir(int indent=0) {
        string result, ret;
        if (exp->is_number()) {
            result = std::to_string(exp->value());
        }
        else {
            ret = exp->to_ir(indent);
            result = "%" + std::to_string(used_variable);
        }
        LValAST *derived = dynamic_cast<LValAST*>(lval.get());
        Symbol symbol = get_symbol(derived->name);
        ret += indent_space(indent) + string("store ") + result + ", @" + symbol.name + '\n';
        return ret;
    }
};

class StmtAST_LValArray_Exp : public StmtAST
{
public:
    std::unique_ptr<BaseAST> lval;
    std::unique_ptr<BaseAST> exp;

    StmtAST_LValArray_Exp(BaseAST* lval_c, BaseAST* exp_c) : lval(lval_c), exp(exp_c) {}

    string to_ir(int indent=0) {
        string result, ret;
        if (exp->is_number()) {
            result = std::to_string(exp->value());
        }
        else {
            ret = exp->to_ir(indent);
            result = "%" + std::to_string(used_variable);
        }
        LValAST_Array *derived = dynamic_cast<LValAST_Array*>(lval.get());
        Symbol symbol = get_symbol(derived->name);
        ret += access_array(symbol, derived->index, indent);
        ret += indent_space(indent) + string("store ") + result + ", %" + std::to_string(used_variable) + '\n';
        return ret;
    }
};

class StmtAST_While : public StmtAST
{
public:
    std::unique_ptr<BaseAST> cond;
    std::unique_ptr<BaseAST> stmt;

    StmtAST_While(BaseAST* cond_c, BaseAST* stmt_c) : cond(cond_c), stmt(stmt_c) {}

    string to_ir(int indent=0) {
        string ret;
        int while_cond_id = ++used_variable;
        while_stack.push_back(while_cond_id);
        // ret += indent_space(indent) + "@while_cond_" + std::to_string(while_cond_id) + " = alloc i32\n";
        ret += indent_space(indent) + "jump %while_entry_" + std::to_string(while_cond_id) + "\n";

        ret += "\n%while_entry_" + std::to_string(while_cond_id) + ":\n";
        ret += cond->to_ir(indent);
        // ret += indent_space(indent) + "store %" + std::to_string(used_variable) + ", @while_cond_" + std::to_string(while_cond_id) + "\n";
        // ret += indent_space(indent) + "%" + std::to_string(++used_variable) + " = load @while_cond_" + std::to_string(while_cond_id) + "\n";
        ret += indent_space(indent) + "br %" + std::to_string(used_variable) + ", %while_body_" + std::to_string(while_cond_id) + ", %while_end_" + std::to_string(while_cond_id) + "\n";

        ret += "\n%while_body_" + std::to_string(while_cond_id) + ":\n";
        ret += stmt->to_ir(indent + 1);
        if (!block_return){
            ret += indent_space(indent) + "jump %while_entry_" + std::to_string(while_cond_id) + "\n";
        }
        else block_return = 0;

        ret += "\n%while_end_" + std::to_string(while_cond_id) + ":\n";

        while_stack.pop_back();

        return ret;
    }
};

class StmtAST_Break : public StmtAST
{
public:
    string to_ir(int indent=0) {
        string ret;
        int while_cond_id = while_stack.back();
        ret += indent_space(indent) + "jump %while_end_" + std::to_string(while_cond_id) + "\n";
        block_return++;
        return ret;
    }
};

class StmtAST_Continue : public StmtAST
{
public:
    string to_ir(int indent=0) {
        string ret;
        int while_cond_id = while_stack.back();
        ret += indent_space(indent) + "jump %while_entry_" + std::to_string(while_cond_id) + "\n";
        block_return++;
        return ret;
    }
};

class PrimaryExpAST_LVal : public PrimaryExpAST
{
public:
    std::unique_ptr<BaseAST> lval;

    PrimaryExpAST_LVal(BaseAST* lval_c) : lval(lval_c) {}

    string to_string(int indent=0) {
        return lval->to_string(indent);
    }

    string to_ir(int indent=0) {
        return lval->to_ir(indent);
    }

    int value() {
        return lval->value();
    }

    bool is_number() {
        return lval->is_number();
    }
};

class PrimaryExpAST_LValArray : public PrimaryExpAST
{
public:
    std::unique_ptr<BaseAST> lval;

    PrimaryExpAST_LValArray(BaseAST* lval_c) : lval(lval_c) {}

    string to_string(int indent=0) {
        return lval->to_string(indent);
    }

    string to_ir(int indent=0) {
        return lval->to_ir(indent);
    }

    int value() {
        return lval->value();
    }
};

class VarDeclBlockAST : public BaseAST
{
public:
    std::vector<std::unique_ptr<BaseAST> > name;

    VarDeclBlockAST() : name() {}

    VarDeclBlockAST(BaseAST* name_c) {
        name.push_back(std::unique_ptr<BaseAST>(name_c));
    }

    void push_back(VarDeclBlockAST*);
};

class VarDeclAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> btype;
    std::vector<std::unique_ptr<BaseAST> > var_def_list;

    VarDeclAST(BaseAST* btype_c, BaseAST* var_def_c) : btype(btype_c) {
        var_def_list.push_back(std::unique_ptr<BaseAST>(var_def_c));
    }

    void push_back(VarDeclBlockAST*);

    void update_symbol_table();

    string to_string(int indent=0) {
        string str;
        str += btype->to_string(indent);
        for (auto& var_def : var_def_list) {
            str += var_def->to_string(indent);
        }
        return str;
    }

    string to_ir(int indent=0) {
        update_symbol_table();
        string str;
        for (auto& var_def : var_def_list) {
            str += var_def->to_ir(indent);
        }
        return str;
    }
};

class InitValAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> exp;

    InitValAST(BaseAST* exp_c) : exp(exp_c) {}

    string to_ir(int indent=0) {
        return exp->to_ir(indent);
    }

    string to_string(int indent=0) {
        return exp->to_string(indent);
    }

    int value () {
        return exp->value();
    }

    bool is_number() {
        return exp->is_number();
    }
};

// class InitValListAST : public BaseAST
// {
// public:
//     std::vector<std::unique_ptr<BaseAST> > init_val_list;
//     vector<int> saved_init_val;

//     InitValListAST() : init_val_list() {}

//     InitValListAST(BaseAST* init_val_c) : init_val_list() {
//         init_val_list.push_back(std::unique_ptr<BaseAST>(init_val_c));
//     }

//     void push_back(InitValListAST*);

//     string prepare_initval_list(std::unique_ptr<BaseAST> &shape, int indent=0) {
//         string str;
//         ConstExpAST* const_exp = dynamic_cast<ConstExpAST*>(shape.get());
//         // for (auto& const_init_val : const_init_val_list) {
//         //     str += const_init_val->to_string(indent) + ", ";
//         // }
//         for (int i=0; i<const_exp->value(); i++) {
//             if (i<init_val_list.size()) {
//                 str += init_val_list[i]->to_ir(indent);
//             }
//             else {
//                 str += NumberAST(0).to_ir(indent);
//             }
//             saved_init_val.push_back(used_variable);
//         }
//         return str;
//     }

//     string to_initval_list(std::unique_ptr<BaseAST> &shape) {
//         string str = "{";
//         ConstExpAST* const_exp = dynamic_cast<ConstExpAST*>(shape.get());
//         // for (auto& const_init_val : const_init_val_list) {
//         //     str += const_init_val->to_string(indent) + ", ";
//         // }
//         for (int i=0; i<const_exp->value(); i++) {
//             if (i<init_val_list.size()) {
//                 str += init_val_list[i]->to_string() + ", ";
//             }
//             else {
//                 str += "0, ";
//             }
//         }
//         str.pop_back();
//         str.pop_back();
//         str += "}";
//         return str;
//     }
// };

class InitValListAST : public BaseAST
{
public:
    std::vector<std::unique_ptr<BaseAST> > init_val_list;
    vector<int> saved_init_val;
    bool is_element = false;

    InitValListAST() : init_val_list() {}

    InitValListAST(BaseAST* init_val_c) : is_element(false) {
        init_val_list.push_back(std::unique_ptr<BaseAST>(init_val_c));
    }

    InitValListAST(BaseAST* init_val_c, bool is_element_c) : is_element(is_element_c) {
        init_val_list.push_back(std::unique_ptr<BaseAST>(init_val_c));
    }

    void push_back(InitValListAST*);

    void regularize(std::vector<std::unique_ptr<BaseAST> >::iterator index_list_begin, std::vector<std::unique_ptr<BaseAST> >::iterator index_list_end) {
        std::vector<std::unique_ptr<BaseAST> > new_init_val_list;

        for (auto &init_val : init_val_list) {
            auto derived_init_val = dynamic_cast<InitValListAST*>(init_val.get());
            if (derived_init_val->is_element) {
                new_init_val_list.push_back(std::move(derived_init_val->init_val_list[0]));
            }
            else {
                // ArrayDefIndexListAST* remain_shape = new ArrayDefIndexListAST();
                int now_size = new_init_val_list.size();
                int remain_shape_len = 0;
                auto mid = index_list_end;
                while (now_size % (*(mid-1))->value() == 0 && (mid-1) != index_list_begin) {
                    now_size /= (*(mid-1))->value();
                    mid--;
                    remain_shape_len++;
                }
                assert(remain_shape_len > 0);
                derived_init_val->regularize(mid, index_list_end);
                std::move(derived_init_val->init_val_list.begin(), derived_init_val->init_val_list.end(), std::back_inserter(new_init_val_list));
            }
        }
        // for (auto &x : new_const_init_val_list) {
        //     printf("%d\n", x->value());
        // }
        int target_size = 1;
        for (auto ptr = index_list_begin; ptr != index_list_end; ptr++) {
            target_size *= (*ptr)->value();
        }
        assert(new_init_val_list.size() <= target_size);
        for (int i=new_init_val_list.size(); i<target_size; i++) {
            new_init_val_list.push_back(std::unique_ptr<BaseAST>(new NumberAST(0)));
        }
        init_val_list = std::move(new_init_val_list);
    }

    string _to_initval_list_recursive(vector<std::unique_ptr<BaseAST> >::iterator l, std::unique_ptr<BaseAST>& shape, int dim, int& used_size) {
        string str = "{";
        auto shape_exp = dynamic_cast<ArrayDefIndexListAST*>(shape.get());
        if (dim == shape_exp->index_list.size()-1) {
            for (int i=0; i<shape_exp->index_list[dim]->value(); i++) {
                str += std::to_string((*(l+used_size+i))->value());
                if (i != shape_exp->index_list[dim]->value()-1) {
                    str += ", ";
                }
            }
            used_size += shape_exp->index_list[dim]->value();
            str += "}";
            return str;
        }
        else {
            int my_used_size = 0;
            for (int i=0; i<shape_exp->index_list[dim]->value(); i++) {
                str += _to_initval_list_recursive(l+used_size, shape, dim+1, my_used_size);
                if (i != shape_exp->index_list[dim]->value()-1) {
                    str += ", ";
                }
            }
            used_size += my_used_size;
            str += "}";
            return str;
        }
    }
    string to_initval_list(std::unique_ptr<BaseAST> &shape) {
        int my_used_size = 0;
        return _to_initval_list_recursive(init_val_list.begin(), shape, 0, my_used_size);
    }

    string prepare_initval_list(std::unique_ptr<BaseAST> &shape, int indent) {
        string str;
        ArrayDefIndexListAST* const_exp = dynamic_cast<ArrayDefIndexListAST*>(shape.get());
        for (int i=0; i<const_exp->total_length(); i++) {
            if (!init_val_list[i]->is_number()) {
                str += init_val_list[i]->to_ir(indent);
            }
            saved_init_val.push_back(used_variable);
        }
        return str;
    }
};

class VarDefAST : public BaseAST
{
public:
    string name;
    std::unique_ptr<BaseAST> init_val;
    bool is_array;

    VarDefAST(std::string* name_c) : name(*name_c), init_val(
        new InitValAST(
            new ExpAST(
                new PrimaryExpAST_Number(
                    new NumberAST(0)
                )
            )
        )
    ) {is_array=false;}
    VarDefAST(std::string* name_c, BaseAST* init_val_c) : name(*name_c), init_val(init_val_c) {}

    string to_ir(int indent=0) {
        string str;
        Symbol &symbol = get_symbol(name);
        if (is_global()) {
            auto value = init_val->value();
            symbol.value = value;
            str = "global " + string("@") + symbol.name + " = alloc i32, " + std::to_string(value) + "\n";
        }
        else {
            str = indent_space(indent) + string("@") + symbol.name + " = alloc i32\n";
            string result;
            if (init_val->is_number()) {
                result = std::to_string(init_val->value());
            }
            else {
                str += init_val->to_ir(indent);
                result = string("%") + std::to_string(used_variable);
            }
            str += indent_space(indent) + string("store ") + result + ", " + string("@") + symbol.name + '\n';
        }
        return str;
    }
};

class VarDefAST_Array : public VarDefAST
{
public:
    std::unique_ptr<BaseAST> shape;
    bool have_init_val;

    VarDefAST_Array(std::string* name_c, BaseAST* shape_c) : VarDefAST(name_c), shape(shape_c), have_init_val(0) {is_array=true;}

    VarDefAST_Array(std::string* name_c, BaseAST* shape_c, BaseAST* init_val_c) : VarDefAST(name_c, init_val_c), shape(shape_c), have_init_val(1) {is_array=true;}

    string to_ir(int indent=0) {
        string str;
        Symbol &symbol = get_symbol(name);
        ArrayDefIndexListAST* const_shape_exp = dynamic_cast<ArrayDefIndexListAST*>(shape.get());
        InitValListAST *derived_init_val = dynamic_cast<InitValListAST*>(init_val.get());
        auto index_list_begin = dynamic_cast<ArrayDefIndexListAST*>(shape.get())->index_list.begin();
        auto index_list_end = dynamic_cast<ArrayDefIndexListAST*>(shape.get())->index_list.end();
        if (have_init_val) {
            derived_init_val->regularize(index_list_begin, index_list_end);
        }
        if (is_global()) {
            str += indent_space(indent) + "global @" + symbol.name + " = alloc " + shape->to_string() + ", ";
            if (have_init_val) {
                str += derived_init_val->to_initval_list(shape) + "\n";
            }
            else {
                str += "zeroinit\n";
            }
        }
        else {
            if (have_init_val) {
                str += derived_init_val->prepare_initval_list(shape, indent);
            }
            str += indent_space(indent) + "@" + symbol.name + " = alloc " + shape->to_string() + "\n";
            if (have_init_val) {
                for (int i=0; i<const_shape_exp->total_length(); i++) {
                    // std::unique_ptr<BaseAST> index(new ExpAST(new NumberAST(i)));
                    auto index = const_shape_exp->access_by_1d(i);
                    str += access_array(symbol.name, index, indent);
                    // printf("ACCESS %d :", i);
                    // ArrayAccessIndexListAST *derived = dynamic_cast<ArrayAccessIndexListAST*>(index.get());
                    // for (int i=0; i<derived->index_list.size(); i++)
                    // {printf("%d ", derived->saved_index_list[i]);}
                    // printf("\n");
                    if (derived_init_val->init_val_list[i]->is_number()) {
                        str += indent_space(indent) + "store " + std::to_string(derived_init_val->init_val_list[i]->value()) + ", %" + std::to_string(used_variable) + "\n";
                    }
                    else {
                        str += indent_space(indent) + "store %" + std::to_string(derived_init_val->saved_init_val[i]) + ", %" + std::to_string(used_variable) + "\n";
                    }
                }
            }
        }
        return str;
    }
};

#endif