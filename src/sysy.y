%code requires {
  #include <memory>
  #include <string>
  #include <memory>
  #include <cstring>
  #include "ast.h"
  #include "koopa.h"
}

%{

#include <iostream>
#include <memory>
#include <string>
#include "ast.h"

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<RootAST> &ast, const char *s);

using namespace std;

%}

// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<RootAST> &ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 至于为什么要用字符串指针而不直接用 string 或者 unique_ptr<string>?
// 请自行 STFW 在 union 里写一个带析构函数的类会出现什么情况
%union {
  // Basic statements
  int int_val;
  std::string *str_val;
  RootAST *Root_val;
  CompUnitAST *CompUnit_val;
  FuncDefAST *FuncDef_val;
  FuncTypeAST *FuncType_val;
  FuncFParamAST *FuncFParam_val;
  FuncFParamsAST *FuncFParams_val;
  FuncFParamAST_Array *FuncFParam_Array_val;
  FuncRParamsAST *FuncRParams_val;
  BlockAST *Block_val;
  BlockBlockAST *BlockBlock_val;
  StmtAST *Stmt_val;
  StmtAST_Return *Stmt_Return_val;
  StmtAST_Return_Empty *Stmt_Return_Empty_val;
  StmtAST_Exp *Stmt_Exp_val;
  StmtAST_If *Stmt_If_val;
  StmtAST_Block *Stmt_Block_val;
  StmtAST_LVal_Exp *Stmt_LVal_Exp_val;
  StmtAST_LValArray_Exp *Stmt_LValArray_Exp_val;
  StmtAST_While *Stmt_While_val;
  StmtAST_Break *Stmt_Break_val;
  StmtAST_Continue *Stmt_Continue_val;
  NumberAST *Number_val;

  // For expression use
  ExpAST *Exp_val;
  PrimaryExpAST *PrimaryExp_val;
  PrimaryExpAST_Exp *PrimaryExp_Exp_val;
  PrimaryExpAST_Number *PrimaryExp_Number_val;
  UnaryExpAST *UnaryExp_val;
  UnaryExpAST_PrimaryExp *UnaryExp_PrimaryExp_val;
  UnaryExpAST_UnaryOP_UnaryExp *UnaryExp_UnaryOP_UnaryExp_val;
  UnaryOPAST *UnaryOP_val;
  UnaryExpAST_Ident *UnaryExp_Ident_val;
  MulExpAST *MulExp_val;
  MulExpAST_UnaryExp *MulExp_UnaryExp_val;
  MulExpAST_MulExp_MulOP_UnaryExp *MulExp_MulExp_MulOP_UnaryExp_val;
  MulOPAST *MulOP_val;
  AddExpAST *AddExp_val;
  AddExpAST_MulExp *AddExp_MulExp_val;
  AddExpAST_AddExp_AddOP_MulExp *AddExp_AddExp_AddOP_MulExp_val;
  AddOPAST *AddOP_val;
  RelExpAST *RelExp_val;
  RelOPAST *RelOP_val;
  RelExpAST_AddExp *RelExp_AddExp_val;
  RelExpAST_RelExp_RelOP_AddExp *RelExp_RelExp_RelOP_AddExp_val;
  EqExpAST *EqExp_val;
  EqOPAST *EqOP_val;
  EqExpAST_RelExp *EqExp_RelExp_val;
  EqExpAST_EqExp_EqOP_RelExp *EqExp_EqExp_EqOP_RelExp_val;
  LAndExpAST *LAndExp_val;
  LAndExpAST_EqExp *LAndExp_EqExp_val;
  LAndExpAST_LAndExp_AND_EqExp *LAndExp_LAndExp_AND_EqExp_val;
  LOrExpAST *LOrExp_val;
  LOrExpAST_LAndExp *LOrExp_LAndExp_val;
  LOrExpAST_LOrExp_OR_LAndExp *LOrExp_LOrExp_OR_LAndExp_val;
  
  // For declaration
  BlockItemAST *BlockItem_val;
  BlockItemAST_Decl *BlockItem_Decl_val;
  DeclAST *Decl_val;
  DeclAST_ConstDecl *Decl_ConstDecl_val;
  DeclAST_VarDecl *Decl_VarDecl_val;
  ConstDeclAST *ConstDecl_val;
  ConstDeclBlockAST *ConstDeclBlock_val;
  // BTypeAST *BType_val;
  ConstDefAST *ConstDef_val;
  ConstInitValAST *ConstInitVal_val;
  ConstInitValListAST *ConstInitValList_val;
  LValAST *LVal_val;
  LValAST_Array *LVal_Array_val;
  ConstExpAST *ConstExp_val;
  ConstExpAST_Exp *ConstExp_Exp_val;
  PrimaryExpAST_LVal *PrimaryExp_LVal_val;
  PrimaryExpAST_LValArray *PrimaryExp_LValArray_val;
  VarDeclAST *VarDecl_val;
  VarDeclBlockAST *VarDeclBlock_val;
  VarDefAST *VarDef_val;
  VarDefAST_Array *VarDef_Array_val;
  InitValAST *InitVal_val;
  InitValListAST *InitValList_val;
  ArrayDefIndexListAST *ArrayDefIndexList_val;
  ArrayAccessIndexListAST *ArrayAccessIndexList_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN EQ NEQ LEQ GEQ LAND LOR ASSIGN LT GT NOT BITAND BITOR CONST IF ELSE WHILE BREAK CONTINUE VOID
%token <str_val> IDENT
%token <int_val> INT_CONST

// 非终结符的类型定义
// %type <str_val> FuncType Block Stmt Number
// Basic statements
%type <Root_val> Root
%type <CompUnit_val> CompUnit
%type <FuncDef_val> FuncDef
%type <FuncType_val> FuncType
%type <FuncFParam_val> FuncFParam
%type <FuncFParams_val> FuncFParams
%type <FuncFParam_Array_val> FuncFParam_Array
%type <FuncRParams_val> FuncRParams
%type <Block_val> Block
%type <BlockBlock_val> BlockBlock
%type <Stmt_val> Stmt OpenStmt CloseStmt NonIfStmt
%type <Stmt_Return_val> Stmt_Return
%type <Stmt_Return_Empty_val> Stmt_Return_Empty
%type <Stmt_Exp_val> Stmt_Exp
%type <Stmt_If_val> Stmt_If
%type <Stmt_Block_val> Stmt_Block
%type <Stmt_LVal_Exp_val> Stmt_LVal_Exp
%type <Stmt_LValArray_Exp_val> Stmt_LValArray_Exp
%type <Stmt_While_val> Stmt_While
%type <Stmt_Break_val> Stmt_Break
%type <Stmt_Continue_val> Stmt_Continue
%type <Number_val> Number

// Expressions
%type <Exp_val> Exp
%type <PrimaryExp_val> PrimaryExp
%type <PrimaryExp_Exp_val> PrimaryExp_Exp
%type <PrimaryExp_Number_val> PrimaryExp_Number
%type <UnaryExp_val> UnaryExp
%type <UnaryExp_PrimaryExp_val> UnaryExp_PrimaryExp
%type <UnaryExp_UnaryOP_UnaryExp_val> UnaryExp_UnaryOP_UnaryExp
%type <UnaryOP_val> UnaryOP
%type <UnaryExp_Ident_val> UnaryExp_Ident
%type <MulExp_val> MulExp
%type <MulExp_UnaryExp_val> MulExp_UnaryExp
%type <MulExp_MulExp_MulOP_UnaryExp_val> MulExp_MulExp_MulOP_UnaryExp
%type <MulOP_val> MulOP
%type <AddExp_val> AddExp
%type <AddExp_MulExp_val> AddExp_MulExp
%type <AddExp_AddExp_AddOP_MulExp_val> AddExp_AddExp_AddOP_MulExp
%type <AddOP_val> AddOP
%type <RelExp_val> RelExp
%type <RelOP_val> RelOP
%type <RelExp_AddExp_val> RelExp_AddExp
%type <RelExp_RelExp_RelOP_AddExp_val> RelExp_RelExp_RelOP_AddExp
%type <EqExp_val> EqExp
%type <EqOP_val> EqOP
%type <EqExp_RelExp_val> EqExp_RelExp
%type <EqExp_EqExp_EqOP_RelExp_val> EqExp_EqExp_EqOP_RelExp
%type <LAndExp_val> LAndExp
%type <LAndExp_EqExp_val> LAndExp_EqExp
%type <LAndExp_LAndExp_AND_EqExp_val> LAndExp_LAndExp_AND_EqExp
%type <LOrExp_val> LOrExp
%type <LOrExp_LAndExp_val> LOrExp_LAndExp
%type <LOrExp_LOrExp_OR_LAndExp_val> LOrExp_LOrExp_OR_LAndExp

// For declaration
%type <BlockItem_val> BlockItem
%type <BlockItem_Decl_val> BlockItem_Decl
%type <Decl_val> Decl
%type <Decl_ConstDecl_val> Decl_ConstDecl
%type <Decl_VarDecl_val> Decl_VarDecl
%type <ConstDecl_val> ConstDecl
%type <ConstDeclBlock_val> ConstDeclBlock
// %type <BType_val> BType
%type <ConstDef_val> ConstDef
%type <ConstInitVal_val> ConstInitVal
%type <ConstInitValList_val> ConstInitValList ConstInitValListBlock
%type <LVal_val> LVal
%type <LVal_Array_val> LValArray
%type <ConstExp_val> ConstExp
%type <ConstExp_Exp_val> ConstExp_Exp
%type <PrimaryExp_LVal_val> PrimaryExp_LVal
%type <PrimaryExp_LValArray_val> PrimaryExp_LValArray
%type <VarDecl_val> VarDecl
%type <VarDeclBlock_val> VarDeclBlock
%type <VarDef_val> VarDef
%type <VarDef_Array_val> VarDef_Array
%type <InitVal_val> InitVal
%type <InitValList_val> InitValList InitValListBlock
%type <ArrayDefIndexList_val> ArrayDefIndexList
%type <ArrayAccessIndexList_val> ArrayAccessIndexList
%%

// 开始符, CompUnit ::= FuncDef, 大括号后声明了解析完成后 parser 要做的事情
// 之前我们定义了 FuncDef 会返回一个 str_val, 也就是字符串指针
// 而 parser 一旦解析完 CompUnit, 就说明所有的 token 都被解析了, 即解析结束了
// 此时我们应该把 FuncDef 返回的结果收集起来, 作为 AST 传给调用 parser 的函数
// $1 指代规则里第一个符号的返回值, 也就是 FuncDef 的返回值

Root
  : CompUnit {
    ast = unique_ptr<RootAST>(new RootAST($1));
  }

CompUnit
  : FuncDef CompUnit {
    $$ = new CompUnitAST($1);
    $$->push_back($2);
  } | Decl CompUnit {
    // printf("CompUnit: Decl CompUnit\n");
    $$ = new CompUnitAST($1);
    $$->push_back($2);
  } | /* Empty */ {
    $$ = new CompUnitAST();
  }
  ;

// FuncDef ::= FuncType IDENT '(' ')' Block;
// 我们这里可以直接写 '(' 和 ')', 因为之前在 lexer 里已经处理了单个字符的情况
// 解析完成后, 把这些符号的结果收集起来, 然后拼成一个新的字符串, 作为结果返回
// $$ 表示非终结符的返回值, 我们可以通过给这个符号赋值的方法来返回结果
// 你可能会问, FuncType, IDENT 之类的结果已经是字符串指针了
// 为什么还要用 unique_ptr 接住它们, 然后再解引用, 把它们拼成另一个字符串指针呢
// 因为所有的字符串指针都是我们 new 出来的, new 出来的内存一定要 delete
// 否则会发生内存泄漏, 而 unique_ptr 这种智能指针可以自动帮我们 delete
// 虽然此处你看不出用 unique_ptr 和手动 delete 的区别, 但当我们定义了 AST 之后
// 这种写法会省下很多内存管理的负担
FuncDef
  : FuncType IDENT '(' ')' Block {
    $$ = new FuncDefAST($1, $2, $5);
  } | FuncType IDENT '(' FuncFParams ')' Block {
    $$ = new FuncDefAST($1, $2, $4, $6);
  }
  ;

FuncFParams
  : FuncFParam {
    $$ = new FuncFParamsAST($1);
  } | FuncFParam ',' FuncFParams {
    $$ = new FuncFParamsAST($1);
    $$->push_back($3);
  }
  ;

FuncRParams
  : Exp {
    $$ = new FuncRParamsAST($1);
  } | Exp ',' FuncRParams {
    $$ = new FuncRParamsAST($1);
    $$->push_back($3);
  }
  ;

FuncFParam
  : FuncType IDENT {
    $$ = new FuncFParamAST($1, $2);
  } | FuncType IDENT '[' ']' {
    $$ = new FuncFParamAST_Array($1, $2);
  } | FuncType IDENT '[' ']' ArrayDefIndexList {
    $$ = new FuncFParamAST_Array($1, $2, $5);
  }
  ;

// 同上, 不再解释
FuncType
  : INT {
    $$ = new FuncTypeAST("int");
    // $$ = new string("int");
  } | VOID {
    $$ = new FuncTypeAST("void");
    // $$ = new string("void");
  }
  ;

Block
  : '{' BlockBlock '}' {
    $$ = new BlockAST();
    $$->push_back($2);
  }
  ;

BlockBlock
  : BlockItem BlockBlock {
    $$ = new BlockBlockAST($1);
    $$->push_back($2);
  } | {
    $$ = new BlockBlockAST();
  }

BlockItem
  : Decl {
    $$ = new BlockItemAST_Decl($1);
  } | Stmt {
    $$ = new BlockItemAST_Stmt($1);
  }
  ;

Stmt
  : OpenStmt {
    $$ = $1;
  } | CloseStmt {
    $$ = $1;
  }

OpenStmt
  : IF '(' Exp ')' Stmt {
    $$ = new StmtAST_If($3, $5);
  } | IF '(' Exp ')' CloseStmt ELSE OpenStmt {
    $$ = new StmtAST_If($3, $5, $7);
  }

CloseStmt
  : NonIfStmt {
    $$ = $1;
  } | IF '(' Exp ')' CloseStmt ELSE CloseStmt {
    $$ = new StmtAST_If($3, $5, $7);
  }

NonIfStmt
  : RETURN Exp ';' {
    $$ = new StmtAST_Return($2);
  } | RETURN ';' {
    $$ = new StmtAST_Return_Empty();
  } | Exp ';' {
    $$ = new StmtAST_Exp($1);
  } | ';' {
    $$ = new StmtAST();
  } | LVal ASSIGN Exp ';' {
    $$ = new StmtAST_LVal_Exp($1, $3);
  } | LValArray ASSIGN Exp ';' {
    $$ = new StmtAST_LValArray_Exp($1, $3);
  }| Block {
    $$ = new StmtAST_Block($1);
  } | WHILE '(' Exp ')' Stmt {
    $$ = new StmtAST_While($3, $5);
  } | BREAK ';' {
    $$ = new StmtAST_Break();
  } | CONTINUE ';' {
    $$ = new StmtAST_Continue();
  }
  ;

Number
  : INT_CONST {
    $$ = new NumberAST($1);
  }
  ;

Exp
  : LOrExp {
    $$ = new ExpAST($1);
  }
  ;

PrimaryExp
  : '(' Exp ')' {
    $$ = new PrimaryExpAST_Exp($2);
  } | Number {
    $$ = new PrimaryExpAST_Number($1);
  } | LVal {
    $$ = new PrimaryExpAST_LVal($1);
  } | LValArray {
    $$ = new PrimaryExpAST_LValArray($1);
  }
  ;

UnaryExp
  : PrimaryExp {
    $$ = new UnaryExpAST_PrimaryExp($1);
  } | UnaryOP UnaryExp {
    $$ = new UnaryExpAST_UnaryOP_UnaryExp($1, $2);
  } | IDENT '(' ')' {
    $$ = new UnaryExpAST_Ident($1);
  } | IDENT '(' FuncRParams ')' {
    $$ = new UnaryExpAST_Ident($1, $3);
  }
  ;

UnaryOP
  : '+' {
    $$ = new UnaryOPAST('+');
  } | '-' {
    $$ = new UnaryOPAST('-');
  } | NOT {
    $$ = new UnaryOPAST('!');
  }
  ;

MulExp
  : UnaryExp {
    $$ = new MulExpAST_UnaryExp($1);
  } | MulExp MulOP UnaryExp {
    $$ = new MulExpAST_MulExp_MulOP_UnaryExp($1, $2, $3);
  }
  ;

MulOP
  : '*' {
    $$ = new MulOPAST('*');
  } | '/' {
    $$ = new MulOPAST('/');
  } | '%' {
    $$ = new MulOPAST('%');
  }
  ;

AddExp
  : MulExp {
    $$ = new AddExpAST_MulExp($1);
  } | AddExp AddOP MulExp {
    $$ = new AddExpAST_AddExp_AddOP_MulExp($1, $2, $3);
  }
  ;

AddOP
  : '+' {
    $$ = new AddOPAST('+');
  } | '-' {
    $$ = new AddOPAST('-');
  }
  ;

RelExp
  : AddExp {
    $$ = new RelExpAST_AddExp($1);
  } | RelExp RelOP AddExp {
    $$ = new RelExpAST_RelExp_RelOP_AddExp($1, $2, $3);
  }
  ;

RelOP
  : LT {
    $$ = new RelOPAST(string("<"));
  } | GT {
    $$ = new RelOPAST(string(">"));
  } | LEQ {
    $$ = new RelOPAST(string("<="));
  } | GEQ {
    $$ = new RelOPAST(string(">="));
  }
  ;

EqExp
  : RelExp {
    $$ = new EqExpAST_RelExp($1);
  } | EqExp EqOP RelExp {
    $$ = new EqExpAST_EqExp_EqOP_RelExp($1, $2, $3);
  }
  ;

EqOP
  : EQ {
    $$ = new EqOPAST(string("=="));
  } | NEQ {
    $$ = new EqOPAST(string("!="));
  }
  ;

LAndExp
  : EqExp {
    $$ = new LAndExpAST_EqExp($1);
  } | LAndExp LAND EqExp {
    $$ = new LAndExpAST_LAndExp_AND_EqExp($1, $3);
  }
  ;

LOrExp
  : LAndExp {
    $$ = new LOrExpAST_LAndExp($1);
  } | LOrExp LOR LAndExp {
    $$ = new LOrExpAST_LOrExp_OR_LAndExp($1, $3);
  }
  ;

Decl
  : ConstDecl {
    $$ = new DeclAST_ConstDecl($1);
  } | VarDecl {
    $$ = new DeclAST_VarDecl($1);
  }
  ;

ConstDecl
  : CONST FuncType ConstDef ConstDeclBlock ';' {
    $$ = new ConstDeclAST($2, $3);
    $$->push_back($4);
    // $$->update_symbol_table();
  }
  ;

ConstDeclBlock
  :  {
    $$ = new ConstDeclBlockAST();
  } | ',' ConstDef ConstDeclBlock {
    $$ = new ConstDeclBlockAST($2);
    $$->push_back($3);
  }
  ;

// BType
//   : INT {
//     $$ = new BTypeAST("int");
//   }
//   ;

ConstDef
  : IDENT ASSIGN ConstInitVal {
    $$ = new ConstDefAST($1, $3);
  } | IDENT ArrayDefIndexList ASSIGN ConstInitValList {
    $$ = new ConstDefAST_Array($1, $2, $4);
  }
  ;

ConstInitValList
  : ConstExp {
    $$ = new ConstInitValListAST($1, 1);
  } | '{' '}' {
    $$ = new ConstInitValListAST();
  } | '{' ConstInitValListBlock '}' {
    $$ = $2;
  }
  ;

ConstInitValListBlock
  : ConstInitValList {
    $$ = new ConstInitValListAST($1);
  } | ConstInitValList ',' ConstInitValListBlock {
    $$ = new ConstInitValListAST($1);
    $$->push_back($3);
  }
  ;

ConstInitVal
  : ConstExp {
    $$ = new ConstInitValAST($1);
  }
  ;

LVal
  : IDENT {
    $$ = new LValAST($1);
  }
  ;
LValArray
  : IDENT ArrayAccessIndexList {
    $$ = new LValAST_Array($1, $2);
  }
  ;

ArrayDefIndexList
  : '[' ConstExp ']' {
    $$ = new ArrayDefIndexListAST($2);
  } | '[' ConstExp ']' ArrayDefIndexList {
    $$ = new ArrayDefIndexListAST($2);
    $$->push_back($4);
  }
  ;

ArrayAccessIndexList
  : '[' Exp ']' {
    $$ = new ArrayAccessIndexListAST($2);
  } | '[' Exp ']' ArrayAccessIndexList {
    $$ = new ArrayAccessIndexListAST($2);
    $$->push_back($4);
  }
  ;

ConstExp
  : Exp {
    $$ = new ConstExpAST_Exp($1);
  }
  ;

VarDecl
  : FuncType VarDef VarDeclBlock ';' {
    $$ = new VarDeclAST($1, $2);
    $$->push_back($3);
    // $$->update_symbol_table();
  }
  ;

VarDeclBlock
  :  {
    $$ = new VarDeclBlockAST();
  } | ',' VarDef VarDeclBlock {
    $$ = new VarDeclBlockAST($2);
    $$->push_back($3);
  }
  ;

VarDef
  : IDENT {
    $$ = new VarDefAST($1);
  } | IDENT ASSIGN InitVal {  
    $$ = new VarDefAST($1, $3);
  } | IDENT ArrayDefIndexList {
    $$ = new VarDefAST_Array($1, $2);
  } | IDENT ArrayDefIndexList ASSIGN InitValList {
    $$ = new VarDefAST_Array($1, $2, $4);
  }
  ;

InitValList
  : Exp {
    $$ = new InitValListAST($1, 1);
  } | '{' '}' {
    $$ = new InitValListAST();
  } | '{' InitValListBlock '}' {
    $$ = $2;
  }
  ;

InitValListBlock
  : InitValList {
    $$ = new InitValListAST($1);
  } | InitValList ',' InitValListBlock {
    $$ = new InitValListAST($1);
    $$->push_back($3);
  }
  ;

InitVal
  : Exp {
    $$ = new InitValAST($1);
  }
  ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<RootAST> &ast, const char *s) {
  extern int yylineno;  // yy reserved variable
  extern char *yytext;  // yy reserved variable
  fprintf(stderr, "ERROR: %s at symbol '%s' on line %d\n", s, yytext, yylineno);
}
