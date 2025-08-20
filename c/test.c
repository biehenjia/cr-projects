/* core.c - implementation of C MCR functions.

This file is part of the C MCR Library.
Copyright (C) 1998,1999 Eugene Zima, Vassili Mitrofanov, Vladimir Kislenkov

This software is for educational and research purposes only.

The C MCR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  

Please send your questions about this software to 
Eugene Zima <ezima@daisy.uwaterloo.ca>.

*/
#include<stdlib.h>

#include <math.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER   /* MS DevStudio specific */
#include <malloc.h>
#endif
 
#define NUMBER 1
#define VARIABLE 2
#define PLUS 3
#define B_MINUS 4
#define MUL 5
#define DIV 6
#define POWER 7
#define SINUS  8
#define COSINUS 9
#define TANG 10
#define COTANG 11
#define ARCSIN 12
#define ARCCOS 13
#define ARCTANG 14
#define ARCCOTANG 15
#define EXPONENT 16
#define LN 17
#define LEFT 18
#define RIGHT 19
#define U_MINUS 20

#define PURE_SUM 100
#define PURE_PROD 101
#define TRIG_SIN 102
#define TRIG_COS 103
#define TRIG_TANG 104
#define TRIG_COTANG 105

#define TRIG 94
#define TREE 200
#define PARAMETER 300

#define MAX_DEGREE 25

#define PI 3.14159

#include "core.h"

typedef struct _StackAct
 {
  Node* Act;
  struct _StackAct* Next;
 } StackAct;

typedef struct _StackExpr
 {
  CR* Expression;
  struct _StackExpr* Next;
 } StackExpr;

 struct _StackAct* Action=0;
 struct _StackExpr* Expressions=0;

int Error;

Variable Variables[100];

int Index;

int Count;
long Res_Index;
char c;

char NewVariable;
FILE* res;

double Fact(int i);
double C(int n,int k);
int min(int x,int y);
int max(int x,int y);
int LookExpr2(void);
int LookExpr1(void);
CR* FormDigitCR(double i);
Node* FormDigitExpr(double i);
VarPosition FindVar(char Name);
CR* FormVarCR(char);
Node* FormVarExpr(char);
CR* FormCREx(int);
Node* FormOpExpr(int);
int EmptyAction(void);
void PushExpr(CR*);
void PushAct(Node*);
Node* PopAct(void);
CR* PopExpr(void);
void ClearError();
void StrongClearError();
int MVar(CR*);
int CRType(CR*);
List* CopyLs(List*,int);
ValueList* CopyCRVal(ValueList*,int);
CR* CopyCR(CR*);
Node* Copy(Node* Source,
		  Node* UpSrce,
		  Node* UpRslt);

void DestroyLs(List*,int);
void DestroyCR(CR* Subj);
void Destroy(Node* Subj, Node* UpLevel);
void Speller();
CR* CRSimplify(CR*);
CR* AddCR(CR*, CR*);
CR* MulCR(CR*, CR*);
CR* PowerCR(CR*, CR*);
Node* Addition(Node*,
		      Node*);
Node* Multiply(Node*,
		      Node*);
Node* Power(Node*,Node*);
void Invert(Node*);
CR* ApplyCRRule(int, CR*);
Node* ApplyOperation(Node*,
			    Node*);
CR* InvPowCR(CR*);
void Store(double,double*);
double My_Pow(double, double);
double Value(CR*, int);
double E(CR*, int);
void EvalCR(CR* Chain, int Level, double* Reslt);
double GetValue(CR*);
double GetMValue(CR*);
int Prepare(CR* Subj);
void InvertPow(Node*);
void RegisterVariable(Variable NewVar);
void InitVariable(Variable Init);
void ClearVarInfo();
int Enable(Node*);
double GetDigit(char* Arg);
int GetToken(char*);
CR* Analyse(char* Arg);
void ShowCR(CR* Subj, FILE* outp);

double Fact(int i)
 {
  int Count;
  double Result=1;
  for(Count=1;Count<=i;Count++)
   Result*=Count;
  return Result;
 }

double C(int n,int k)
 {
  return Fact(n)/Fact(k)/Fact(n-k);
 }

int min(int a,int b)
 {
  return ((a>b)?b:a);
 }

int max(int a,int b)
 {
  return ((a>b)?a:b);
 }

int LookExpr2(void)
 {
  if (Expressions)
   return ((*Expressions).Next!=0);
   else return(0);
 }

int LookExpr1(void)
 {
  return (Expressions!=0);
 }

CR* FormDigitCR(double i)
 {
  CR* Temp;
  Temp=(CR*) malloc(sizeof(CR));
  if (Temp)
   {
    (*Temp).Type=NUMBER;
    (*Temp).MainVar=-1;
    (*Temp).Value=i;
    (*Temp).Args=NULL;
    (*Temp).Length=0;
    (*Temp).Values=NULL;
    Error=0;
   }
   else Error=-1;
   return Temp;
 }

Node* FormDigitExpr(double i)
 {
  Node* Temp;
  Temp=(Node*)malloc(sizeof(Node));
  if (Temp)
   {
    (*Temp).Type=1;
    (*Temp).Value=i;
    (*Temp).Variable=' ';
    (*Temp).Sign=1;
    (*Temp).PowSign=1;
    (*Temp).Flag=0;
    (*Temp).Complexity=0;
    (*Temp).Right=Temp;
    (*Temp).Down=Temp;
    (*Temp).ECR=FormDigitCR(i);
    Error=0;
   }
   else Error=-1;
  return Temp;
 }

VarPosition FindVar(char Name)
 {
  int Count;
  int Hit=0;
  VarPosition Temp;
  for(Count=0;Count<Index,Hit==0;Count++)
   if (Variables[Count].Name==Name)
    {
     Temp.Pos=Count;
     Temp.Start=Variables[Count].Start;
     Temp.Step=Variables[Count].Step;
     Hit=1;
    };
  if (Hit==0) Error=-3;
  return Temp;
 }

CR* FormVarCR(char Name)
 {
  CR* Temp;
  VarPosition Place;
  CR* Start;
  CR* Step;
  Temp=(CR*)malloc(sizeof(CR));
  Place=FindVar(Name);
  if (Temp&&(Error==0))
   {
    Place=FindVar(Name);
    Start=FormDigitCR(Place.Start);
    Step=FormDigitCR(Place.Step);
    (*Temp).Type=PURE_SUM;
    (*Temp).MainVar=Place.Pos;
    (*Temp).Value=0;
    (*Temp).Args=(List*)malloc(2*sizeof(CR*));
    (*Temp).Length=2;
    (*Temp).Values=NULL;
    (*((*Temp).Args))[0]=Start;
    (*((*Temp).Args))[1]=Step;
    Temp=CRSimplify(Temp);
   }
  else Error=-1;
  return Temp;
 }

Node* FormVarExpr(char Name)
 {
  Node* Temp;
  Temp=(Node*)malloc(sizeof(Node));
  if (Temp)
   {
    (*Temp).Type=2;
    (*Temp).Value=0;
    (*Temp).Variable=Name;
    (*Temp).Sign=1;
    (*Temp).PowSign=1;
    (*Temp).Flag=0;
    (*Temp).Complexity=1;
    (*Temp).Right=Temp;
    (*Temp).Down=Temp;
    (*Temp).ECR=FormVarCR(Name);
    Error=0;
   }
   else Error=-1;
  return Temp;
 }

CR* FormCREx(int Ex)
 {
  CR* Result;
  Result=(CR*)malloc(sizeof(CR));
  if (Result)
   {
    (*Result).Type=Ex;
    (*Result).MainVar=0;
    (*Result).Value=0;
    (*Result).Args=NULL;
    (*Result).Length=0;
    (*Result).Values=NULL;
   }
  else Error=-1;
  return Result;
 }

Node* FormOpExpr(int Operation)
 {
  Node* Temp;
  Temp=(Node*)malloc(sizeof(Node));
  if (Temp)
   {
    (*Temp).Type=Operation;
    (*Temp).Value=0;
    (*Temp).Variable=' ';
    (*Temp).Sign=1;
    (*Temp).PowSign=1;
    (*Temp).Flag=0;
    (*Temp).Complexity=0;
    (*Temp).Right=Temp;
    (*Temp).Down=Temp;
    (*Temp).ECR=NULL;
    Error=0;
   }
   else Error=-1;
  return Temp;
 }

int EmptyAction(void)
 {
  return (!Action);
 }

void PushExpr(CR* NewExpr)
 {
  StackExpr* Temp;
  Temp=(StackExpr*)malloc(sizeof(StackExpr));
  if (Temp)
   {
    (*Temp).Next=Expressions;
    (*Temp).Expression=NewExpr;
    Expressions=Temp;
   }
   else Error=-1;
 }

void PushAct(Node* NewAct)
 {
  StackAct* Temp;
  Temp=(StackAct*)malloc(sizeof(StackAct));
  if (Temp)
   {
    (*Temp).Next=Action;
    (*Temp).Act=NewAct;
    Action=Temp;
   }
   else Error=-1;
 }

 Node* PopAct(void)
 {
   StackAct* Temp;
   Node* NewAct;
  Temp=Action;
  if (Temp)
   {
    Action=(*Action).Next;
    NewAct=(*Temp).Act;
    free(Temp);
   }
   else
   {
    Error=1;
    NewAct=0;
   };
   return NewAct;
 }

 CR* PopExpr(void)
 {
   StackExpr* Temp;
   CR* NewExpr;
  Temp=Expressions;
  if (Temp)
   {
    Expressions=(*Expressions).Next;
    NewExpr=(*Temp).Expression;
    free(Temp);
   }
   else
   {
    Error=1;
    NewExpr=0;
   };
   return NewExpr;
 }

void ClearError()
 {
  if (Error>0) Error=0;
 }

void StrongClearError()
 {
  Error=0;
 }

int MVar( CR* Subj)
 {
  return (*Subj).MainVar;
 }

int CRType( CR* Subj)
 {
  return (*Subj).Type;
 }

List* CopyLs(List* Source,int Length)
 {
  List* Result;
  int Count;
  Result=NULL;
  if (Source!=NULL)
   {
    Result=(List*)malloc(Length*sizeof( CR*));
    for(Count=0;Count<Length;Count++)
     (*Result)[Count]=CopyCR((*Source)[Count]);
   };
  return Result;
 }

ValueList* CopyCRVal(ValueList* Subj, int Length)
 {
  ValueList* Temp=NULL;
  int Count;
  if (Subj)
   {
    Temp=(ValueList*)malloc(Length*sizeof(double));
    if (Temp)
     {
      for(Count=0;Count<Length;Count++)
       (*Temp)[Count]=(*Subj)[Count];
     }
    else Error=-1;
   }
  return Temp;
 }

 CR* CopyCR( CR* Source)
 {
   CR* Result;
   CR* Temp;
  if (Source!=NULL)
   {
    Temp=( CR*) malloc(sizeof( CR));
    if (Temp)
     {
      (*Temp).Type=(*Source).Type;
      (*Temp).MainVar=(*Source).MainVar;
      (*Temp).Value=(*Source).Value;
      (*Temp).Args=CopyLs((*Source).Args,(*Source).Length);
      (*Temp).Length=(*Source).Length;
      (*Temp).Values=CopyCRVal((*Source).Values,(*Source).Length);
      Error=0;
     }
     else Error=-1;
    Result=Temp;
   }
   else Result=NULL;
  return Result;
 }

 Node* Copy( Node* Source,
		   Node* UpSrce,
		   Node* UpRslt)
 {
   Node* Result;
   Node* Temp;
  Result=( Node*)malloc(sizeof( Node));
  if (Result)
   {
    (*Result).Type=(*Source).Type;
    (*Result).Value=(*Source).Value;
    (*Result).Variable=(*Source).Variable;
    (*Result).Sign=(*Source).Sign;
    (*Result).PowSign=(*Source).PowSign;
    (*Result).Flag=(*Source).Flag;
    (*Result).Complexity=(*Source).Complexity;
    if ((*Source).Down!=Source)
     {
      Temp=Copy((*Source).Down,Source,Result);
      if (Temp)
       {
	(*Result).Down=Temp;
       };
     }
     else (*Result).Down=Result;
    if ((*Source).Right!=Source)
     {
      if ((*Source).Right!=UpSrce)
       {
	Temp=Copy((*Source).Right,UpSrce,UpRslt);
	if (Temp)
	 {
	  (*Result).Right=Temp;
	 };
       }
       else (*Result).Right=UpRslt;
     }
     else (*Result).Right=Result;
    (*Result).ECR=CopyCR((*Source).ECR);
   }
   else Error=-1;
  return Result;
 }

