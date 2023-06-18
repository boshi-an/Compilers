#include <assert.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <cstring>

#include "ast.h"
#include "raw_program.h"

using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段
extern FILE *yyin, *yyout;
extern int yyparse(unique_ptr<RootAST> &ast);

int main(int argc, const char *argv[]) {
  // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
  // compiler 模式 输入文件 -o 输出文件
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
  yyin = fopen(input, "r");
  assert(yyin);

  yyout = fopen(output, "w");
  assert(yyout);

  // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
  unique_ptr<RootAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  if(strcmp(mode, "-ast") == 0) {
    // 如果是 ast 模式, 那么直接输出 AST
    cout << ast->to_string() << endl;
  }
  else if(strcmp(mode, "-koopa") == 0) {
    // 如果是 ir 模式, 那么输出 IR
    printf("output koopa IR\n");
    fprintf(yyout, "%s", ast->to_ir().c_str());
    // cout << ast->to_ir() << endl;
  }
  else if(strcmp(mode, "-riscv") == 0) {
    // 如果是 riscv 模式, 那么输出 RISC-V 汇编
    auto temp_out = fopen((string(output)+".out").c_str(), "w");
    assert(temp_out);
    printf("Generating IR:\n");
    auto ir = ast->to_ir();
    fprintf(temp_out, "%s", ir.c_str());
    fflush(temp_out);
    fclose(temp_out);
    
    printf("Output RISCV:\n");
    fprintf(yyout, "%s", ir_to_raw_program(ir).c_str());
  }
  else if(strcmp(mode, "-perf") == 0) {
    auto ir = ast->to_ir();
    printf("Output RISCV:\n");
    fprintf(yyout, "%s", ir_to_raw_program(ir).c_str());
  }
  else {
    // 其他模式都是错误的
    assert(false);
  }
  return 0;
}
