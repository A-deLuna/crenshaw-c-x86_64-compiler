#include "cradle.h"
#include "stdio.h"
int main() {
  Init();
  //Assignment();
  //if(Look != '\n') Expected("Newline");
  while(Look != '.') {
    switch(Look) {
      case '?' : Input(); break;
      case '!' : Output(); break;
      default: Assignment();
    }
    NewLine();
  }
}

void Assignment() {
  char Name = GetName();
  Match('=');
  Table[Name - 'A'] = Expression();
}

int Expression() {
  int Value;
  if(IsAddop(Look)) {
    Value = 0;
  } 
  else {
    Value = Term();
  }
  while(IsAddop(Look)) {
    switch(Look) {
      case '+' :
        Match('+');
        Value = Value + Term();
        break;
      case '-':
        Match('-');
        Value = Value - Term();
        break;
    }
  }
  return Value;
}

int Term() {
  int Value = Factor();
  while(Look == '*' || Look == '/') {
    switch(Look) {
    case '*' : 
      Match('*');
      Value = Value * Factor();
      break;
    case '/' :
      Match('/');
      Value = Value / Factor();
      break;
    }
  }
  return Value;
}

int Factor() {
  int Value;
  if(Look == '(') {
    Match('(');
    Value = Expression();
    Match(')');
  } else if(IsAlpha(Look)) {
    Value = Table[GetName() - 'A'];
  }
  else {
    Value = GetNum();
  }
  return Value;
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

void NewLine() {
  if(Look == '\n') {
    GetChar();
  }
}

void Input() {
  Match('?');
  Table[GetName() - 'A'] = Expression();
}

void Output() {
  Match('!');
  printf("%d\n", Table[GetName() - 'A']);
}