void DestroyLs(List* Subj,int Length)
 {
  int Count;
  if (Subj)
   {
    for(Count=0;Count<Length;Count++)
     DestroyCR((*Subj)[Count]);
    free(Subj);
   };
 }

void DestroyCR( CR* Subj)
 {
  if (Subj)
   {
    DestroyLs((*Subj).Args,(*Subj).Length);
    if ((*Subj).Values!=NULL)
     free(*((*Subj).Values));
    free(Subj);
   };
 }

void Destroy( Node* Subj, Node* UpLevel)
 {
   Node* Temp;
  Temp=Subj;
  DestroyCR((*Subj).ECR);
  if ((*Temp).Down!=Temp) Destroy((*Temp).Down,Temp);
  if ((*Temp).Right!=UpLevel) Destroy((*Temp).Right,UpLevel);
  free(Temp);
 }

void Speller()
 {
  if (Error==0 ) printf("Ok\n");
  if (Error==-1) printf("Out of memory.\n");
  if (Error==-2) printf("Out of environment space.\n");
  if (Error==-3) printf("No such variable registered.\n");
  if (Error==-4) printf("Error in parsing. Check input expression.\n");
  if (Error==-5) printf("Division by zero.\n");
  if (Error==-6) printf("Unknown error found.\n");
  if (Error==-7)
   {
    printf("Error attempting evaluate ctg.\n");
    printf("Possible error: argument+Pi*n\n");
   };
  if (Error==-8)
   {
    printf("Error attempting evaluate arcsin.\n");
    printf("Possible error: argument out of range.\n");
   };
  if (Error==-9)
   {
    printf("Error attempting evaluate arccos.\n");
    printf("Possible error: argument out of range.\n");
   };
  if (Error==-10)
   {
    printf("Error attempting evaluate ln.\n");
    printf("Possible error: argument<0.\n");
   };
 }

int IsZero( CR* Subj)
 {
  int Result;
  Result=0;
  if ((CRType(Subj)==NUMBER)&&((*Subj).Value==0)) Result=1;
  return Result;
 }

 CR* CRSimplify( CR* Subj)

 {
   CR* Temp;
  int Count1;
  int Hit=-1;
   CR* Result;
  switch ((*Subj).Type)
   {
    case PURE_SUM :
		 while (((*(*(*Subj).Args)[(*Subj).Length-1]).Type==NUMBER)
			&&((*(*(*Subj).Args)[(*Subj).Length-1]).Value==0)
			&&((*Subj).Length>1))
		  {
		   DestroyCR((*(*Subj).Args)[(*Subj).Length-1]);
		   (*Subj).Length-=1;
		   (*Subj).Args=(List*)realloc((*Subj).Args,((*Subj).Length)*sizeof( CR*));
		  };
		 if ((*Subj).Length==1)
		  {
		   Result=CopyCR((*(*Subj).Args)[0]);
		   DestroyCR(Subj);
		  }
		 else Result=Subj;
		 break;
    case PURE_PROD:
		 while (((*(*(*Subj).Args)[(*Subj).Length-1]).Type==NUMBER)
			&&((*(*(*Subj).Args)[(*Subj).Length-1]).Value==0)
			&&((*Subj).Length>1))
		  {
		   DestroyCR((*(*Subj).Args)[(*Subj).Length-1]);
		   (*Subj).Length-=1;
		   (*Subj).Args=(List*)realloc((*Subj).Args,((*Subj).Length)*sizeof( CR*));
		  };
		 if ((*Subj).Length==1)
		  {
		   Temp=CopyCR((*(*Subj).Args)[0]);
		   DestroyCR(Subj);
		  }
		 else Temp=Subj;
		 for(Count1=0;Count1<(*Temp).Length;Count1++)
		  {
		   if (CRType((*(*Temp).Args)[Count1])==NUMBER)
		    if ((*(*(*Temp).Args)[Count1]).Value==0)
		     Hit=Count1;
		   };
		 if (Hit==0)
		  {
		   DestroyCR(Temp);
		   Result=FormDigitCR(0);
		  }
		 else
		 if (Hit>0)
		  {
		   for(Count1=Hit+1;Count1<(*Temp).Length;Count1++)
		    {
		     DestroyCR((*(*Temp).Args)[Count1]);
		    };
		   (*Temp).Args=(List*)realloc((*Temp).Args,(Hit+1)*sizeof( CR*));
		   (*Temp).Length=Hit+1;
		   Result=Temp;
		  }
		 else Result=Temp;
		 break;
    case TRIG_COS    :
    case TRIG_SIN    :
		 Count1=(*Subj).Length/2;
		 if (IsZero((*(*Subj).Args)[0])&&IsZero((*(*Subj).Args)[Count1]))
		  {
		   DestroyCR(Subj);
		   Result=FormDigitCR(0);
		  }
    /*case TRIG_TANG   :
      case TRIG_COTANG :
      ��� ���� �� �।�ᬮ���� ᫥���饥 : �᫨ 1 - � � 3 - � ��㬥��� -
      �㫨, � � �� ��ࠦ���� - ���⮩ ���.
		 break;*/
    default          :
		 Result=Subj;
		 break;
   };
  return Result;
 }

int ESPS( CR* Source,  CR* Example)
 {
  int Res;
  int i;
  Res=-1;
  for(i=0;i<(*Source).Length;i++)
   {
    if ((CRType((*(*Source).Args)[i])==PURE_SUM)&&
	(MVar((*(*Source).Args)[i])==MVar(Example))&&(Res==-1)) Res=i;
   };
  return Res;
 }

int ESPP( CR* Source,  CR* Example)
 {
  int Res;
  int i;
  Res=-1;
  for(i=0;i<(*Source).Length;i++)
   {
    if ((CRType((*(*Source).Args)[i])==PURE_PROD)&&
	(MVar((*(*Source).Args)[i])==MVar(Example))&&(Res==-1)) Res=i;
   };
  return Res;
 }

int EST( CR* Source,  CR* Example)
 {
  int Res;
  int i;
  Res=-1;
  for(i=0;i<(*Source).Length;i++)
   {
    if ((CRType((*(*Source).Args)[i])>=102)&&(CRType((*(*Source).Args)[i])<=103)&&
	(MVar((*(*Source).Args)[i])==MVar(Example))&&(Res==-1)) Res=i;
   };
  return Res;
 }

