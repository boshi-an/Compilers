#ifndef __AST_H__
#define __AST_H__
#include <memory>
#include <string>
#include <iostream>
#define INDENT_SIZE 4

using namespace std;

string indent_space(int);

// 所有 AST 的基类
class BaseAST
{
public:
    virtual ~BaseAST() = default;

    virtual string to_string(int indent=0) {
        return string(typeid(*this).name()) + string(" NOT IMPLEMENTED");
    }

    virtual string to_ir(int indent=0) {
        return string(typeid(*this).name()) + string(" NOT IMPLEMENTED");
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
    int value;

    NumberAST(int value) : value(value) {}
    NumberAST(string value) : value(std::atoi(value.c_str())) {}

    string to_string(int indent=0) {
        return std::to_string(value);
    }
    
    string to_ir(int indent=0) {
        return std::to_string(value);
    }
};

class StmtAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> number;

    StmtAST(BaseAST* number_c) : number(number_c) {}

    string to_string(int indent=0) {
        string ret;
        ret += indent_space(indent) + "return " + number->to_string() + ";";
        return ret;
    }

    string to_ir(int indent=0) {
        string ret;
        ret += indent_space(indent) + string("ret ") + number->to_ir() + '\n';
        return ret;
    }
};

class BlockAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> stmt;

    BlockAST(BaseAST* stmt_c) : stmt(stmt_c) {}

    string to_string(int indent=0) {
        string ret;
        ret += "{\n";
        ret += stmt->to_string(indent+1) + "\n";
        ret += indent_space(indent) + "}";
        return ret;
    }

    string to_ir(int indent=0) {
        string ret;
        ret += "{\n";
        ret += indent_space(indent) + string("%entry: ") + "\n";
        ret += stmt->to_ir(indent+1) + "\n";
        ret += indent_space(indent) + "}";
        return ret;
    }
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    FuncDefAST(BaseAST* func_type_c, std::string* ident_c, BaseAST* block_c)
        : func_type(func_type_c), ident(*ident_c), block(block_c) {}

    string to_string(int indent=0) {
        return func_type->to_string() + " " + ident + "()" + block->to_string();
    }

    string to_ir(int indent=0) {
        string ret;
        ret += indent_space(indent) + string("fun ") + string(" @") + ident + "(): i32 " + block->to_ir(indent) + '\n';
        return ret;
    }
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST
{
public:
    // 用智能指针管理对象
    std::unique_ptr<BaseAST> func_def;

    CompUnitAST(BaseAST* func_def_c) : func_def(func_def_c) {}

    string to_string(int indent=0) {
        return func_def->to_string(indent);
    }

    string to_ir(int indent=0) {
        return func_def->to_ir(indent);
    }
};

#endif