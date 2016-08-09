void GetChar();
void Init();
void Expression();
void Add();
void Subtract();
void Multiply();
void Divide();
void Factor();
void SignedFactor();
void EmitLn(char*);
void Expected(char*);
void Match(char);
void MatchString(char* );
void Ident();
void Other();
void Term();
void Term1();
void FirstTerm();
void Assignment();
void Block();
void DoProgram();
void Condition();
int IsAddop(char);
int IsAlpha(char);
int IsBoolean(char);
int IsOrOp(char);
int IsRelop(char);
int IsDigit(char);
int IsAlNum(char);
int IsOp(char);
int Lookup(const char * const *, char *, int);
void GetNum();
void Scan();
int GetBoolean();
void GetOp();
void GetName();
void NewLine();
void Input();
void Output();
void SkipWhite();
void BoolExpression();
void BoolTerm();
void BoolOr();
void BoolXor();
void Fin();
void NotFactor();
void BoolFactor();
void Relation();
void Equals();
void NotEquals();
void Less();
void Greater();
void PostLabel(char*);
void DoIf();
void DoWhile();
void DoLoop();
void DoRepeat();
void DoFor();
char* NewLabel();
char Look;
char tmp [100];
char LabelName [100];
int Table[26];
int LCount;
char Value[16];
char Token;
extern const char KWcode[];
extern const char * const KWlist[];
extern int KWlistSz;