int EN( CR* Source)
 {
  int Res;
  int i;
  Res=-1;
  for(i=0;i<(*Source).Length;i++)
   {
    if ((CRType((*(*Source).Args)[i])==NUMBER)&&(Res==-1)) Res=i;
   };
  return Res;
 }

 CR* AddCR( CR* First,  CR* Second)


 {
   CR* Result;
   CR* Temp;
   CR* Temp1;
  List* LTemp;
  double i;
  int Pos1,Pos2;
  int Count1;
  if (MVar(First)!=MVar(Second))
   {
    if ((CRType(First)==NUMBER)&&(CRType(Second)==PURE_SUM))
     {
      Result=CopyCR(Second);
      Temp=(*((*Result).Args))[0];
      Temp1=AddCR(First,Temp);
      DestroyCR((*((*Result).Args))[0]);
      (*((*Result).Args))[0]=Temp1;
     }
    else
    if ((CRType(First)==PURE_SUM)&&(CRType(Second)==NUMBER))
     {
      Result=AddCR(Second,First);
     }
    else

    if ((MVar(First)>MVar(Second))&&
	(CRType(First)==PURE_SUM)&&(CRType(Second)==PURE_SUM))
     {
      Result=CopyCR(First);
      Temp=(*((*Result).Args))[0];
      Temp1=AddCR(Second,Temp);
      DestroyCR((*((*Result).Args))[0]);
      (*((*Result).Args))[0]=Temp1;
     }
    else
    if ((MVar(First)<MVar(Second))&&
	(CRType(First)==PURE_SUM)&&(CRType(Second)==PURE_SUM))
     {
      Result=AddCR(Second,First);
     }
    else
    if ((CRType(First)==PLUS)&&(CRType(Second)==PURE_SUM))
    {
     Pos1=ESPS(First,Second);
     Pos2=EN(First);
     if (Pos1>-1)
      {
       Temp=AddCR((*(*First).Args)[Pos1],Second);
       DestroyCR((*(*First).Args)[Pos1]);
       (*(*First).Args)[Pos1]=Temp;
       Result=CopyCR(First);
      }
     else
     if (Pos2>-1)
      {
       Temp=AddCR((*(*First).Args)[Pos2],Second);
       DestroyCR((*(*First).Args)[Pos2]);
       (*(*First).Args)[Pos2]=Temp;
       Result=CopyCR(First);
      }
     else
      {
       Result=CopyCR(First);
       Temp=CopyCR(Second);
       if (Result&&Temp)
	{
	 (*Result).Args=(List*)realloc((*Result).Args,
	 sizeof( CR*)*((*Result).Length+1));
	 (*(*Result).Args)[(*Result).Length]=Temp;
	 (*Result).Length+=1;
	}
       else Error=-1;
      };
    }
    else
    if ((CRType(First)==PURE_SUM)&&(CRType(Second)==PLUS))
     Result=AddCR(Second,First);
    else
     {
      Result=FormCREx(PLUS);
      LTemp=(List*)malloc(2*sizeof( CR*));
      if (Result&&LTemp)
       {
	(*Result).Length=2;
	if (MVar(First)<MVar(Second))
	 {
	  (*Result).MainVar=MVar(Second);
	  (*LTemp)[0]=CopyCR(First);
	  (*LTemp)[1]=CopyCR(Second);
	  (*Result).Args=LTemp;
	 }
	else
	 {
	  (*Result).MainVar=MVar(First);
	  (*LTemp)[0]=CopyCR(Second);
	  (*LTemp)[1]=CopyCR(First);
	  (*Result).Args=LTemp;
	 }
       }
      else Error=-1;
     };
   }
  else
   {
    if ((CRType(First)==NUMBER)&&(CRType(Second)==NUMBER))
     {
      i=(*First).Value+(*Second).Value;
      Result=FormDigitCR(i);
     }
    else
    if ((CRType(First)==PLUS)&&(CRType(Second)==PURE_SUM))
    {
     Pos1=ESPS(First,Second);
     Pos2=EN(First);
     if (Pos1>-1)
      {
       Temp=AddCR((*(*First).Args)[Pos1],Second);
       DestroyCR((*(*First).Args)[Pos1]);
       (*(*First).Args)[Pos1]=Temp;
       Result=CopyCR(First);
      }
     else
     if (Pos2>-1)
      {
       Temp=AddCR((*(*First).Args)[Pos2],Second);
       DestroyCR((*(*First).Args)[Pos2]);
       (*(*First).Args)[Pos2]=Temp;
       Result=CopyCR(First);
      }
     else
      {
	Result=CopyCR(First);
	Temp=CopyCR(Second);
	if (Result&&Temp)
	 {
	  (*Result).Args=(List*)realloc((*Result).Args,
	  sizeof( CR*)*((*Result).Length+1));
	  (*(*Result).Args)[(*Result).Length]=Temp;;
	  (*Result).Length+=1;
	 }
	else Error=-1;
      };
    }
    else
    if ((CRType(First)==PURE_SUM)&&(CRType(Second)==PLUS))
     Result=AddCR(Second,First);
    else
    if ((CRType(First)==PURE_SUM)&&(CRType(Second)==PURE_SUM))
     {
      if ((*First).Length>=(*Second).Length)
       {
	Result=CopyCR(First);
	for(Count1=0;Count1<(*Second).Length;Count1++)
	 {
	  Temp1=AddCR((*((*First).Args))[Count1],(*((*Second).Args))[Count1]);
	  DestroyCR((*((*Result).Args))[Count1]);
	  (*((*Result).Args))[Count1]=Temp1;
	 };
       }
      else
       {
	Result=AddCR(Second,First);
       };
     }
    else
     {
      Result=FormCREx(PLUS);
      LTemp=(List*)malloc(2*sizeof( CR*));
      if (Result&&LTemp)
       {
	(*Result).Length=2;
	(*Result).MainVar=MVar(Second);
	(*LTemp)[0]=CopyCR(First);
	(*LTemp)[1]=CopyCR(Second);
	(*Result).Args=LTemp;
       }
      else Error=-1;
     };
   };
  if (Result) Result=CRSimplify(Result);
  return Result;
 }

 Node* Addition( Node* First,
		       Node* Second)
 {
   Node* FstCpy;
   Node* ScndCpy;
   Node* Result;
   Node* Temp;
   CR* CRRes;
  int CommSign;
  int Done;
  Done=0;
  FstCpy=Copy(First,First,0);
  ScndCpy=Copy(Second,Second,0);
  CRRes=AddCR((*First).ECR,(*Second).ECR);
  Result=0;
  if (FstCpy&&ScndCpy&&CRRes)
   {
    if (((*FstCpy).Type==PLUS)&&((*ScndCpy).Type==PLUS))
     {
      CommSign=(*FstCpy).Sign*(*ScndCpy).Sign;
      Temp=(*ScndCpy).Down;
      while ((*Temp).Right!=ScndCpy)
       {
	(*Temp).Sign=(*Temp).Sign*CommSign;
	Temp=(*Temp).Right;
       };
      (*Temp).Sign=(*Temp).Sign*CommSign;
      (*Temp).Right=(*FstCpy).Down;
      (*FstCpy).Down=(*ScndCpy).Down;
      (*FstCpy).Complexity+=(*ScndCpy).Complexity;
      free(ScndCpy);
      Result=FstCpy;
      Done=1;
     };
    if (((*ScndCpy).Type!=PLUS)&&((*FstCpy).Type==PLUS)&&(!Done))
     {
      CommSign=(*FstCpy).Sign*(*ScndCpy).Sign;
      (*ScndCpy).Sign=(*ScndCpy).Sign*CommSign;
      (*ScndCpy).Right=(*FstCpy).Down;
      (*FstCpy).Down=ScndCpy;
      (*FstCpy).Complexity+=(*ScndCpy).Complexity;
      Result=FstCpy;
     };
    if (((*FstCpy).Type!=PLUS)&&((*ScndCpy).Type==PLUS)&&(!Done))
     {
      CommSign=(*FstCpy).Sign*(*ScndCpy).Sign;
      (*FstCpy).Sign=(*FstCpy).Sign*CommSign;
      (*FstCpy).Right=(*ScndCpy).Down;
      (*ScndCpy).Down=FstCpy;
      (*ScndCpy).Complexity+=(*FstCpy).Complexity;
      Result=ScndCpy;
     };
    if (((*FstCpy).Type!=PLUS)&&((*ScndCpy).Type!=PLUS)&&(!Done))
     {
      Temp=( Node*)malloc(sizeof( Node));
      if (Temp)
       {
	(*Temp).Type=PLUS;
	(*Temp).Value=0;
	(*Temp).Variable=' ';
	(*Temp).Sign=1;
	(*Temp).PowSign=1;
	(*Temp).Flag=0;
	(*Temp).Right=Temp;
	(*Temp).Down=FstCpy;
	(*Temp).Complexity=(*FstCpy).Complexity+(*ScndCpy).Complexity;
	(*FstCpy).Right=ScndCpy;
	(*ScndCpy).Right=Temp;
	(*Temp).ECR=NULL;
       }
       else Error=-1;
      Result=Temp;
     };
    /*Start=Result;
    SFirst=(*Start).Down;
    SSecond=(*SFirst).Right;
    while ((*SFirst).Right!=Result)
     {
      while ((*SFirst).Complexity>(*SSecond).Complexity)
       {
	MStart=Start;
	if (MStart==Result)
	 (*MStart).Down=SSecond;
	 else (*MStart).Right=SSecond;
	Temp=(*SSecond).Right;
	(*SSecond).Right=SFirst;
	(*SFirst).Right=Temp;
	if (MStart==Start)
	 MStart=(*MStart).Down;
	 else MStart=(*MStart).Right;
	SFirst=(*MStart).Right;
	SSecond=(*SFirst).Right;
       };
      if (Start==Result)
       Start=(*Start).Down;
       else Start=(*Start).Right;
      SFirst=(*Start).Right;
      SSecond=(*SFirst).Right;
     };
    if ((*Result).Complexity==0)
     {
      AddRes=0;
      Temp=(*Result).Down;
      while ((Temp!=Result)&&(Error==0))
       {
	(*Temp).Value*=(*Temp).Sign;
	if (((*Temp).Value!=0)&&((*Temp).PowSign==-1))
	 (*Temp).Value=1/(*Temp).Value;
	 else
	  if (((*Temp).Value==0)&&((*Temp).PowSign==-1))
	   Error=-5;
	AddRes+=(*Temp).Value;
	Temp=(*Temp).Right;
       };
      if (Error==0)
       {
	Destroy(Result,Result);
	Result=FormDigitExpr(AddRes);
       };
     };
    if ((*Result).Type==PLUS)
     {
      SFirst=(*Result).Down;
      SSecond=(*SFirst).Right;
      while (((*SFirst).Complexity==0)&&((*SSecond).Complexity==0)&&(Error==0))
       {
	(*SSecond).Value*=(*SSecond).Sign;
	if (((*SSecond).Value!=0)&&((*SSecond).PowSign==-1))
	 (*SSecond).Value=1/(*SSecond).Value;
	 else
	  if (((*SSecond).Value==0)&&((*SSecond).PowSign==-1))
	   Error=-5;
	(*SFirst).Value+=(*SSecond).Value;
	Temp=(*SSecond).Right;
	if (Error==0) Destroy(SSecond,SSecond);
	SSecond=Temp;
       };
      (*SFirst).Right=SSecond;
     };*/
    (*Result).ECR=CRRes;
   }
   else Error=-1;
  return Result;
 }

 CR* CorrectP( CR* Arg, int NLength)
 {
   CR* Temp;
   CR* Result;
  List* LTemp;
  int Count;
  Result=CopyCR(Arg);
  Temp=FormDigitCR(1);
  LTemp=(List*)malloc(sizeof( CR*)*NLength);

  for(Count=0;Count<(*Result).Length;Count++)
   (*LTemp)[Count]=(*(*Result).Args)[Count];

  for(Count=(*Result).Length;Count<NLength;Count++)
   (*LTemp)[Count]=CopyCR(Temp);

  free((*Result).Args);

  (*Result).Args=LTemp;

  (*Result).Length=NLength;

  Result=CRSimplify(Result);

  return Result;
 }

 CR* CorrectT( CR* Arg, int NLength)
 {
   CR* Temp1;
   CR* Temp2;
   CR* Result;
  List* LTemp;
  int Count,L;
  Result=CopyCR(Arg);
  Temp1=FormDigitCR(0);
  Temp2=FormDigitCR(1);
  L=(*Result).Length;

  LTemp=(List*)malloc(sizeof( CR*)*NLength*2);
  
  for(Count=0;Count<L/2;Count++)
   {
    (*LTemp)[Count]=(*(*Result).Args)[Count];
    (*LTemp)[Count+NLength]=(*(*Result).Args)[Count+L/2];
   }
  for(Count=L/2;Count<NLength;Count++)
   {
    (*LTemp)[Count]=CopyCR(Temp1);
    (*LTemp)[Count+NLength]=CopyCR(Temp2);
   };
  free((*Result).Args);
  (*Result).Args=LTemp;
  (*Result).Length=NLength*2;
  Result=CRSimplify(Result);
  return Result;
 }

 CR* MulCR( CR* First,  CR* Second)
 {
   CR* Result;
   CR* Temp;
   CR* Temp1;
   CR* Temp2;
   CR* Temp3;
  List* LTemp;
  double i;
  int Count;
  int Count1, Count2;
  int Bound11, Bound12;
  int Bound21, Bound22;
  int m,n;
  int Pos1,Pos2,Pos3;
  int L,L1,LA;
   CR* s1;
   CR* s2;
  Pos1=0;
  if (MVar(First)!=MVar(Second))
   {
    if ((CRType(First)==NUMBER)&&(CRType(Second)==PURE_PROD))
     {
      Result=CopyCR(Second);
      Temp=(*((*Result).Args))[0];
      Temp1=MulCR(First,Temp);
      DestroyCR((*((*Result).Args))[0]);
      (*((*Result).Args))[0]=Temp1;
     }
    else
    if ((CRType(First)==PURE_PROD)&&(CRType(Second)==NUMBER))
     {
      Result=MulCR(Second,First);
     }
    else
    if ((CRType(First)<=103)&&(CRType(First)>=102)&&(CRType(Second)==NUMBER))
     {
      Result=CopyCR(First);
      L=(*Result).Length;
      if (Result)
       {
	Temp=(*(*Result).Args)[0];
	Temp1=MulCR(Temp,Second);
	DestroyCR((*(*Result).Args)[0]);
	(*(*Result).Args)[0]=Temp1;
	Temp=(*(*Result).Args)[L/2];
	Temp1=MulCR(Temp,Second);
	DestroyCR((*(*Result).Args)[L/2]);
	(*(*Result).Args)[L/2]=Temp1;
       }
      else Error=-1;
     }
    else
    if ((CRType(First)==NUMBER)&&(CRType(Second)<=103)&&(CRType(Second)>=102))
     {
      Result=MulCR(Second,First);
     }
    else
    if ((CRType(First)<=103)&&(CRType(First)>=102)&&(CRType(Second)>=102)&&(CRType(Second)<=103))
     {
      if (MVar(First)>MVar(Second))
       {
	Result=CopyCR(First);
	L=(*Result).Length;
	if (Result)
	 {
	  Temp=(*(*Result).Args)[0];
	  Temp1=MulCR(Temp,Second);
	  DestroyCR((*(*Result).Args)[0]);
	  (*(*Result).Args)[0]=Temp1;
	  Temp=(*(*Result).Args)[L/2];
	  Temp1=MulCR(Temp,Second);
	  DestroyCR((*(*Result).Args)[L/2]);
	  (*(*Result).Args)[L/2]=Temp1;
	 }
	else Error=-1;
       }
      else
       {
	Result=CopyCR(Second);
	L=(*Result).Length;
	if (Result)
	 {
	  Temp=(*(*Result).Args)[0];
	  Temp1=MulCR(Temp,First);
	  DestroyCR((*(*Result).Args)[0]);
	  (*(*Result).Args)[0]=Temp1;
	  Temp=(*(*Result).Args)[L/2];
	  Temp1=MulCR(Temp,First);
	  DestroyCR((*(*Result).Args)[L/2]);
	  (*(*Result).Args)[L/2]=Temp1;
	 }
	else Error=-1;
       }
     }
    else
    if ((CRType(First)==PURE_SUM)&&(CRType(Second)<=103)&&(CRType(Second)>=102))
     {
      if (MVar(First)>MVar(Second))
       {
	Result=CopyCR(First);
	if (Result)
	 {
	  for(Count=0;Count<(*Result).Length;Count++)
	   {
	    Temp=(*(*Result).Args)[Count];
	    Temp1=MulCR(Temp,Second);
	    DestroyCR((*(*Result).Args)[Count]);
	    (*(*Result).Args)[Count]=Temp1;
	   };
	 }
	else Error=-1;
       }
      else
       {
	Result=CopyCR(Second);
	L=(*Result).Length;
	if (Result)
	 {
	  Temp=(*(*Result).Args)[0];
	  Temp1=MulCR(Temp,First);
	  DestroyCR((*(*Result).Args)[0]);
	  (*(*Result).Args)[0]=Temp1;
	  Temp=(*(*Result).Args)[L/2];
	  Temp1=MulCR(Temp,First);
	  DestroyCR((*(*Result).Args)[L/2]);
	  (*(*Result).Args)[L/2]=Temp1;
	 }
	else Error=-1;
       };
     }
    else
    if ((CRType(Second)==PURE_SUM)&&(CRType(First)<=103)&&(CRType(First)>=102))
     {
      Result=MulCR(Second,First);
     }
    else
    if ((CRType(First)==PURE_PROD)&&(CRType(Second)<=103)&&(CRType(Second)>=102))
     {
      if (MVar(First)>MVar(Second))
       {
	Result=CopyCR(First);
	if (Result)
	 {
	  Temp=(*(*Result).Args)[0];
	  Temp1=MulCR(Temp,Second);
	  DestroyCR((*(*Result).Args)[0]);
	  (*(*Result).Args)[0]=Temp1;
	 }
	else Error=-1;
       }
      else
       {
	Result=CopyCR(Second);
	L=(*Result).Length;
	if (Result)
	 {
	  Temp=(*(*Result).Args)[0];
	  Temp1=MulCR(Temp,First);
	  DestroyCR((*(*Result).Args)[0]);
	  (*(*Result).Args)[0]=Temp1;
	  Temp=(*(*Result).Args)[L/2];
	  Temp1=MulCR(Temp,First);
	  DestroyCR((*(*Result).Args)[L/2]);
	  (*(*Result).Args)[L/2]=Temp1;
	 }
	else Error=-1;
       };
     }
    else
    if ((CRType(Second)==PURE_PROD)&&(CRType(First)<=103)&&(CRType(First)>=102))
     {
      Result=MulCR(Second,First);
     }
    else
    if ((CRType(First)==NUMBER)&&(CRType(Second)==PURE_SUM))
     {
      Result=CopyCR(Second);
      for(Count=0;Count<(*Second).Length;Count++)
       {
	Temp=(*((*Result).Args))[Count];
	Temp1=MulCR(First,Temp);
	DestroyCR((*((*Result).Args))[Count]);
	(*((*Result).Args))[Count]=Temp1;
       };
     }
    else
    if ((CRType(First)==PURE_SUM)&&(CRType(Second)==NUMBER))
     {
      Result=MulCR(Second,First);
     }
    else
    if ((CRType(First)==PURE_PROD)&&(CRType(Second)==PURE_PROD))
     {
      if (MVar(First)>MVar(Second))
       {
	Result=CopyCR(First);
	Temp=(*((*Result).Args))[0];
	Temp1=MulCR(Second,Temp);
	DestroyCR((*((*Result).Args))[0]);
	(*((*Result).Args))[0]=Temp1;
       }
      else
       {
	Result=MulCR(Second,First);
       };
     }
    else
    if ((CRType(First)==PURE_SUM)&&(CRType(Second)==PURE_SUM))
     {
      if (MVar(First)>MVar(Second))
       {
	Result=CopyCR(First);
	for(Count=0;Count<(*Result).Length;Count++)
	 {
	  Temp=(*((*Result).Args))[Count];
	  Temp1=MulCR(Second,Temp);
	  DestroyCR((*((*Result).Args))[Count]);
	  (*((*Result).Args))[Count]=Temp1;
	 };
       }
      else
       {
	Result=MulCR(Second,First);
       };
     }
    else
    if ((MVar(First)>MVar(Second))
      &&( ((CRType(First)==PURE_SUM)||(CRType(First)==PURE_PROD)) 
      &&((CRType(Second)==PURE_SUM) ||(CRType(Second)==PURE_PROD)) ) )
     {
      if (CRType(First)==PURE_PROD)
       {
	Result=CopyCR(First);
	Temp=(*((*Result).Args))[0];
	Temp1=MulCR(Temp,Second);
	DestroyCR((*((*Result).Args))[0]);
	(*((*Result).Args))[0]=Temp1;
       }
      else
       {
	Result=CopyCR(First);
	if (Result)
	 {
	  for(Count=0;Count<(*Result).Length;Count++)
	   {
	    Temp=(*(*Result).Args)[Count];
	    Temp1=MulCR(Temp,Second);
	    DestroyCR((*(*Result).Args)[Count]);
	    (*(*Result).Args)[Count]=Temp1;
	   };
	 }
	else Error=-1;
       };
     }
    else
    if ((MVar(First)<MVar(Second))&&(((CRType(First)==PURE_SUM)
	 ||(CRType(First)==PURE_PROD))&&((CRType(Second)==PURE_SUM)
	 ||(CRType(Second)==PURE_PROD))))
     {
      Result=MulCR(Second,First);
     }
    else
    if ((CRType(First)==MUL)&&(CRType(Second)==PURE_PROD))
    {
     if (MVar(First)>MVar(Second))
      {
       Pos1=ESPP(First,Second);
       Pos2=EST(First,Second);
       Pos3=EN(First);
       if (Pos1>-1)
	{
	 Temp=MulCR((*(*First).Args)[Pos1],Second);
	 DestroyCR((*(*First).Args)[Pos1]);
	 (*(*First).Args)[Pos1]=Temp;
	 Result=CopyCR(First);
	}
       else
       if (Pos2>-1)
	{
	 Temp=MulCR((*(*First).Args)[Pos2],Second);
	 DestroyCR((*(*First).Args)[Pos2]);
	 (*(*First).Args)[Pos2]=Temp;
	 Result=CopyCR(First);
	}
       else
       if (Pos3>-1)
	{
	 Temp=MulCR((*(*First).Args)[Pos3],Second);
	 DestroyCR((*(*First).Args)[Pos3]);
	 (*(*First).Args)[Pos3]=Temp;
	 Result=CopyCR(First);
	}
       else
	{
	 Result=CopyCR(First);
	 Temp=CopyCR(Second);
	 if (Result&&Temp)
	  {
	   (*Result).Args=(List*)realloc((*Result).Args,
	   sizeof( CR*)*((*Result).Length+1));
	   (*(*Result).Args)[(*Result).Length]=Temp;
	   (*Result).Length+=1;
	  }
	 else Error=-1;
	};
      }
     else
      {
       Result=CopyCR(Second);
       Temp=(*(*Result).Args)[0];
       Temp1=MulCR(Temp,First);
       DestroyCR((*(*Result).Args)[0]);
       (*(*Result).Args)[0]=Temp1;
      }
    }
    else
    if ((CRType(First)==PURE_PROD)&&(CRType(Second)==MUL))
     Result=MulCR(Second,First);
    else
    if ((CRType(First)==MUL)&&(CRType(Second)==PURE_SUM))
    {
     if (MVar(First)>=MVar(Second))
      {
       Pos1=ESPS(First,Second);
       Pos2=EN(First);
       if (Pos1>-1)
	{
	 Temp=MulCR((*(*First).Args)[Pos1],Second);
	 DestroyCR((*(*First).Args)[Pos1]);
	 (*(*First).Args)[Pos1]=Temp;
	 Result=CopyCR(First);
	}
       else
       if (Pos2>-1)
	{
	 Temp=MulCR((*(*First).Args)[Pos2],Second);
	 DestroyCR((*(*First).Args)[Pos2]);
	 (*(*First).Args)[Pos2]=Temp;
	 Result=CopyCR(First);
	}
       else
	{
	 Result=CopyCR(First);
	 Temp=CopyCR(Second);
	 if (Result&&Temp)
	  {
	   (*Result).Args=(List*)realloc((*Result).Args,
	   sizeof( CR*)*((*Result).Length+1));
	   (*(*Result).Args)[(*Result).Length]=Temp;
	   (*Result).Length+=1;
	  }
	 else Error=-1;
	};
      }
     else
      {
       Result=CopyCR(Second);
       if (Result)
	for(Count=0;Count<(*Result).Length;Count++)
	 {
	  Temp=MulCR((*(*Result).Args)[Count],First);
	  DestroyCR((*(*Result).Args)[Count]);
	  (*(*Result).Args)[Count]=Temp;
	 }
       else Error=-1;
      };
    }
    else
    if ((CRType(First)==PURE_SUM)&&(CRType(Second)==MUL))
     Result=MulCR(Second,First);
    else
    if ((CRType(First)==MUL)&&(CRType(Second)==NUMBER))
     {
      Result=CopyCR(First);
      if (Result)
       {
	Temp=MulCR((*(*First).Args)[0],Second);
	DestroyCR((*(*Result).Args)[0]);
	(*(*Result).Args)[0]=Temp;
       }
      else Error=-1;
     }
    else
    if ((CRType(Second)==NUMBER)&&(CRType(First)==MUL))
     Result=MulCR(Second,First);
    else
    if ((CRType(First)==MUL)&&(CRType(Second)>=102)&&(CRType(Second)<=103))
     {
      Result=CopyCR(First);
      Temp=CopyCR(Second);
       if (Result&&Temp)
	{
	 (*Result).Args=(List*)realloc((*Result).Args,
	 sizeof( CR*)*((*Result).Length+1));
	 (*(*Result).Args)[(*Result).Length]=Temp;
	 (*Result).Length+=1;
	}
       else Error=-1;
     }
    else
    if ((CRType(Second)==MUL)&&(CRType(First)>=102)&&(CRType(First)<=103))
     Result=MulCR(Second,First);
    else
    if ((CRType(First)==DIV)&&(CRType(Second)==DIV))
     {
      if (MVar(First)>MVar(Second))
       {
	Result=CopyCR(First);
	Temp1=Second;
       }
      else
       {
	Result=CopyCR(Second);
	Temp1=First;
       };
      if (Result)
       for(Count=0;Count<(*Result).Length;Count++)
	{
	 Temp2=MulCR((*(*Result).Args)[Count],(*(*Temp1).Args)[Count]);
	 DestroyCR((*(*Result).Args)[Count]);
	 (*(*Result).Args)[Count]=Temp2;
	}
      else Error=-1;
     }
    else
    if (CRType(First)==DIV)
     {
      Result=CopyCR(First);
      if (Result)
       {
	Temp1=MulCR((*(*Result).Args)[0],Second);
	DestroyCR((*(*Result).Args)[0]);
	(*(*Result).Args)[0]=Temp1;
	(*Result).MainVar=max(MVar(First),MVar(Second));
       }
      else Error=-1;
     }
    else
    if (CRType(Second)==DIV)
     Result=MulCR(Second,First);
    else
     {
      Result=FormCREx(MUL);
      LTemp=(List*)malloc(2*sizeof( CR*));
      if (Result&&LTemp)
       {
	(*Result).Length=2;
	if (MVar(First)<MVar(Second))
	 {
	  (*Result).MainVar=MVar(Second);
	  (*LTemp)[0]=CopyCR(First);
	  (*LTemp)[1]=CopyCR(Second);
	  (*Result).Args=LTemp;
	 }
	else
	 {
	  (*Result).MainVar=MVar(First);
	  (*LTemp)[0]=CopyCR(Second);
	  (*LTemp)[1]=CopyCR(First);
	  (*Result).Args=LTemp;
	 }
       }
      else Error=-1;
     };
   }
  else
   {
    if ((CRType(First)==NUMBER)&&(CRType(Second)==NUMBER))
     {
      i=(*First).Value*(*Second).Value;
      Result=FormDigitCR(i);
     }
    else
    if ((CRType(First)==MUL)&&(CRType(Second)==PURE_PROD))
    {
     Pos1=ESPP(First,Second);
     Pos2=EST(First,Second);
     Pos3=EN(First);
     if (Pos1>-1)
      {
       Temp=MulCR((*(*First).Args)[Pos1],Second);
       DestroyCR((*(*First).Args)[Pos1]);
       (*(*First).Args)[Pos1]=Temp;
       Result=CopyCR(First);
      }
     else
     if (Pos2>-1)
      {
       Temp=MulCR((*(*First).Args)[Pos2],Second);
       DestroyCR((*(*First).Args)[Pos2]);
       (*(*First).Args)[Pos2]=Temp;
       Result=CopyCR(First);
      }
     if (Pos3>-1)
      {
       Temp=MulCR((*(*First).Args)[Pos3],Second);
       DestroyCR((*(*First).Args)[Pos3]);
       (*(*First).Args)[Pos3]=Temp;
       Result=CopyCR(First);
      }
     else
      {
       Result=CopyCR(First);
       Temp=CopyCR(Second);
       if (Result&&Temp)
	{
	 (*Result).Args=(List*)realloc((*Result).Args,
	 sizeof( CR*)*((*Result).Length+1));
	 (*(*Result).Args)[(*Result).Length]=Temp;
	 (*Result).Length+=1;
	}
       else Error=-1;
      };
    }
    else
    if ((CRType(First)==PURE_PROD)&&(CRType(Second)==MUL))
     Result=MulCR(Second,First);
    else
    if ((CRType(First)==MUL)&&(CRType(Second)==PURE_SUM))
    {
     Pos1=ESPS(First,Second);
     Pos2=EN(First);
     if (Pos1>-1)
      {
       Temp=MulCR((*(*First).Args)[Pos1],Second);
       DestroyCR((*(*First).Args)[Pos1]);
       (*(*First).Args)[Pos1]=Temp;
       Result=CopyCR(First);
      }
     else
     if (Pos2>-1)
      {
       Temp=MulCR((*(*First).Args)[Pos2],Second);
       DestroyCR((*(*First).Args)[Pos2]);
       (*(*First).Args)[Pos2]=Temp;
       Result=CopyCR(First);
      }
     else
      {
       Result=CopyCR(First);
       Temp=CopyCR(Second);
       if (Result&&Temp)
	{
	 (*Result).Args=(List*)realloc((*Result).Args,
	 sizeof( CR*)*((*Result).Length+1));
	 (*(*Result).Args)[(*Result).Length]=Temp;
	 (*Result).Length+=1;
	}
       else Error=-1;
      };
    }
    else
    if ((CRType(First)==PURE_SUM)&&(CRType(Second)==MUL))
     Result=MulCR(Second,First);
    else
    if ((CRType(First)==MUL)&&(CRType(Second)==NUMBER))
     {
      Result=CopyCR(First);
      if (Result)
       {
	for(Count=0;Count<(*First).Length;Count++)
	{
	 Temp=MulCR((*(*First).Args)[Count],Second);
	 DestroyCR((*(*Result).Args)[Count]);
	 (*(*Result).Args)[Count]=Temp;
	};
       }
      else Error=-1;
     }
    else
    if ((CRType(Second)==NUMBER)&&(CRType(First)==MUL))
     Result=MulCR(Second,First);
    else
    if ((CRType(First)==PURE_PROD)&&(CRType(Second)==PURE_PROD))
     {
      if ((*First).Length>=(*Second).Length)
       {
	Result=CopyCR(First);
	for(Count=0;Count<(*Second).Length;Count++)
	 {
	  Temp1=MulCR((*((*First).Args))[Count],(*((*Second).Args))[Count]);
	  DestroyCR((*((*Result).Args))[Count]);
	  (*((*Result).Args))[Count]=Temp1;
	 };
       }
      else
	Result=MulCR(Second,First);
     }
    else
    if ((CRType(First)==PURE_SUM)&&(CRType(Second)==PURE_SUM))
     {
      if ((*First).Length>=(*Second).Length)
      {
       Result=FormCREx(PURE_SUM);
       LTemp=(List*)malloc(((*Second).Length+(*First).Length-1)*sizeof( CR*));
       if (Result&&LTemp)
	{
	 (*Result).Length=(*Second).Length+(*First).Length-1;
	 (*Result).MainVar=MVar(First);
	 (*Result).Args=LTemp;
	 m=min((*First).Length,(*Second).Length)-1;
	 n=max((*First).Length,(*Second).Length)-1;
	 for(Count=0;Count<(*Result).Length;Count++)
	  {
	   s1=FormDigitCR(0);
	   Bound11=max(0,Count-m);
	   Bound12=min(Count,n);
	   for(Count1=Bound11;Count1<=Bound12;Count1++)
	    {
	     s2=FormDigitCR(0);
	     Bound21=Count-Count1;
	     Bound22=min(Count,m);
	     for(Count2=Bound21;Count2<=Bound22;Count2++)
	      {
	       Temp1=MulCR(FormDigitCR(C(Count1,Count-Count2)),(*((*Second).Args))[Count2]);
	       Temp=AddCR(s2,Temp1);
	       DestroyCR(Temp1);
	       s2=Temp;
	      };
	     Temp2=MulCR(MulCR(FormDigitCR(C(Count,Count1)),s2),(*((*First).Args))[Count1]);
	     Temp=AddCR(s1,Temp2);
	     DestroyCR(Temp2);
	     s1=Temp;
	    };
	   (*((*Result).Args))[Count]=s1;
	  };
	}
       else Error=-1;
      }
     else Result=MulCR(Second,First);
     }
    else
    if ((CRType(First)==PURE_PROD)&&(CRType(Second)>=102)&&(CRType(Second)<=103))
     {
      L=(*First).Length;
      L1=(*Second).Length/2;
      if (L>L1)
       {
	Temp1=CopyCR(First);
	Result=CorrectT(Second,L);
	LA=L;
       }
      else
      if (L<L1)
       {
	Temp1=CorrectP(First,L1);
	Result=CopyCR(Second);
	LA=L1;
       }
      else
       {
	Temp1=CopyCR(First);
	Result=CopyCR(Second);
	LA=L;
       };
      for(Count=0;Count<LA;Count++)
       {
	Temp2=MulCR((*(*Result).Args)[Count],(*(*Temp1).Args)[Count]);
	DestroyCR((*(*Result).Args)[Count]);
	(*(*Result).Args)[Count]=Temp2;
	Temp3=MulCR((*(*Result).Args)[Count+LA],(*(*Temp1).Args)[Count]);
	DestroyCR((*(*Result).Args)[Count+LA]);
	(*(*Result).Args)[Count+LA]=Temp3;
       }
     }
    else
    if ((CRType(First)>=102)&&(CRType(First)<=103)&&(CRType(Second)==PURE_PROD))
     Result=MulCR(Second,First);
    else
    if ((CRType(First)==DIV)&&(CRType(Second)==DIV))
     {
      if (MVar(First)>MVar(Second))
       {
	Result=CopyCR(First);
	Temp1=Second;
       }
      else
       {
	Result=CopyCR(Second);
	Temp1=First;
       };
      if (Result)
       for(Count=0;Count<(*Result).Length;Count++)
	{
	 Temp2=MulCR((*(*Result).Args)[Count],(*(*Temp1).Args)[Count]);
	 DestroyCR((*(*Result).Args)[Count]);
	 (*(*Result).Args)[Count]=Temp2;
	}
      else Error=-1;
     }
    else
    if (CRType(First)==DIV)
     {
      Result=CopyCR(First);
      if (Result)
       {
	Temp1=MulCR((*(*Result).Args)[0],Second);
	DestroyCR((*(*Result).Args)[0]);
	(*(*Result).Args)[0]=Temp1;
	(*Result).MainVar=max(MVar(First),MVar(Second));
       }
      else Error=-1;
     }
    else
    if (CRType(Second)==DIV)
     Result=MulCR(Second,First);
    else
     {
      Result=FormCREx(MUL);
      LTemp=(List*)malloc(2*sizeof( CR*));
      if (Result&&LTemp)
       {
	(*Result).Length=2;
	(*Result).MainVar=MVar(Second);
	(*LTemp)[0]=CopyCR(First);
	(*LTemp)[1]=CopyCR(Second);
	(*Result).Args=LTemp;
       }
      else Error=-1;
     };
   };
  if (Result) Result=CRSimplify(Result);
  return Result;
 }

 Node* Multiply( Node* First,
		       Node* Second)
 {
   Node* FstCpy;
   Node* ScndCpy;
   Node* Result;
   Node* Temp;
   CR* CRRes;
  int CommSign;
  int Done;
  Done=0;
  FstCpy=Copy(First,First,0);
  ScndCpy=Copy(Second,Second,0);
  CRRes=MulCR((*First).ECR,(*Second).ECR);
  Result=0;
  if (FstCpy&&ScndCpy&&CRRes)
   {
    if (((*FstCpy).Type==MUL)&&((*ScndCpy).Type==MUL))
     {
      CommSign=(*FstCpy).Sign*(*ScndCpy).Sign;
      Temp=(*ScndCpy).Down;
      while ((*Temp).Right!=ScndCpy)
       {
	(*Temp).Sign=(*Temp).Sign*CommSign;
	Temp=(*Temp).Right;
       };
      (*Temp).Sign=(*Temp).Sign*CommSign;
      (*Temp).Right=(*FstCpy).Down;
      (*FstCpy).Down=(*ScndCpy).Down;
      (*FstCpy).Complexity+=(*ScndCpy).Complexity;
      free(ScndCpy);
      Result=FstCpy;
      (*Result).Sign=CommSign;
      Done=1;
     };
    if (((*ScndCpy).Type!=MUL)&&((*FstCpy).Type==MUL)&&(!Done))
     {
      CommSign=(*FstCpy).Sign*(*ScndCpy).Sign;
      (*ScndCpy).Sign=(*ScndCpy).Sign*CommSign;
      (*ScndCpy).Right=(*FstCpy).Down;
      (*FstCpy).Down=ScndCpy;
      (*FstCpy).Complexity+=(*ScndCpy).Complexity;
      Result=FstCpy;
      (*Result).Sign=CommSign;
      Done=1;
     };
    if (((*FstCpy).Type!=MUL)&&((*ScndCpy).Type==MUL)&&(!Done))
     {
      CommSign=(*FstCpy).Sign*(*ScndCpy).Sign;
      (*FstCpy).Sign=(*FstCpy).Sign*CommSign;
      (*FstCpy).Right=(*ScndCpy).Down;
      (*ScndCpy).Down=FstCpy;
      (*ScndCpy).Complexity+=(*FstCpy).Complexity;
      Result=ScndCpy;
      (*Result).Sign=CommSign;
      Done=1;
     };
    if (((*FstCpy).Type!=MUL)&&((*ScndCpy).Type!=MUL)&&(!Done))
     {
      Temp=( Node*)malloc(sizeof( Node));
      if (Temp)
       {
	(*Temp).Type=MUL;
	(*Temp).Value=0;
	(*Temp).Variable=' ';
	(*Temp).Sign=1;
	(*Temp).PowSign=1;
	(*Temp).Flag=0;
	(*Temp).Right=Temp;
	(*Temp).Down=FstCpy;
	(*Temp).Complexity=(*FstCpy).Complexity+(*ScndCpy).Complexity;
	(*FstCpy).Right=ScndCpy;
	(*ScndCpy).Right=Temp;
	(*Temp).ECR=NULL;
       }
       else Error=-1;
      Result=Temp;
     };
   /* SFirst=0;
    SSecond=0;
    Start=Result;
    SFirst=(*Start).Down;
    SSecond=(*SFirst).Right;
    while ((*SFirst).Right!=Result)
     {
      while ((*SFirst).Complexity>(*SSecond).Complexity)
       {

	MStart=Start;
	if (MStart==Result)
	 (*MStart).Down=SSecond;
	 else (*MStart).Right=SSecond;
	Temp=(*SSecond).Right;
	(*SSecond).Right=SFirst;
	(*SFirst).Right=Temp;
	if (MStart==Start)
	 MStart=(*MStart).Down;
	 else MStart=(*MStart).Right;
	SFirst=(*MStart).Right;
	SSecond=(*SFirst).Right;
       };
      if (Start==Result)
       Start=(*Start).Down;
       else Start=(*Start).Right;
      SFirst=(*Start).Right;
      SSecond=(*SFirst).Right;
     };
    if ((*Result).Complexity==0)
     {
      MulRes=1;
      Temp=(*Result).Down;
      while ((Temp!=Result)&&(Error==0))
       {
	(*Temp).Value*=(*Temp).Sign;
	if (((*Temp).Value!=0)&&((*Temp).PowSign==-1))
	 (*Temp).Value=1/(*Temp).Value;
	 else
	  if (((*Temp).Value==0)&&((*Temp).PowSign==-1))
	   Error=-5;
	MulRes*=(*Temp).Value;
	Temp=(*Temp).Right;
       };
      if (Error==0)
       {
	Destroy(Result,Result);
	Result=FormDigitExpr(MulRes);
       };
     };
    if ((*Result).Type==PLUS)
     {
      SFirst=(*Result).Down;
      SSecond=(*SFirst).Right;
      while (((*SFirst).Complexity==0)&&((*SSecond).Complexity==0)&&(Error==0))
       {
	(*SSecond).Value*=(*SSecond).Sign;
	if (((*SSecond).Value!=0)&&((*SSecond).PowSign==-1))
	 (*SSecond).Value=1/(*SSecond).Value;
	 else
	  if (((*SSecond).Value!=0)&&((*SSecond).PowSign==-1))
	   Error=-5;
	(*SFirst).Value*=(*SSecond).Value;
	Temp=(*SSecond).Right;
	if (Error==0) Destroy(SSecond,SSecond);
	SSecond=Temp;
       };
      (*SFirst).Right=SSecond;
     };*/
    (*Result).ECR=CRRes;
   }
   else Error=-1;
  return Result;
 }

 CR* PowerCR( CR* First,  CR* Second)
 {
   CR* Result;
   CR* Temp;
   CR* Temp1;
   CR* Temp2;
   CR* Temp3;
  List* LTemp;
  int Count,Count1,Count2;
  int m,n;
  int Bound11,Bound12;
  int Bound21,Bound22;
   CR* s1;
   CR* s2;
  if ((CRType(First)==NUMBER)&&(CRType(Second)==NUMBER))
   {
    Result=FormDigitCR(pow((*First).Value,(*Second).Value));
   }
  else
  if ((CRType(First)==PURE_PROD)&&(CRType(Second)==PURE_SUM))
   {
    if (MVar(First)==MVar(Second))
     {
      Result=FormCREx(PURE_PROD);
      LTemp=(List*)malloc(((*First).Length+(*Second).Length-1)*sizeof( CR*));
      if (Result&&LTemp)
       {
	(*Result).Length=(*Second).Length+(*First).Length-1;
	(*Result).MainVar=MVar(First);
	(*Result).Args=LTemp;
	m=min((*First).Length,(*Second).Length)-1;
	n=max((*First).Length,(*Second).Length)-1;
	for(Count=0;Count<(*Result).Length;Count++)
	 {
	  s1=FormDigitCR(1);
	  Bound11=max(0,Count-m);
	  Bound12=min(Count,n);
	  for(Count1=Bound11;Count1<=Bound12;Count1++)
	   {
	    s2=FormDigitCR(1);
	    Bound21=Count-Count1;
	    Bound22=min(Count,m);
	    for(Count2=Bound21;Count2<=Bound22;Count2++)
	     {
	      Temp1=MulCR(FormDigitCR(C(Count1,Count-Count2)),(*(*Second).Args)[Count2]);
	      Temp2=MulCR(FormDigitCR(C(Count,Count1)),Temp1);
	      Temp3=PowerCR((*(*First).Args)[Count1],Temp2);
	      Temp=MulCR(s2,Temp3);
	      DestroyCR(s2);
	      DestroyCR(Temp1);
	      DestroyCR(Temp2);
	      DestroyCR(Temp3);
	      s2=Temp;
	     };
	    s1=MulCR(s1,s2);
	   };
	  (*((*Result).Args))[Count]=s1;
	 };
       }
      else Error=-1;
     }
    else
     {
      Result=CopyCR(First);
      if (Result)
       {
	for(Count=0;Count<(*Result).Length;Count++)
	 {
	  Temp=PowerCR((*(*Result).Args)[Count],Second);
	  DestroyCR((*(*Result).Args)[Count]);
	  (*(*Result).Args)[Count]=Temp;
	 };
       }
      else Error=-1;
     };
   }
  else
  if ((CRType(First)==PURE_SUM)&&(CRType(Second)==NUMBER)&&((*Second).Value<=MAX_DEGREE)
      &&((*Second).Value>=-MAX_DEGREE)&&((int)(*Second).Value==(*Second).Value))
   {
    Temp=CopyCR(First);
    for(Count1=1;Count1<abs((int)(*Second).Value);Count1++)
     {
      Temp1=MulCR(Temp,First);
      DestroyCR(Temp);
      Temp=Temp1;
     };
    if ((*Second).Value<0)
     {
      Temp1=InvPowCR(Temp);
      DestroyCR(Temp);
      ShowCR(Temp1,res);
     }
    else Temp1=Temp;
    Result=Temp1;
   }
  else
  if ((CRType(First)==PURE_PROD)&&(CRType(Second)==NUMBER))
   {
    Result=CopyCR(First);
    if (Result)
     {
      for(Count=0;Count<(*Result).Length;Count++)
       {
	Temp=PowerCR((*(*Result).Args)[Count],Second);
	DestroyCR((*(*Result).Args)[Count]);
	(*(*Result).Args)[Count]=Temp;
       };
     }
    else Error=-1;
   }
  else
  if ((CRType(First)==NUMBER)&&(CRType(Second)==PURE_SUM))
   {
    Result=FormCREx(PURE_PROD);
    LTemp=(List*)malloc((*Second).Length*sizeof( CR*));
    if (Result&&LTemp)
     {
      (*Result).Length=(*Second).Length;
      (*Result).MainVar=MVar(Second);
      (*Result).Args=LTemp;
      for(Count=0;Count<(*Result).Length;Count++)
	(*LTemp)[Count]=PowerCR(First,(*(*Second).Args)[Count]);
     }
    else Error=-1;
   }
  else
   {
    Result=FormCREx(POWER);
    LTemp=(List*)malloc(2*sizeof( CR*));
    if (Result&&LTemp)
     {
      (*Result).Length=2;
      (*Result).MainVar=MVar(First);
      (*LTemp)[0]=CopyCR(First);
      (*LTemp)[1]=CopyCR(Second);
      (*Result).Args=LTemp;
     }
    else Error=-1;
   };
  return Result;
 }

 Node* Power( Node* First, Node* Second)
 {
   Node* FstCpy;
   Node* ScndCpy;
   Node* Result;
   CR* CRRes;
  double PowRes;
  FstCpy=Copy(First,First,0);
  ScndCpy=Copy(Second,Second,0);
  Result=FormOpExpr(POWER);
  CRRes=PowerCR((*First).ECR,(*Second).ECR);
  if (FstCpy&&ScndCpy&&Result&&CRRes)
   {
    (*Result).Down=FstCpy;
    (*FstCpy).Right=ScndCpy;
    (*ScndCpy).Right=Result;
    (*Result).Complexity=(*FstCpy).Complexity+(*ScndCpy).Complexity;
    (*Result).ECR=NULL;
    if ((*Result).Complexity==0)
     {
      PowRes=pow((*((*Result).Down)).Value,(*((*((*Result).Down)).Right)).Value);
      PowRes*=(*Result).Sign;
      if (((*Result).PowSign==-1)&&(PowRes!=0))
       PowRes=1/PowRes;
       else
	if (((*Result).PowSign==-1)&&(PowRes==0)) Error=-5;
      if (Error==0)
       {
	Destroy(Result,Result);
	Result=FormDigitExpr(PowRes);
       };
     };
    (*Result).ECR=CRRes;
   }
   else Error=-1;
  return Result;
 }

