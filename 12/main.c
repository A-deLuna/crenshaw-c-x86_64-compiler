#include "cradle.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

char KWcode [] = "xileweRWve";
const char * const KWlist[] = {
  "IF",
  "ELSE",
  "ENDIF",
  "WHILE",
  "ENDWHILE",
  "READ",
  "WRITE",
  "VAR",
  "END",
};

int KWsz = sizeof KWlist / sizeof *KWlist;
int main() {
  Init();
  MatchString("PROGRAM");
  Semi();
  Header();
  TopDecls();
  MatchString("BEGIN");
  Prolog();
  Block();
  MatchString("END");
  Epilog();
}
void Header() {
  EmitLn("extern printf");
  EmitLn("extern scanf");
  EmitLn("section .data");
  printf("fmt:	db \"%%d\", 10, 0\n");
  printf("fmtin:	db \"%%d\", 0\n");

}

void TopDecls() {
  EmitLn("section .data");
  Scan();
  while(Token == 'v') {
    Alloc();
    while(Token == ',') {
      Alloc();
    }
    Semi();
  }
}

void Allocate(char* Name, char* Val) {
  printf("%s:	DQ %s\n", Name, Val);
}
void Alloc() {
  Next();
  if(Token != 'x') {
    Expected("Variable Name");
  }
  CheckDup(Value);
  AddEntry(Value, 'v');
  if(Token == '=') {
    char Name[100];
    strcpy(Name, Value);
    Next();
    if(Token == '-') {
    }
    Allocate(Name, Value);
  }
  else {
    Allocate(Value, "0");
  }
  Next();

}

int InTable(char* N) {
  return LookupSymbol(N, 100) != -1;
}

void Statement() {
  Scan();
    switch(Token) {
      case 'i':
        DoIf();
        break;
      case 'w':
        DoWhile();
        break;
      case 'W':
        DoWrite();
        break;
      case 'R':
        DoRead();
        break;
      case 'x':
        Assignment();
        break;
    }
}
void Block() {
  Statement();
  while(Token == ';') {
    Next();
    Statement();
  }
}

void DoWrite() {
  Next();
  MatchString("(");
  Expression();
  WriteVar();
    NewLine();
  while(Token == ',') {
    Next();
    Expression();
    WriteVar();
  }
  MatchString(")");
}

void DoRead() {
  Next();
  MatchString("(");
  ReadVar();
  while(Token == ',') {
    Next();
    ReadVar();
  }
  MatchString(")");
}
void Assignment() {
  char Name[100];
  CheckTable(Value);
  strcpy(Name, Value);
  Next();
  MatchString("=");
  BoolExpression();
  Store(Name);
}

void DoWhile() {
  char L1[100];
  char L2[100];
  Next();
  strcpy(L1, NewLabel());
  strcpy(L2, NewLabel());
  PostLabel(L1);
  BoolExpression();
  BranchFalse(L2);
  Block();
  MatchString("ENDWHILE");
  Branch(L1);
  PostLabel(L2);
}

void DoIf() {
  char L1[100];
  char L2[100];
  Next();
  BoolExpression();
  strcpy(L1,  NewLabel());
  strcpy(L2,  L1);
  BranchFalse(L1);
  Block();
  if(Token == 'l') {
    Next();
    strcpy(L2,  NewLabel());
    Branch(L2);
    PostLabel(L1);
    Block();
  }
  PostLabel(L2);
  MatchString("ENDIF");
}

void BoolExpression() {
  BoolTerm();
  while(IsOrOp(Token)) {
    Push();
    switch(Token) {
      case '|': BoolOr(); break;
      case '~': BoolXor(); break;
    }
  }
}
void BoolXor() {
  Next();
  BoolTerm();
  PopXor();
}
void BoolOr() {
  Next();
  BoolTerm();
  PopOr();
}

void BoolTerm() {
  NotFactor();
  while(Token == '&') {
    Push();
    Next();
    NotFactor();
    PopAnd();
  }
}
void NotFactor() {
  if(Token == '!') {
    Next();
    Relation();
    NotIt();
  }
  else {
    Relation();
  }
}

void Relation() {
  Expression();
  if(IsRelop(Token)) {
    Push();
    switch(Token) {
      case '=': Equals();break;
      case '#': NotEquals();break;
      case '<': Less();break;
      case '>': Greater();break;
    }
  }
}

void Equals() {
  Next();
  Expression();
  Compare();
  SetEqual();
  Pop();
}

void NotEquals() {
  Next();
  Expression();
  Compare();
  SetNEqual();
  Pop();
}

void Less() {
  Next();
  if(Token == '=') {
    LessOrEqual();
  }
  else {
    Expression();
    Compare();
    SetLess();
    Pop();
  }
}

void Greater() {
  Next();
  if(Token == '=') {
    GreaterOrEqual();
  }
  else {
    Expression();
    Compare();
    SetGreater();
    Pop();
  }
}

