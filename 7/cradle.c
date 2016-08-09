#include "stdio.h"
#include "stdlib.h"
#include "cradle.h"
#include "string.h"

#define UPCASE(c) (~(1<<5) & (c))


const char * const KWlist[] = {
  "IF",
  "ELSE",
  "ENDIF",
  "END"
};
int KWlistSz = sizeof KWlist / sizeof *KWlist; 
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
  GetChar();
  SkipWhite();
}

void MatchString(char* string) {
  if(strcmp(Value, string) != 0) {
    Expected(string);
  }
}

int IsAlpha(char x) {
  return (UPCASE(x) >= 'A' && UPCASE(x) <= 'Z');
}

int IsDigit(char x) {
  return x >= '0' && x <= '9';
}

void GetName() {
  if(!IsAlpha(Look)) {
      Expected("Name");
  }
  int i = 0;
  
  while(IsAlNum(Look)) {
    Value[i] = UPCASE(Look);
    i++;
    GetChar();
  }
  Value[i] = '\0';
  SkipWhite();
  
}

void GetNum() {
  if(!IsDigit(Look)) {
    Expected("Integer");
  }
  int i = 0;
  while(IsDigit(Look)) {
    Value[i] = Look;
    i++;
    GetChar();
  }
  Value[i] = '\0';
  Token = '#';
  SkipWhite();
}

void GetOp() {
  if(!IsOp(Look)) {
    Expected("Operator");
  }
  int i = 0;
  while(IsOp(Look)) {
    Value[i] = Look;
    i++;
    GetChar();
  }
  Value[i] = '\0';
  if(i == 1) {
    Token = Value[0];
  }
  else {
    Token = '?';
  }
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

char* NewLabel() {
  sprintf(LabelName, "L%02d", LCount);
  LCount++;
  return LabelName;
}

void PostLabel(char* L) {
  printf("%s:\n", L);
}

int IsBoolean(char c) {
  return UPCASE(c) == 'T' || UPCASE(c) == 'F';
}

int IsOrOp(char c) {
  return c == '|' || c == '~';
}

int GetBoolean() {
  if(!IsBoolean(Look)) {
    Expected("Boolean Literal");
  }
  int b = UPCASE(Look) == 'T';
  GetChar();
  return b;

}

int IsRelop(char c) {
  return c == '=' ||
         c == '#' ||
         c == '<' ||
         c == '>';
}

int IsAlNum(char c) {
  return IsAlpha(c) || IsDigit(c);
}

int IsWhite(char c) {
  return c == ' ' || c == '\t' ;
}

void SkipWhite() {
  while(IsWhite(Look)) {
    GetChar();
  }
}

void SkipComma() {
  SkipWhite();
  if(Look == ',') {
    GetChar();
    SkipWhite();
  }
}

int IsOp(char c) {
  return strchr("+-*/<>:=", c) != NULL;
}