void Invert( Node* Source)
 {
  if ((*Source).Type==NUMBER)
   {
    (*Source).Value=-(*Source).Value;
   }
   else (*Source).Sign=-(*Source).Sign;
  (*Source).ECR=MulCR(FormDigitCR(-1),(*Source).ECR);
 }

 CR* ApplyCRRule(int Oper,  CR* Arg)
 {
   CR* Result;
  List* LTemp;
  int Count;
  switch (Oper)
   {
    case SINUS   :
    case COSINUS :
    case TANG    :
    case COTANG  :
		  if (CRType(Arg)==PURE_SUM)
		   {
		    Result=FormCREx(TRIG+Oper);
		    LTemp=(List*)malloc((*Arg).Length*2*sizeof( CR*));
		    if (Result&&LTemp)
		     {
		      (*Result).MainVar=MVar(Arg);
		      (*Result).Args=LTemp;
		      (*Result).Length=2*(*Arg).Length;
		      for(Count=0;Count<(*Arg).Length;Count++)
		       (*LTemp)[Count]=ApplyCRRule(SINUS,(*(*Arg).Args)[Count]);
		      for(Count=0;Count<(*Arg).Length;Count++)
		       (*LTemp)[Count+(*Arg).Length]=ApplyCRRule(COSINUS,(*(*Arg).Args)[Count]);
		     }
		    else Error=-1;
		   }
		  else
		  if (CRType(Arg)==NUMBER)
		   {
		    switch (Oper)
		     {
		      case SINUS  : Result=FormDigitCR(sin((*Arg).Value));
				    break;
		      case COSINUS: Result=FormDigitCR(cos((*Arg).Value));
				    break;
		      case TANG   : Result=FormDigitCR(tan((*Arg).Value));
				    break;
		      case COTANG : if (tan((*Arg).Value)!=0)
				     {
				      Result=FormDigitCR(1/tan((*Arg).Value));
				     }
				    else Error=-7;
				    break;
		     }
		   }
		  else
		   {
		    Result=FormCREx(Oper);
		    LTemp=(List*)malloc(sizeof( CR*));
		    if (Result&&LTemp)
		     {
		      (*Result).MainVar=MVar(Arg);
		      (*Result).Length=1;
		      (*LTemp)[0]=CopyCR(Arg);
		      (*Result).Args=LTemp;
		     }
		    else Error=-1;
		   };
	   break;
    case EXPONENT:
		  if (CRType(Arg)==PURE_SUM)
		   {
		    Result=FormCREx(PURE_PROD);
		    LTemp=(List*)malloc((*Arg).Length*sizeof( CR*));
		    if (Result&&LTemp)
		     {
		      (*Result).Length=(*Arg).Length;
		      (*Result).MainVar=MVar(Arg);
		      (*Result).Args=LTemp;
		      for(Count=0;Count<(*Arg).Length;Count++)
		       (*LTemp)[Count]=ApplyCRRule(EXPONENT,(*(*Arg).Args)[Count]);
		     }
		    else Error=-1;
		   }
		  else
		  if (CRType(Arg)==NUMBER)
		   {
		    Result=FormDigitCR(exp((*Arg).Value));
		   }
		  else
		   {
		    Result=FormCREx(EXPONENT);
		    LTemp=(List*)malloc(sizeof( CR*));
		    if (Result&&LTemp)
		     {
		      (*Result).MainVar=MVar(Arg);
		      (*Result).Length=1;
		      (*LTemp)[0]=CopyCR(Arg);
		      (*Result).Args=LTemp;
		     }
		    else Error=-1;
		   };
	   break;
    case LN      :
		  if (CRType(Arg)==NUMBER)
		   {
		    if ((*Arg).Value>0)
		     Result=FormDigitCR(log((*Arg).Value));
		    else Error=-10;
		   }
		  else
		  if (CRType(Arg)==PURE_PROD)
		   {
		    Result=FormCREx(PURE_SUM);
		    LTemp=(List*)malloc((*Arg).Length*sizeof( CR*));
		    if (Result&&LTemp)
		     {
		      (*Result).MainVar=MVar(Arg);
		      (*Result).Args=LTemp;
		      (*Result).Length=(*Arg).Length;
		      for (Count=0;Count<(*Result).Length;Count++)
		       (*LTemp)[Count]=ApplyCRRule(LN,(*(*Arg).Args)[Count]);
		     }
		    else Error=-1;
		   }
		  else
		   {
		    Result=FormCREx(LN);
		    LTemp=(List*)malloc(sizeof( CR*));
		    if (Result&&LTemp)
		     {
		      (*Result).MainVar=MVar(Arg);
		      (*Result).Length=1;
		      (*LTemp)[0]=CopyCR(Arg);
		      (*Result).Args=LTemp;
		     }
		    else Error=-1;
		   };
	   break;
    case U_MINUS :
		  Result=MulCR(FormDigitCR(-1),Arg);
	   break;
    default      :
	   Result=FormCREx(Oper);
	   LTemp=(List*)malloc(sizeof( CR*));
	   if (Result&&LTemp)
	    {
	     (*Result).Type=Oper;
	     (*Result).MainVar=MVar(Arg);
	     (*LTemp)[0]=CopyCR(Arg);
	     (*Result).Args=LTemp;
	     (*Result).Length=1;
	    }
	   else Error=-1;
   };
  return Result;
 }

 Node* ApplyOperation( Node* Operation,
			     Node* Argument)
 {
   Node* ArgCpy;
   Node* Result;
  double ApplRes;
   CR* CRRes;
  ArgCpy=Copy(Argument,Argument,0);
  Result=FormOpExpr((*Operation).Type);
  CRRes=ApplyCRRule((*Operation).Type,(*Argument).ECR);
  if (ArgCpy&&Result&&CRRes)
   {
    if ((*Result).Type!=U_MINUS)
     {
      (*Result).Down=ArgCpy;
      (*ArgCpy).Right=Result;
      (*Result).Complexity=(*ArgCpy).Complexity;
     } 
    else
     {
      Invert(ArgCpy);
      Result=ArgCpy;
     };
    if ((*Result).Complexity==0)
     {
      switch ((*Result).Type)
       {
       case NUMBER:
		 ApplRes=(*Result).Value;
		 ApplRes*=(*Result).Sign;
		 if ((ApplRes!=0)&&((*Result).PowSign==-1))
		 ApplRes=1/ApplRes;
		  else
		   if ((ApplRes==0)&&((*Result).PowSign==-1))
		    Error=-5;
		 break;
       case SINUS:
		 ApplRes=sin((*((*Result).Down)).Value);
		 ApplRes*=(*Result).Sign;
		 if ((ApplRes!=0)&&((*Result).PowSign==-1))
		 ApplRes=1/ApplRes;
		  else
		   if ((ApplRes==0)&&((*Result).PowSign==-1))
		    Error=-5;
		 break;
       case COSINUS:
		 ApplRes=cos((*((*Result).Down)).Value);
		 ApplRes*=(*Result).Sign;
		 if ((ApplRes!=0)&&((*Result).PowSign==-1))
		  ApplRes=1/ApplRes;
		  else
		   if ((ApplRes==0)&&((*Result).PowSign==-1))
		    Error=-5;
		 break;
       case TANG:
		 ApplRes=tan((*((*Result).Down)).Value);
		 ApplRes*=(*Result).Sign;
		 if ((ApplRes!=0)&&((*Result).PowSign==-1))
		  ApplRes=1/ApplRes;
		  else
		   if ((ApplRes==0)&&((*Result).PowSign==-1))
		    Error=-5;
		 break;
       case COTANG:
		 if (tan((*((*Result).Down)).Value)!=0)
		  ApplRes=1/tan((*((*Result).Down)).Value);
		  else Error=-7;
		 ApplRes*=(*Result).Sign;
		 if ((ApplRes!=0)&&((*Result).PowSign==-1))
		  ApplRes=1/ApplRes;
		  else
		   if ((ApplRes==0)&&((*Result).PowSign==-1))
		    Error=-5;
		 break;
       case ARCSIN:
		 if (((*((*Result).Down)).Value>=-1)&&((*((*Result).Down)).Value<=1))
		  ApplRes=asin((*Result).Value);
		  else Error=-8;
		 ApplRes*=(*Result).Sign;
		 if ((ApplRes!=0)&&((*Result).PowSign==-1))
		  ApplRes=1/ApplRes;
		  else
		   if ((ApplRes==0)&&((*Result).PowSign==-1))
		    Error=-5;
		 break;
       case ARCCOS:
		 if (((*((*Result).Down)).Value>=-1)&&((*((*Result).Down)).Value<=1))
		  ApplRes=acos((*Result).Value);
		  else Error=-9;
		 ApplRes*=(*Result).Sign;
		 if ((ApplRes!=0)&&((*Result).PowSign==-1))
		  ApplRes=1/ApplRes;
		  else
		   if ((ApplRes==0)&&((*Result).PowSign==-1))
		    Error=-5;
		 break;
       case ARCTANG:
		 ApplRes=atan((*((*Result).Down)).Value);
		 ApplRes*=(*Result).Sign;
		 if ((ApplRes!=0)&&((*Result).PowSign==-1))
		  ApplRes=1/ApplRes;
		  else
		   if ((ApplRes==0)&&((*Result).PowSign==-1))
		    Error=-5;
		 break;
       case ARCCOTANG:
		 ApplRes=PI/2-atan((*((*Result).Down)).Value);
		 ApplRes*=(*Result).Sign;
		 if ((ApplRes!=0)&&((*Result).PowSign==-1))
		  ApplRes=1/ApplRes;
		  else
		   if ((ApplRes==0)&&((*Result).PowSign==-1))
		    Error=-5;
		 break;
       case EXPONENT:
		 ApplRes=exp((*((*Result).Down)).Value);
		 ApplRes*=(*Result).Sign;
		 if ((*Result).PowSign==-1)
		  ApplRes=1/ApplRes;
		 break;
       case LN:
		 if ((*((*Result).Down)).Value>0)
		  ApplRes=log((*((*Result).Down)).Value);
		  else Error=-10;
		 ApplRes*=(*Result).Sign;
		 if ((ApplRes!=0)&&((*Result).PowSign==-1))
		  ApplRes=1/ApplRes;
		  else
		   if ((ApplRes==0)&&((*Result).PowSign==-1))
		    Error=-5;
		 break;
       case U_MINUS:
		 ApplRes=(*((*Result).Down)).Value;
		 break;
       default:  Error=-6;
       };
      if (Error==0)
       {
	Destroy(Result,Result);
	Result=FormDigitExpr(ApplRes);
       };
     };
    (*Result).ECR=CRRes;
   }
   else Error=-1;
  return Result;
 }

 CR* InvPowCR( CR* Subj)
 {
   CR* Result;
   CR* Temp;
  List* LTemp;
  int Count;
  if (CRType(Subj)==NUMBER)
   {
    Result=CopyCR(Subj);
    if (Result)
     {
      if ((*Result).Value!=0)
       (*Result).Value=1/(*Result).Value;
      else Error=-5;
     }
    else Error=-1;
   }
  else
  if ((CRType(Subj)==PURE_PROD)||(CRType(Subj)==MUL))
   {
    Result=CopyCR(Subj);
    if (Result)
     {
      for(Count=0;Count<(*Result).Length;Count++)
       {
	Temp=InvPowCR((*(*Result).Args)[Count]);
	DestroyCR((*(*Result).Args)[Count]);
	(*(*Result).Args)[Count]=Temp;
       };
     }
    else Error=-1;
   }
  else
  if (CRType(Subj)==DIV)
   {
    Result=CopyCR(Subj);
    if (Result)
     {
      Temp=(*(*Result).Args)[0];
      (*(*Result).Args)[0]=(*(*Result).Args)[1];
      (*(*Result).Args)[1]=Temp;
     }
    else Error=-1;
   }
  else
   {
    Result=FormCREx(DIV);
    LTemp=(List*)malloc(2*sizeof( CR*));
    if (Result&&LTemp)
     {
      (*Result).MainVar=MVar(Subj);
      (*Result).Length=2;
      (*Result).Args=LTemp;
      (*LTemp)[0]=FormDigitCR(1);
      (*LTemp)[1]=CopyCR(Subj);
     }
    else Error=-1;
   };
  return Result;
 }

