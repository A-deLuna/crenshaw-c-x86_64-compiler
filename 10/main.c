#include "cradle.h"
#include "stdio.h"
#include "string.h"
int main() {
  Init();

  Prog();
  if(Look != '\n') {
    printf("Unexpected data after '.'\n");
    return 1; 
  }
  /*
  //BoolExpression();
  DoProgram();*/
}
void Prog() {
  Match('p');
  Header();
  TopDecls();
  Main();
  Match('.');
}
void Header() {
}

void TopDecls() {
  EmitLn("section .data");
  while(Look != 'b') {
    switch(Look) {
      case 'v': Decl(); break;
      default:
        sprintf(tmp,"Unrecognized Keyword '%c'", Look);
        Abort(tmp);
    }
  }
}

void Decl() {
  Match('v');
  Alloc(GetName());
  while (Look == ',') {
    Match(',');
    Alloc(GetName());
  }
}
void Alloc(char Name) {
  if(InTable(Name)) {
    sprintf(tmp, "Duplicate Variable Name %c", Name);
    Abort(tmp);
  }
  Table[Name-'A'] = 'v';
  printf("%c:	dq ", Name);
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

int InTable(char N) {
  return Table[N-'A'] != 0;
}

void Main() {
  Match('b');
  Prolog();
  Match('e');
  Epilog();
}

void Prolog() {
  EmitLn("extern printf");
  EmitLn("section .text");
  EmitLn("global  main");
  PostLabel("main");
  
  
}

void Epilog() {
  EmitLn("xor rax, rax");
  EmitLn("ret");
}

void DoBlock(char Name) {
  Declarations();
  sprintf(tmp, "%c", Name);
  PostLabel("main");
  EmitLn("push rbp");
  Statements();
}

void Declarations() {

  while(strchr("lctvpf", Look)) {
    switch(Look) {
      case 'l': Labels();
      case 'c': Constants();
      case 't': Types();
      case 'v': Variables();
      case 'p': DoProcedure();
      case 'f': DoFunction();
    }
  }
}

void Labels() {
  Match('l');
}
void Constants() {
  Match('c');
}
void Types() {
  Match('t');
}
void Variables() {
  Match('v');
}
void DoProcedure() {
  Match('p');
}
void DoFunction() {
  Match('f');
}

void Statements() {
  Match('b');
  while(Look != 'e') {
    GetChar();
  }
  Match('e');
}


void BoolExpression() {
  BoolTerm();
  while(IsOrOp(Look)) {
    EmitLn("push rax");
    switch(Look) {
      case '|': BoolOr();break;
      case '~': BoolXor();break;
    }
  }
}

void BoolTerm() {
  NotFactor();
  while (Look == '&') {
    EmitLn("push rax");
    Match('&');
    NotFactor();
    EmitLn("and rax, [rsp]");
    EmitLn("add rsp, 8");
  }
}

void NotFactor() {
  if(Look == '!') {
    Match('!');
    BoolFactor();
    EmitLn("not rax");
  }
  else {
    BoolFactor();
  }
}
void BoolFactor() {
  if(IsBoolean(Look)) {
    if(GetBoolean()) {
      EmitLn("mov rax, -1");
    }
    else {
      EmitLn("xor rax, rax");
    }
  }
  else {
    Relation();
  }
}
void Relation() {
  Expression();
  if (IsRelop(Look)) {
    EmitLn("push rax");
    switch(Look) {
      case '=': Equals(); break;
      case '#': NotEquals(); break;
      case '<': Less(); break;
      case '>': Greater(); break;
    }
    EmitLn("add rsp, 8");
    EmitLn("cmp rax, 0");
  }
}

void Equals() {
  Match('=');
  Expression();
  EmitLn("cmp rax, [rsp]");
  EmitLn("sete al");
}

void NotEquals() {
  Match('#');
  Expression();
  EmitLn("cmp rax, [rsp]");
  EmitLn("setne al");
}

void Less() {
  Match('<');
  Expression();
  EmitLn("cmp rax, [rsp]");
  EmitLn("setg al");
}

void Greater() {
  Match('>');
  Expression();
  EmitLn("cmp rax, [rsp]");
  EmitLn("setle al");

}
void BoolOr() {
  Match('|');
  BoolTerm();
  EmitLn("or rax, [rsp]");
  EmitLn("add rsp, 8");
}

void BoolXor() {
  Match('~');
  BoolTerm();
  EmitLn("xor rax, [rsp]");
  EmitLn("add rsp, 8");
}

void DoProgram() {
  Block();
  if(Look != 'e') {
    Expected("End");
  }
  EmitLn("END");
}


void Block() {
  while(Look != 'e' && Look !='l' && Look != 'u') {
    Fin();
    switch(Look) {
      case 'i': DoIf(); break;
      case 'w': DoWhile(); break;
      case 'p': DoLoop(); break;
      case 'r': DoRepeat(); break;
      case 'f': DoFor(); break;
      default : Assignment(); break;
    }
  }
}

void Other() {
  sprintf(tmp, "%c", GetName());
  EmitLn(tmp);
}

void DoIf() {
  char L1[100];
  char L2[100];
  Match('i');
  BoolExpression();
  strcpy(L1,  NewLabel());
  strcpy(L2,  L1);
  sprintf(tmp, "jz %s", L1);
  EmitLn(tmp);
  Block();
  if(Look == 'l') {
    Match('l');
    strcpy(L2,  NewLabel());
    sprintf(tmp, "jmp %s", L2);
    EmitLn(tmp);
    PostLabel(L1);
    Block();
  }
  Match('e');
  PostLabel(L2);
}

void DoWhile() {
  char L1[100];
  char L2[100];

  Match('w');
  strcpy(L1, NewLabel());
  strcpy(L2, NewLabel());
  PostLabel(L1);
  BoolExpression();
  sprintf(tmp, "jz %s", L2);
  EmitLn(tmp);
  Block();
  Match('e');
  sprintf(tmp, "jmp %s", L1);
  EmitLn(tmp);
  PostLabel(L2);
}

void DoLoop() {
  char L[100];
  Match('p');
  strcpy(L, NewLabel());
  PostLabel(L);
  Block();
  Match('e');
  sprintf(tmp, "jmp %s", L);
  EmitLn(tmp);
}

void DoRepeat() {
  char L[100];
  Match('r');
  strcpy(L, NewLabel());
  PostLabel(L);
  Block();
  Match('u');
  BoolExpression();
  sprintf(tmp, "jmp %s", L);
  EmitLn(tmp);
}

void DoFor() {
  char L1[100];
  char L2[100];

  Match('f');
  strcpy(L1, NewLabel());
  strcpy(L2, NewLabel());
  char Name = GetName();

  Match('=');
  Expression();
  EmitLn("dec rax");
  sprintf(tmp, "mov qword [%c], rax", Name);
  EmitLn(tmp);
  Expression();
  EmitLn("push rax");
  PostLabel(L1);
  sprintf(tmp, "mov rax, qword [%c]", Name);
  EmitLn(tmp);
  EmitLn("inc rax");
  sprintf(tmp, "mov qword [%c], rax", Name);
  EmitLn(tmp);
  EmitLn("cmp rax, qword [rsp]");
  sprintf(tmp, "jg %s", L2);
  EmitLn(tmp);
  Block();
  Match('e'); 
  sprintf(tmp, "jmp %s", L1);
  EmitLn(tmp);
  PostLabel(L2);
  EmitLn("add rsp, 8");

}

void Assignment() {
  char Name = GetName();
  Match('=');
  BoolExpression();
  sprintf(tmp, "mov [%c], rax", Name);
  EmitLn(tmp);
}

void Expression() {
  Term();
  while(IsAddop(Look)) {
    EmitLn("push rax");
    switch(Look) {
      case '+': Add(); break;
      case '-': Subtract(); break;
    }
  }
}

void Term() {
  SignedFactor();
  while(Look == '*' || Look == '/') {
    EmitLn("push rax");
    switch(Look) {
    case '*' : Multiply();break;
    case '/' : Divide();break;
    }
  }
}

void SignedFactor() {
  if(Look == '+') {
    GetChar();
  }
  else if(Look == '-') {
    GetChar();
    if(IsDigit(Look)) {
      sprintf(tmp, "mov rax, -%c", GetNum());
      EmitLn(tmp);
    }
    else {
      Factor();
      EmitLn("neg rax");
    }
  }
  else {
    Factor();
  }
}

void Factor() {
  if(Look == '(') {
    Match('(');
    Expression();
    Match(')');
  }
  else if(IsAlpha(Look)) {
    Ident();
  } else {
    
    sprintf(tmp, "mov rax, %c", GetNum());
    EmitLn(tmp);
  }
}

void Ident() {
  char Name = GetName();
  if(Look == ('(')) {
    Match('(');
    Match(')');
    sprintf(tmp, "call %c", Name);
    EmitLn(tmp);
  } else {
    sprintf(tmp, "mov rax, [%c]", Name);
    EmitLn(tmp);
  }
}

void Add() {
  Match('+');
  Term();
  EmitLn("pop rbx");
  EmitLn("add rax, rbx");
}

void Subtract() {
  Match('-');
  Term();
  EmitLn("pop rbx");
  EmitLn("sub rax, rbx");
  EmitLn("neg rax");
}


void Multiply() {
  Match('*');
  Factor();
  EmitLn("pop rbx");
  EmitLn("imul rax, rbx");
}

void Divide() {
  Match('/');
  Factor();
  EmitLn("mov rbx, rax");
  EmitLn("pop rax");
  EmitLn("cqo");
  EmitLn("idiv rbx");
}

void Fin() {
  if(Look == '\n') GetChar();
}
