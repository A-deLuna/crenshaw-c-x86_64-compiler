#include "stdio.h"
#include "stdlib.h"
#include "cradle.h"

#define UPCASE(c) (~(1<<5) & (c))


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
  if(Look != x) {
    sprintf(tmp, "' %c '", x);
    Expected(tmp);
  }
  else {
    GetChar();
    SkipWhite();
  }
}

int IsAlpha(char x) {
  return (UPCASE(x) >= 'A' && UPCASE(x) <= 'Z');
}

int IsDigit(char x) {
  return x >= '0' && x <= '9';
}

char *  GetName() {
  if(!IsAlpha(Look)) {
      Expected("Name");
  }
  char* Token = malloc(100 * sizeof *Token);
  Token[0] = '\0';
  int i = 0;
  while(IsAlNum(Look)) {
    Token[i] = UPCASE(Look);
    GetChar();
    i++;
  }
  Token[i] = '\0';
  SkipWhite();
  return Token;
}

char* GetNum() {
  if(!IsDigit(Look)) {
    Expected("Integer");
  }
  char* Value = malloc(100 * sizeof *Value);
  int i = 0;
  while(IsDigit(Look)) {
    Value[i] = Look;
    GetChar();
    i++;
  }
  Value[i] = '\0';
  SkipWhite();
  return Value;
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
  SkipWhite();
}

int IsAddop(char c) {
  return c == '+' || c == '-';
}

int IsAlNum(char c) {
  return IsAlpha(c) || IsDigit(c);
}

int IsWhite(char c) {
  return c == ' ' || c == '\t';
}

void SkipWhite() {
  while(IsWhite(Look)) {
    GetChar();
  }
}