void Store(double Arg,double* res)
 {
  res[Res_Index]=Arg;
  Res_Index++;
 }

double My_Pow(double Arg, double Power)
 {
  int i;
  double Result;
  if ((int)Power==Power)
   {
    Result=Arg;
    for(i=2;i<=Power;i++)
     Result*=Arg;
   }
  else Result=pow(Arg,Power);
  return Result;
 }

double Value( CR* Arg, int Level)
 {
  int v;
  int w;
  int L;
  double Result;
  double Temp1;
  int Count;
  List* Argum;
  ValueList* Vals;
  v=MVar(Arg);
  w=(*Arg).Type;
  L=(*Arg).Length;
  Argum=(*Arg).Args;
  Vals=(*Arg).Values;
  if (v<=Level)
   {
    switch (w)
     {
      case NUMBER     :
		       Result=(*Arg).Value;
		       break;
      case PURE_SUM   :
      case PURE_PROD  :
      case TRIG_SIN   :
		       Result=(*Vals)[0];
		       break;
      case TRIG_COS   :
		       Result=(*Vals)[L/2];
		       break;
      case TRIG_TANG  :
		       Result=(*Vals)[0]/(*Vals)[L/2];
		       break;
      case TRIG_COTANG:
		       Result=(*Vals)[L/2]/(*Vals)[0];
		       break;
      default         :
		       for(Count=0;Count<L;Count++)
			{
			 Temp1=Value((*Argum)[Count],Level);
			 (*Vals)[Count]=Temp1;
			};
		       switch (w)
			{
			 case PLUS       :
					  Result=0;
					  for(Count=0;Count<L;Count++)
					   Result+=(*Vals)[Count];
					  break;
			 case MUL        :
					  Result=1;
					  for(Count=0;Count<L;Count++)
					   Result*=(*Vals)[Count];
					  break;
			 case DIV        :
					  Result=(*Vals)[0]/(*Vals)[1];
					  break;
			 case POWER      :
					  Result=My_Pow((*Vals)[0],(*Vals)[1]);
					  break;
			 case SINUS      :
					  Result=sin((*Vals)[0]);
					  break;
			 case COSINUS    :
					  Result=cos((*Vals)[0]);
					  break;
			 case TANG       :
					  Result=tan((*Vals)[0]);
					  break;
			 case COTANG     :
					  Result=1/tan((*Vals)[0]);
					  break;
			 case ARCSIN     :
					  Result=asin((*Vals)[0]);
					  break;
			 case ARCCOS     :
					  Result=acos((*Vals)[0]);
					  break;
			 case ARCTANG    :
					  Result=atan((*Vals)[0]);
					  break;
			 case ARCCOTANG  :
					  Result=PI/2-atan((*Vals)[0]);
					  break;
			 case EXPONENT   :
					  Result=exp((*Vals)[0]);
					  break;
			 case LN         :
					  Result=log((*Vals)[0]);
					  break;
			};
		       break;
     };
   }
  else
  if (v>Level+1)
   {
    for(Count=0;Count<L;Count++)
     {
      Value((*Argum)[Count],Level); /*Just for side effect*/
     };
    Result=(*Vals)[0];
   }
  else
  if (v==Level+1)
   {
    for(Count=0;Count<L;Count++)
     {
      Temp1=Value((*Argum)[Count],Level);
      (*Vals)[Count]=Temp1;
     };
    Result=(*Vals)[0];
   }
  else
   {
    Result=Value(Arg,Level-1);
   };
  return Result;
 }

