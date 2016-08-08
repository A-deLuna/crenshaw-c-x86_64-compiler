#include "cradle.h"
#include "stdio.h"
#include "string.h"
int main() {
  Init();
  BoolExpression();
  //DoProgram();
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
  EmitLn("setge al");
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

