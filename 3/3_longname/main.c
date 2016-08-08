#include "cradle.h"
#include "stdio.h"
#include "stdlib.h"
int main() {
  Init();
  Assignment();
  if(Look != '\n') Expected("Newline");
}

void Assignment() {
  char* Name = GetName();
  Match('=');
  Expression();
  sprintf(tmp, "mov [%s], rax", Name);
  free(Name);
  EmitLn(tmp);
}

void Expression() {
  if(IsAddop(Look)) {
    EmitLn("xor rax, rax");
  }
  else {
    Term();
  }
  while(IsAddop(Look)) {
    EmitLn("push rax");
    switch(Look) {
    case '+' : Add();break;
    case '-' : Subtract();break;
    }
  }
}

void Term() {
  Factor();
  while(Look == '*' || Look == '/') {
    EmitLn("push rax");
    switch(Look) {
    case '*' : Multiply();break;
    case '/' : Divide();break;
    }
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
    char* Num = GetNum();   
    sprintf(tmp, "mov rax, %s", Num);
    free(Num);
    EmitLn(tmp);
  }
}

void Ident() {
  char* Name = GetName();
  if(Look == ('(')) {
    Match('(');
    Match(')');
    sprintf(tmp, "call %s", Name);
    EmitLn(tmp);
  } else {
    sprintf(tmp, "mov rax, [%s]", Name);
    EmitLn(tmp);
  }
  free(Name);
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