double E( CR* Arg, int Level)
 {
  int v;
  int w;
  int t;
  int L;
  double Z,r1,r2,r3,r4,Result;
  int Count;
  List* Argum;
  ValueList* Vals;
  v=MVar(Arg);
  w=(*Arg).Type;
  L=(*Arg).Length;
  Argum=(*Arg).Args;
  Vals=(*Arg).Values;
  if (v>Level)
   {
    for(Count=0;Count<L;Count++)
     Z=E((*Argum)[Count],Level);
    Result=(*Vals)[0];
   }
  else
  if (v==Level)
   {
    t=(*Arg).Length/2;
    switch (w)
     {
      case PURE_SUM   :
		       for(Count=0;Count<L-1;Count++)
			(*Vals)[Count]+=(*Vals)[Count+1];
		       Result=(*Vals)[0];
		       break;
      case PURE_PROD  :
		       for(Count=0;Count<L-1;Count++)
			(*Vals)[Count]*=(*Vals)[Count+1];
		       Result=(*Vals)[0];
		       break;
      case TRIG_SIN   :
		       for(Count=0;Count<t-1;Count++)
			{
			 r1=(*Vals)[Count]*(*Vals)[Count+t+1];
			 r2=(*Vals)[Count+t]*(*Vals)[Count+1];
			 Z=r1+r2;
			 r3=(*Vals)[Count+t]*(*Vals)[Count+t+1];
			 r4=(*Vals)[Count]*(*Vals)[Count+1];
			 (*Vals)[Count+t]=r3-r4;
			 (*Vals)[Count]=Z;
			};
		       Result=(*Vals)[0];
		       break;
      case TRIG_COS   :
		       for(Count=0;Count<t-1;Count++)
			{
			 r1=(*Vals)[Count]*(*Vals)[Count+t+1];
			 r2=(*Vals)[Count+t]*(*Vals)[Count+1];
			 Z=r1+r2;
			 r3=(*Vals)[Count+t]*(*Vals)[Count+t+1];
			 r4=(*Vals)[Count]*(*Vals)[Count+1];
			 (*Vals)[Count+t]=r3-r4;
			 (*Vals)[Count]=Z;
			};
		       Result=(*Vals)[t];
		       break;
      case TRIG_TANG  :
		       for(Count=0;Count<t-1;Count++)
			{
			 r1=(*Vals)[Count]*(*Vals)[Count+t+1];
			 r2=(*Vals)[Count+t]*(*Vals)[Count+1];
			 Z=r1+r2;
			 r3=(*Vals)[Count+t]*(*Vals)[Count+t+1];
			 r4=(*Vals)[Count]*(*Vals)[Count+1];
			 (*Vals)[Count+t]=r3-r4;
			 (*Vals)[Count]=Z;
			};
		       Result=(*Vals)[0]/(*Vals)[t];
		       break;
      case TRIG_COTANG:
		       for(Count=0;Count<t-1;Count++)
			{
			 r1=(*Vals)[Count]*(*Vals)[Count+t+1];
			 r2=(*Vals)[Count+t]*(*Vals)[Count+1];
			 Z=r1+r2;
			 r3=(*Vals)[Count+t]*(*Vals)[Count+t+1];
			 r4=(*Vals)[Count]*(*Vals)[Count+1];
			 (*Vals)[Count+t]=r3-r4;
			 (*Vals)[Count]=Z;
			};
		       Result=(*Vals)[t]/(*Vals)[0];
		       break;
      default         :
		       for(Count=0;Count<L;Count++)
			 (*Vals)[Count]=E((*Argum)[Count],Level);
		       Result=(*Vals)[0];
		       break;
     };
   };
   return Result;
 }