void LessOrEqual() {
  Next();
  Expression();
  Compare();
  SetLessOrEqual();
  Pop();
}

void GreaterOrEqual() {
  Next();
  Expression();
  Compare();
  SetGreaterOrEqual();
  Pop();
}

void Expression() {
  if(IsAddop(Token)) {
    Clear();
  }
  else {
    Term();
  }
  while(IsAddop(Token)) {
    Push();
    switch(Token) {
      case '+': Add(); break;
      case '-': Subtract(); break;
    }
  }
}


void Add() {
  Next();
  Term();
  PopAdd();
}
void Term() {
  Factor();
  while(Token == '*' || Token == '/') {
    Push();
    switch(Token) {
      case '*': Multiply(); break;
      case '/': Divide(); break;
    }
  }
}

void Factor() {
  if(Token == '(') {
    Next();
    BoolExpression();
    MatchString(")");
  }
  else {
    if(Token == 'x') {
      LoadVar(Value);
    } else if(Token == '#') {
      LoadConst(Value);
    } else {
      Expected("Math Factor");
    }
    Next();
  }
}

void ReadVar() {
  CheckIdent();
  CheckTable(Value);
  EmitLn("push rax");
  EmitLn("mov rdi, fmtin");
  sprintf(tmp,"mov rsi, %s", Value);
  EmitLn(tmp);
  EmitLn("xor rax, rax");
  EmitLn("call scanf");
  EmitLn("pop rax");
  Next();
}

void WriteVar() {
  EmitLn("push rax");
  EmitLn("mov rdi, fmt");
  EmitLn("mov rsi, rax");
  EmitLn("xor rax, rax");
  EmitLn("call printf");
  EmitLn("pop rax");
}

void Branch(char* L) {
  sprintf(tmp,"jmp %s", L);
  EmitLn(tmp);
}

void BranchFalse(char* L) {
  EmitLn("cmp rax, 0");
  sprintf(tmp,"je %s", L);
  EmitLn(tmp);
}
void Clear() {
  EmitLn("xor rax, rax");
}

void Negate() {
  EmitLn("neg rax");
}

void LoadConst(char* n){
  sprintf(tmp, "mov rax, %s", n);
  EmitLn(tmp);
}

void LoadVar(char* Name) {
  if(!InTable(Name)) {
    Undefined(Name);
  }
  sprintf(tmp, "mov rax, [%s]", Name);
  EmitLn(tmp);
}

void Push() {
  EmitLn("push rax");
}

void PopAdd() {
  EmitLn("add rax, [rsp]");
  EmitLn("add rsp, 8");
}

void PopSub() {
  EmitLn("sub rax, [rsp]");
  EmitLn("add rsp, 8");
}

void PopMul() {
  EmitLn("imul rax, [rsp]");
  EmitLn("add rsp, 8");
}

void PopDiv() {
  EmitLn("mov rbx, rax");
  EmitLn("pop rax");
  EmitLn("cqo");
  EmitLn("idiv rbx");
}

void Prolog() {
  EmitLn("section .text");
  EmitLn("global  main");
  PostLabel("main");
  EmitLn("push rbp");
}

void Store(char* Name) {
  if(!InTable(Name)) {
    Undefined(Name);
  }
  sprintf(tmp, "mov [%s], rax", Name);
  EmitLn(tmp);
}

void NotIt() {
  EmitLn("not rax");
}

void PopAnd() {
  EmitLn("and rax, [rsp]");
  EmitLn("add rsp, 8");
}

void PopOr() {
  EmitLn("or rax, [rsp]");
  EmitLn("add rsp, 8");
}

void PopXor() {
  EmitLn("xor rax, [rsp]");
  EmitLn("add rsp, 8");
}

void Compare() {
  EmitLn("cmp rax, [rsp]");
}

void Pop() {
  EmitLn("add rsp, 8");
}

void SetEqual() {
  EmitLn("sete al");
  EmitLn("movsx rax, al");
}

void SetNEqual() {
  EmitLn("setne al");
  EmitLn("movsx rax, al");
}

void SetGreater() {
  EmitLn("setl al");
  EmitLn("movsx rax, al");
}

void SetLess() {
  EmitLn("setg al");
  EmitLn("movsx rax, al");
}

void SetLessOrEqual() {
  EmitLn("setge al");
  EmitLn("movsx rax, al");
}

void SetGreaterOrEqual() {
  EmitLn("setle al");
  EmitLn("movsx rax, al");
}

void Undefined(char* n) {
  sprintf(tmp, "Undefined Identifier %s", n);
  Abort(tmp);
}

void Epilog() {
  EmitLn("xor rax, rax");
  EmitLn("ret");
}

void Subtract() {
  Next();
  Term();
  PopSub();
}


void Multiply() {
  Next();
  Factor();
  PopMul();
}

void Divide() {
  Next();
  Factor();
  PopDiv();
}

