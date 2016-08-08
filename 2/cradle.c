#include "stdio.h"
#include "stdlib.h"
#include "cradle.h"

#define UPCASE(c) ((1<<6) | (c))


void GetChar() {
  Look = getchar();
}

void Error(char* s) {
  printf("\n");
  printf("Error: %s\n", s);
}

void Abort(char* s) {
  Error(s);
  exit(1);
}

void Expected(char* s) {
  sprintf(tmp, "%s Expected", s);
  Abort(tmp);
}

void Match(char x) {
  if(Look == x) {
    GetChar();
  }
  else {
    sprintf(tmp, "' %c '", x);
    Expected(tmp);
  }
}

int IsAlpha(char x) {
  return (UPCASE(x) >= 'A' && UPCASE(x) <= 'Z');
}

int IsDigit(char x) {
  return x >= '0' && x <= '9';
}

char GetName() {
  char c = Look;

  if(!IsAlpha(Look)) {
      Expected("Name");
  }
  GetChar();
  return UPCASE(c);
}

char GetNum() {
  char c = Look;

  if(!IsDigit(Look)) {
    Expected("Integer");
  }
  GetChar();
  return c;
}

void Emit(char* s) {
  printf("\t%s", s);
}

void EmitLn(char* s) {
  Emit(s);
  printf("\n");
}

void Init() {
  GetChar();
}

int IsAddop(char c) {
  return c == '+' || c == '-';
}