void EvalCR( CR* Chain, int Level, double* Reslt)
 {
  int Count;
  int Num;
   CR* Work;
  double Temp;
  Work=CopyCR(Chain);
  Num=Variables[Level].StepNum;
  if (Level==Index-1)
   {
    for(Count=0;Count<=Num;Count++)
     {
      Temp=Value(Work,Level); /*Take a Value*/
      Reslt[Res_Index]=Temp; /*Storing result*/
      Res_Index+=1;
      E(Work,Level); /*Shifting */
     };
   }
  else
   {
    for(Count=0;Count<=Num;Count++)
     {
      Temp=Value(Work,Level);
      EvalCR(Work,Level+1,Reslt);
      E(Work,Level);
     };
   };
 }

double Value_D( CR* Arg, int Level)
 {
  int v;
  int w;
  int L;
  double Result;
  double Temp1;
  int Count;
  List* Argum;
  ValueList* Vals;
  v=MVar(Arg);
  w=(*Arg).Type;
  L=(*Arg).Length;
  Argum=(*Arg).Args;
  Vals=(*Arg).Values;
  if (v<=Level)
   {
    switch (w)
     {
      case NUMBER     :
		       Result=(*Arg).Value;
		       break;
      case PURE_SUM   :
      case PURE_PROD  :
      case TRIG_SIN   :
		       Result=(*Vals)[0];
		       break;
      case TRIG_COS   :
		       Result=(*Vals)[L/2];
		       break;
      case TRIG_TANG  :
		       Result=(*Vals)[0]/(*Vals)[L/2];
		       break;
      case TRIG_COTANG:
		       Result=(*Vals)[L/2]/(*Vals)[0];
		       break;
      default         :
		       for(Count=0;Count<L;Count++)
			{
			 Temp1=Value_D((*Argum)[Count],Level);
			 (*Vals)[Count]=Temp1;
			};
		       switch (w)
			{
			 case PLUS       :
					  Result=0;
					  for(Count=0;Count<L;Count++)
					   Result+=(*Vals)[Count];
					  break;
			 case MUL        :
					  Result=1;
					  for(Count=0;Count<L;Count++)
					   Result*=(*Vals)[Count];
					  break;
			 case DIV        :
					  Result=(*Vals)[0]/(*Vals)[1];
					  break;
			 case POWER      :
					  Result=pow((*Vals)[0],(*Vals)[1]);
					  break;
			 case SINUS      :
					  Result=sin((*Vals)[0]);
					  break;
			 case COSINUS    :
					  Result=cos((*Vals)[0]);
					  break;
			 case TANG       :
					  Result=tan((*Vals)[0]);
					  break;
			 case COTANG     :
					  Result=1/tan((*Vals)[0]);
					  break;
			 case ARCSIN     :
					  Result=asin((*Vals)[0]);
					  break;
			 case ARCCOS     :
					  Result=acos((*Vals)[0]);
					  break;
			 case ARCTANG    :
					  Result=atan((*Vals)[0]);
					  break;
			 case ARCCOTANG  :
					  Result=PI/2-atan((*Vals)[0]);
					  break;
			 case EXPONENT   :
					  Result=exp((*Vals)[0]);
					  break;
			 case LN         :
					  Result=log((*Vals)[0]);
					  break;
			};
		       break;
     };
   }
  else
  if (v>Level+1)
   {
    for(Count=0;Count<L;Count++)
     {
      Value_D((*Argum)[Count],Level); /*Just for side effect*/
     };
    Result=(*Vals)[0];
   }
  else
  if (v==Level+1)
   {
    for(Count=0;Count<L;Count++)
     {
      Temp1=Value_D((*Argum)[Count],Level);
      (*Vals)[Count]=Temp1;
     };
    Result=(*Vals)[0];
   }
  else
   {
    Result=Value_D(Arg,Level-1);
   };
  return Result;
 }

