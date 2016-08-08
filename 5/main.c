#include "cradle.h"
#include "stdio.h"
#include "string.h"
int main() {
  Init();
  DoProgram();
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
    switch(Look) {
      case 'i': DoIf(); break;
      case 'w': DoWhile(); break;
      case 'p': DoLoop(); break;
      case 'r': DoRepeat(); break;
      case 'f': DoFor(); break;
      default : Other(); break;
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
  Condition();
  strcpy(L1,  NewLabel());
  strcpy(L2,  L1);
  sprintf(tmp, "jz %s", L1);
  EmitLn(tmp);
  Block();
  if(Look == 'l') {
    Match('l');
    strcpy(L2,  NewLabel());
    sprintf(tmp, "jnz %s", L2);
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
  Condition();
  sprintf(tmp, "jz %s", L2);
  EmitLn(tmp);
  Block();
  Match('e');
  sprintf(tmp, "jnz %s", L1);
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
  sprintf(tmp, "jnz %s", L);
  EmitLn(tmp);
}

void DoRepeat() {
  char L[100];
  Match('r');
  strcpy(L, NewLabel());
  PostLabel(L);
  Block();
  Match('u');
  Condition();
  sprintf(tmp, "jnz %s", L);
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

void Condition() {
  EmitLn("<condition>");
}

void Assignment() {
  char Name = GetName();
  Match('=');
  Expression();
  sprintf(tmp, "mov [%c], rax", Name);
  EmitLn(tmp);
}

void Expression() {
  EmitLn("<expr>");
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

