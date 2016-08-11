#include "cradle.h"
#include "stdio.h"
#include "string.h"

char KWcode [] = "xilewRWevbep";
const char * const KWlist[] = {
  "IF",
  "ELSE",
  "ENDIF",
  "WHILE",
  "READ",
  "WRITE",
  "ENDWHILE",
  "VAR",
  "BEGIN",
  "END",
  "PROGRAM"
};

int KWsz = sizeof KWlist / sizeof *KWlist;
int main() {
  Init();

  Prog();
  if(Look != '\n') {
    printf("Unexpected data after '.'\n");
    return 1; 
  }
}
void Prog() {
  MatchString("PROGRAM");
  Header();
  TopDecls();
  Main();
  Match('.');
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
  while(Token != 'b') {
    switch(Token) {
      case 'v': Decl(); break;
      default:
        sprintf(tmp,"Unrecognized Keyword '%s'", Value);
        Abort(tmp);
    }
    Scan();
    NewLine();
  }
}

void Decl() {
  GetName();
  Alloc(Value);
  SkipWhite();
  while (Look == ',') {
    Match(',');
    GetName();
    Alloc(Value);
  }
}
void Alloc(char* Name) {
  if(InTable(Name)) {
    sprintf(tmp, "Duplicate Variable Name %s", Name);
    Abort(tmp);
  }
  AddEntry(Name, 'v');
  printf( "%s:	DQ ", Name);
  if(Look == '=') {
    Match('=');
    if(Look == '-') {
      printf("-");
      Match('-');
    }
    printf("%d\n", GetNum());
  }
  else {
    printf("0\n");
  }

}

int InTable(char* N) {
  return LookupSymbol(N, 100) != -1;
}

void Main() {
  MatchString("BEGIN");
  Prolog();
  Block();
  MatchString("END");
  Epilog();
}

void Block() {
  Scan();
  NewLine();
  while(Token != 'e' && Token != 'l') {
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
      default:
        Assignment();
    }
    NewLine();
    Scan();
  }
}

void DoWrite() {
  Match('(');
  Expression();
  WriteVar();
    NewLine();
  while(Look == ',') {
    Match(',');
    Expression();
    WriteVar();
    NewLine();
  }
  Match(')');
}

void DoRead() {
  Match('(');
  GetName();
  ReadVar();
  while(Look == ',') {
    Match(',');
    GetName();
    ReadVar();
  }
  Match(')');
}
void Assignment() {
  char Name[100];
  strcpy(Name, Value);
  Match('=');
  BoolExpression();
  Store(Name);
}

void DoWhile() {
  char L1[100];
  char L2[100];

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
  BoolExpression();
  strcpy(L1,  NewLabel());
  strcpy(L2,  L1);
  BranchFalse(L1);
  Block();
  if(Token == 'l') {
    Match('l');
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
  NewLine();
  while(IsOrOp(Look)) {
    Push();
    switch(Look) {
      case '|': BoolOr(); break;
      case '~': BoolXor(); break;
    }
    NewLine();
  }
}
void BoolXor() {
  Match('~');
  BoolTerm();
  PopXor();
}
void BoolOr() {
  Match('|');
  BoolTerm();
  PopOr();
}

void BoolTerm() {
  NotFactor();
  NewLine();
  while(Look == '&') {
    Push();
    Match('&');
    NotFactor();
    PopAnd();
    NewLine();
  }
}
void NotFactor() {

  if(Look == '!') {
    Match('!');
    Relation();
    NotIt();
  }
  else {
    Relation();
  }
}

void Relation() {
  Expression();
  if(IsRelop(Look)) {
    Push();
    switch(Look) {
      case '=': Equals();break;
      case '#': NotEquals();break;
      case '<': Less();break;
      case '>': Greater();break;
    }
  }
}

void Equals() {
  Match('=');
  Expression();
  Compare();
  SetEqual();
  Pop();
}

void NotEquals() {
  Match('#');
  Expression();
  Compare();
  SetNEqual();
  Pop();
}

void Less() {
  Match('<');
  if(Look == '=') {
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
  Match('>');
  if(Look == '=') {
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
  Match('=');
  Expression();
  Compare();
  SetLessOrEqual();
  Pop();
}

void GreaterOrEqual() {
  Match('=');
  Expression();
  Compare();
  SetGreaterOrEqual();
  Pop();
}

void Expression() {
  FirstTerm();
  NewLine();
  while(IsAddop(Look)) {
    Push();
    switch(Look) {
      case '+': Add(); break;
      case '-': Subtract(); break;
    }
    NewLine();
  }
}

void FirstTerm() {
  FirstFactor();
  Term1();
}

void Add() {
  Match('+');
  Term();
  PopAdd();
}
void Term() {
  Factor();
  Term1();
}
void Term1() {
  NewLine();
  while(Look == '*' || Look == '/') {
    Push();
    switch(Look) {
      case '*': Multiply(); break;
      case '/': Divide(); break;
    }
    NewLine();
  }
}

void FirstFactor() {
  switch(Look) {
    case '+': 
      Match('+');
      Factor();
      break;
    case '-':
      NegFactor();
      break;
    default: Factor();
  }
}
void NegFactor() {
  Match('-');
  if(IsDigit(Look)) {
    LoadConst(-GetNum());
  }
  else {
    Factor();
    Negate();
  }
}

void Factor() {
  if(Look == '(') {
    Match('(');
    BoolExpression();
    Match(')');
  }
  else if(IsAlpha(Look)) {
    GetName();
    LoadVar(Value);
  } else {
    LoadConst(GetNum());
  }
}

void ReadVar() {
  EmitLn("push rax");
  EmitLn("mov rdi, fmtin");
  sprintf(tmp,"mov rsi, %s", Value);
  EmitLn(tmp);
  EmitLn("xor rax, rax");
  EmitLn("call scanf");
  EmitLn("pop rax");
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

void LoadConst(int n){
  sprintf(tmp, "mov rax, %d", n);
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
  Match('-');
  Term();
  PopSub();
}


void Multiply() {
  Match('*');
  Factor();
  PopMul();
}

void Divide() {
  Match('/');
  Factor();
  PopDiv();
}

