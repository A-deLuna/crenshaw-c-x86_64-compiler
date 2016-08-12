#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "cradle.h"

#define UPCASE(c) (~(1<<5) & (c))

int NEntry = 0;

void GetChar() {
  Look = getchar();
}

void Error(char* s) {
  printf("\n");
  fprintf(stderr, "Error: %s\n", s);
}

void Abort(char* s) {
  Error(s);
  exit(1);
}

void Expected(char* s) {
  fprintf(stderr, "Look: %c, Token: %c, Value: %s\n", Look, Token, Value);
  sprintf(tmp, "%s Expected", s);
  Abort(tmp);
}

void Match(char x) {
  NewLine();
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

void NewLine() {
  while(Look == '\n') {
    GetChar();
    SkipWhite();
  }
}

void SkipWhite() {
  while(IsWhite(Look)) {
    GetChar();
  }
}

int IsAlNum(char c) {
  return IsDigit(c) || IsAlpha(c);
}

int IsWhite(char c) {
  return strchr(" \t\r\n", c) != NULL;
}
void GetName() {
  SkipWhite();

  if(!IsAlpha(Look)) {
      Expected("Name");
  }
  Token = 'x';
  int i = 0;
  while(IsAlNum(Look)) {
    Value[i] = UPCASE(Look);
    GetChar();
    i++;
  }
  Value[i] = '\0';
}

void GetNum() {
  SkipWhite();
  if(!IsDigit(Look)) {
    Expected("Integer");
  }
  Token = '#';
  int i = 0;
  while(IsDigit(Look)) {
    Value[i] = Look;
    i++;
    GetChar();
  }
  Value[i] = '\0';
}

void GetOp() {
  SkipWhite();
  Token = Look;
  Value[0] = Look;
  Value[1] = '\0';
  GetChar();
}
void Emit(char* s) {
  printf("\t%s", s);
}

void EmitLn(char* s) {
  Emit(s);
  printf("\n");
}

void Next() {
  SkipWhite();
  if(IsAlpha(Look)) {
    GetName();
  }
  else if(IsDigit(Look)) {
    GetNum();
  }
  else {
    GetOp();
  }
}
void Init() {
  for(int i =0; i <100; i++){
    strcpy(ST[i], "ok");
  }
  GetChar();
  Next();
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

int LookupSymbol(char*s, int n) {
  int i ;
  for(i = n-1; i >= 0; i--) {
    if(strcmp(s, ST[i]) == 0) {
      break;
    }
  }
  return i;
}
int Lookup(const char * const * T, char * s, int n) {
  int i;
  for(i = n - 1; i >= 0; i--) {
    if(strcmp(s, T[i]) == 0) {
      break;
    }
  }
  return  i+1;
}

void Scan() {
  if(Token == 'x') {
    int k = Lookup(KWlist, Value, KWsz);
    Token = KWcode[k]; 
  }
}

void MatchString(char* x) {
  if(strcmp(Value, x) != 0) {
    Expected(x);
  }
  Next();
}

void Duplicate(char* N) {
  sprintf(tmp, "Duplicate Identifier %s", N);
  Abort(tmp); 
}

void CheckIdent() {
  if(Token != 'x') {
    Expected("Identifier");
  }
}

void CheckTable(char* N) {
  if(!InTable(N)) {
    Undefined(N);
  }
}

void CheckDup(char* N) {
  if(InTable(N)) {
    Duplicate(N);
  }
}

void AddEntry(char* N, char T) {
  if(InTable(N)) {
    sprintf(tmp, "Duplicate Identifier %s", N);
    Abort(tmp); 
  }
  if(NEntry == 100) {
    Abort("Symbol Table Full");
  }
  NEntry++;
  strcpy(ST[NEntry], N);
  SType[NEntry] = T;
}