void E_D( CR* Arg, int Level)
 {
  int v;
  int w;
  int t;
  int L;
  double Z,r1,r2,r3,r4;
  int Count;
  List* Argum;
  ValueList* Vals;
  v=MVar(Arg);
  w=(*Arg).Type;
  L=(*Arg).Length;
  Argum=(*Arg).Args;
  Vals=(*Arg).Values;
  if (v>Level)
   {
    for(Count=0;Count<L;Count++)
     E_D((*Argum)[Count],Level);
   }
  else
  if (v==Level)
   {
    t=(*Arg).Length/2;
    switch (w)
     {
      case PURE_SUM   :
		       for(Count=0;Count<L-1;Count++)
			(*Vals)[Count]+=(*Vals)[Count+1];
		       break;
      case PURE_PROD  :
		       for(Count=0;Count<L-1;Count++)
			(*Vals)[Count]*=(*Vals)[Count+1];
		       break;
      case TRIG_SIN   :
      case TRIG_COS   :
      case TRIG_TANG  :
      case TRIG_COTANG:
		       for(Count=0;Count<t-1;Count++)
			{
			 r1=(*Vals)[Count]*(*Vals)[Count+t+1];
			 r2=(*Vals)[Count+t]*(*Vals)[Count+1];
			 Z=r1+r2;
			 r3=(*Vals)[Count+t]*(*Vals)[Count+t+1];
			 r4=(*Vals)[Count]*(*Vals)[Count+1];
			 (*Vals)[Count+t]=r3-r4;
			 (*Vals)[Count]=Z;
			};
		       break;
      default         :
		       for(Count=0;Count<L;Count++)
			{
			 E_D((*Argum)[Count],Level);
			 (*Vals)[Count]=GetMValue((*Argum)[Count]);
			};
		       break;
     };
   };
 }

void EvalCR_D( CR* Chain,double* Reslt)
 {
  int Count0,Count1;
  int Num0,Num1;
   CR* Work;
  double Temp;
  Work=CopyCR(Chain);
  Num0=Variables[0].StepNum;
  Num1=Variables[1].StepNum;
  for(Count0=0;Count0<=Num0;Count0++)
   {
    Temp=Value_D(Work,0);
    for(Count1=0;Count1<=Num1;Count1++)
     {
      Temp=Value_D(Work,1); /*Take a Value*/
      Reslt[Res_Index]=Temp; /*Storing result*/
      Res_Index+=1;
      E_D(Work,1); /*Shifting */
     };
    E_D(Work,0);
   };
 }

double Value_S( CR* Arg)
 {
  int v;
  int w;
  int L;
  double Result;
  double Temp1;
  int Count;
  List* Argum;
  ValueList* Vals;
  v=MVar(Arg);
  w=(*Arg).Type;
  L=(*Arg).Length;
  Argum=(*Arg).Args;
  Vals=(*Arg).Values;
  if (v<0)
   Result=(*Arg).Value;
  else
  if (v==0)
   {
    switch (w)
     {
      case PURE_SUM   :
      case PURE_PROD  :
      case TRIG_SIN   :
		       Result=(*Vals)[0];
		       break;
      case TRIG_COS   :
		       Result=(*Vals)[L/2];
		       break;
      case TRIG_TANG  :
		       Result=(*Vals)[0]/(*Vals)[L/2];
		       break;
      case TRIG_COTANG:
		       Result=(*Vals)[L/2]/(*Vals)[0];
		       break;
      default         :
		       for(Count=0;Count<L;Count++)
			{
			 Temp1=Value_S((*Argum)[Count]);
			 (*Vals)[Count]=Temp1;
			};
		       switch (w)
			{
			 case PLUS       :
					  Result=0;
					  for(Count=0;Count<L;Count++)
					   Result+=(*Vals)[Count];
					  break;
			 case MUL        :
					  Result=1;
					  for(Count=0;Count<L;Count++)
					   Result*=(*Vals)[Count];
					  break;
			 case DIV        :
					  Result=(*Vals)[0]/(*Vals)[1];
					  break;
			 case POWER      :
					  Result=pow((*Vals)[0],(*Vals)[1]);
					  break;
			 case SINUS      :
					  Result=sin((*Vals)[0]);
					  break;
			 case COSINUS    :
					  Result=cos((*Vals)[0]);
					  break;
			 case TANG       :
					  Result=tan((*Vals)[0]);
					  break;
			 case COTANG     :
					  Result=1/tan((*Vals)[0]);
					  break;
			 case ARCSIN     :
					  Result=asin((*Vals)[0]);
					  break;
			 case ARCCOS     :
					  Result=acos((*Vals)[0]);
					  break;
			 case ARCTANG    :
					  Result=atan((*Vals)[0]);
					  break;
			 case ARCCOTANG  :
					  Result=PI/2-atan((*Vals)[0]);
					  break;
			 case EXPONENT   :
					  Result=exp((*Vals)[0]);
					  break;
			 case LN         :
					  Result=log((*Vals)[0]);
					  break;
			};
		       break;
     };
   };
  return Result;
 }

void E_S( CR* Arg)
 {
  int w;
  int t;
  int L;
  double Z,r1,r2,r3,r4;
  int Count;
  List* Argum;
  ValueList* Vals;
  w=(*Arg).Type;
  L=(*Arg).Length;
  Argum=(*Arg).Args;
  Vals=(*Arg).Values;
  t=(*Arg).Length/2;
  switch (w)
   {
    case PURE_SUM   :
		     for(Count=0;Count<L-1;Count++)
		      (*Vals)[Count]+=(*Vals)[Count+1];
		     break;
      case PURE_PROD  :
		       for(Count=0;Count<L-1;Count++)
			(*Vals)[Count]*=(*Vals)[Count+1];
		       break;
      case TRIG_SIN   :
      case TRIG_COS   :
      case TRIG_TANG  :
      case TRIG_COTANG:
		       for(Count=0;Count<t-1;Count++)
			{
			 r1=(*Vals)[Count]*(*Vals)[Count+t+1];
			 r2=(*Vals)[Count+t]*(*Vals)[Count+1];
			 Z=r1+r2;
			 r3=(*Vals)[Count+t]*(*Vals)[Count+t+1];
			 r4=(*Vals)[Count]*(*Vals)[Count+1];
			 (*Vals)[Count+t]=r3-r4;
			 (*Vals)[Count]=Z;
			};
		       break;
      default         :
		       for(Count=0;Count<L;Count++)
			{
			 E_S((*Argum)[Count]);
			 (*Vals)[Count]=GetMValue((*Argum)[Count]);
			};
		       break;
     };
 }

void EvalCR_S( CR* Chain,double* Reslt)
 {
  int Count;
  int Num;
   CR* Work;
  double Temp;
  Work=CopyCR(Chain);
  Num=Variables[0].StepNum;
  for(Count=0;Count<=Num;Count++)
   {
    Temp=Value_S(Work); /*Take a Value*/
    Store(Temp,Reslt); /*Storing result*/
    E_S(Work); /*Shifting */
   };
 }

double GetValue( CR* Subj)
 {
  double Result=0;
  if ((*Subj).Type==NUMBER)
   Result=(*Subj).Value;
  else
   Result=GetValue((*(*Subj).Args)[0]);
  return Result;
 }

double GetMValue( CR* Subj)
 {
  double Result;
  switch ((*Subj).Type)
   {
    case NUMBER      : Result=(*Subj).Value;
		       break;
    case TRIG_COS    : Result=(*(*Subj).Values)[(*Subj).Length/2];
		       break;
    case TRIG_TANG   : Result=(*(*Subj).Values)[0]/(*(*Subj).Values)[(*Subj).Length/2];
		       break;
    case TRIG_COTANG : Result=(*(*Subj).Values)[(*Subj).Length/2]/(*(*Subj).Values)[0];
		       break;
    default          : Result=(*(*Subj).Values)[0];
		       break;
   };
  return Result;
 }

int Prepare( CR* Subj)
 {
  ValueList* Temp=NULL;
  int Result=1;
  int Count;
  if ((*Subj).Type!=NUMBER)
   {
    Temp=(ValueList*)malloc(sizeof(double)*(*Subj).Length);
    if (Temp)
     {
      (*Subj).Values=Temp;
      for(Count=0;Count<(*Subj).Length;Count++)
       if (!Prepare((*(*Subj).Args)[Count]))
	Result=0;
       else
	(*Temp)[Count]=GetValue((*(*Subj).Args)[Count]);
     }
    else Result=0;
   };
  return Result;
 }

void InvertPow( Node* Source)
 {
  (*Source).PowSign*=-1;
  (*Source).ECR=InvPowCR((*Source).ECR);
 }

void RegisterVariable( Variable NewVar)
 {
  int Count;
  int Hit=0;
  for(Count=0;Count<Index;Count++)
   if (Variables[Count].Name==NewVar.Name)
    {
     Hit=1;
     Variables[Count].Start=NewVar.Start;
     Variables[Count].Step=NewVar.Step;
     Variables[Count].StepNum=NewVar.StepNum;
    };
  if (Hit==0)
   {
    if (Index<101)
     {
      Variables[Index].Name=NewVar.Name;
      Variables[Index].Start=NewVar.Start;
      Variables[Index].Step=NewVar.Step;
      Variables[Index].StepNum=NewVar.StepNum;
      Index++;
     }
     else Error=-2;
   };
 }

void InitVariable( Variable Init)
 {
  int Count;
  int Hit=0;
  for(Count=0;Count<Index,Hit==0;Count++)
   if (Variables[Count].Name==Init.Name)
    {
     Variables[Count].Start=Init.Start;
     Variables[Count].Step=Init.Step;
     Hit=1;
    };
  if (Hit==0) Error=-3;
 }

void ClearVarInfo()
 {
  Index=0;
 }

int Enable( Node* Operation)


 {
  int i=0;
  if (Action!=0)
   {
    switch ((*Operation).Type)
     {
      case PLUS   :
      case B_MINUS:
       switch ((*((*Action).Act)).Type)
	{
	 case LEFT:    i=0;break;
	 default:      i=1;break;
	};
	break;
      case MUL    :
      case DIV    :
       switch ((*((*Action).Act)).Type)
	{
	 case LEFT   :
	 case PLUS   :
	 case B_MINUS: i=0;break;
	 default:      i=1;break;
	};
	break;
      case POWER  :
       switch ((*((*Action).Act)).Type)
	{
	 case LEFT   :
	 case MUL    :
	 case DIV    :
	 case PLUS   :
         case U_MINUS:
	 case B_MINUS: i=0;break;
	 default:      i=1;break;
	};
	break;
      case RIGHT:      i=1;break;
      default:         i=0;break;
     };
   } else i=1;
  return i;
 }

