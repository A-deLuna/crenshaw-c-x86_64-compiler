void GetChar();
void Init();
void Expression();
void Add();
void Subtract();
void Multiply();
void Divide();
void Factor();
void EmitLn(char*);
void Expected(char*);
void Match(char);
void Ident();
void Term();
void Assignment();
int IsAddop(char);
int IsAlpha(char);
char GetNum();
char GetName();
char Look;
char tmp [100];