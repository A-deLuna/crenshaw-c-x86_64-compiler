#include "cradle.h"
#include "stdio.h"
int main() {
  Init();
  Expression();
}

void Term() {
  Factor();
  while(Look == '*' || Look == '/') {
    EmitLn("push rax");
    switch(Look) {
    case '*' : Multiply();break;
    case '/' : Divide();break;
    default : Expected("Mulop");break;
    }
  }
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
    default: Expected("Addop"); break;
    }
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

void Factor() {
  if(Look == '(') {
    Match('(');
    Expression();
    Match(')');
  }
  else {
    sprintf(tmp, "mov rax, %c", GetNum());
    EmitLn(tmp);
  }
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
