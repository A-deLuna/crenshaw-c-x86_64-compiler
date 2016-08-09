#include "cradle.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

char SymTab[1000][8]; 

const char KWcode[] = "xilee";


void Scan() {
  SkipWhite();
  while(Look == '\n') Fin();
  GetName();
  int k = Lookup(KWlist, Value, KWlistSz);
  Token = KWcode[k+1];
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



void Other() {
  //sprintf(tmp, "%c", GetName());
  EmitLn(tmp);
}

void Condition() {
  printf(" <CONDITION>\n");
}
int main() {
  Init();
  DoProgram();
}

void DoProgram() {
  Block();
  MatchString("END");
  EmitLn("END");
}

void Block() {
  Scan();
  while(Token != 'e' && Token !='l') {
    switch(Token) {
      case 'i': DoIf(); break;
      default : Assignment(); break;
    }
    Scan();
  }
}
void DoIf() {
  char L1[100];
  char L2[100];
  Condition();
  strcpy(L1,  NewLabel());
  strcpy(L2,  L1);
  sprintf(tmp, "jz %s", L1);
  EmitLn(tmp);
  Block();
  if(Token == 'l') {
    strcpy(L2,  NewLabel());
    sprintf(tmp, "jmp %s", L2);
    EmitLn(tmp);
    PostLabel(L1);
    Block();
  }
  PostLabel(L2);
  MatchString("ENDIF");
}


void Assignment() {
  char Name[80];
  strcpy(Name, Value);
  Match('=');
  Expression();
  sprintf(tmp, "mov [%s], rax", Name);
  EmitLn(tmp);
}

void Expression() {
  FirstTerm();
  while(IsAddop(Look)) {
    EmitLn("push rax");
    switch(Look) {
      case '+': Add(); break;
      case '-': Subtract(); break;
    }
  }
}

void Term() {
  Factor();
  Term1();
}
void FirstTerm() {
  SignedFactor();
  Term1();
}

void Term1() {
  while(Look == '*' || Look == '/') {
    EmitLn("push rax");
    switch(Look) {
    case '*' : Multiply();break;
    case '/' : Divide();break;
    }
  }
}
void SignedFactor() {
  int s = Look == '-';
  if(IsAddop(Look)) {
    GetChar();
    SkipWhite();
  }
  Factor();
  if(s) {
    EmitLn("neg rax");
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
    GetNum();
    sprintf(tmp, "mov rax, %s", Value);
    EmitLn(tmp);
  }
}

void Ident() {
  GetName();
  if(Look == ('(')) {
    Match('(');
    Match(')');
    sprintf(tmp, "call %s", Value);
    EmitLn(tmp);
  } else {
    sprintf(tmp, "mov rax, [%s]", Value);
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
  SkipWhite();
}

int Lookup(const char * const * T, char * s, int n) {
  int i = n-1;
  int found = 0;
  while(i >= 0 && !found) {
    if(strcmp(s, T[i]) == 0) {
      found = 1;
    }
    else {
      i--;
    }
  }
  return i;
}
