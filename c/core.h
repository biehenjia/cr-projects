/* core.h - definitions of public C MCR functions and data types.

This file is part of the C MCR Library.
Copyright (C) 1998,1999 Eugene Zima, Vassili Mitrofanov, Vladimir Kislenkov

This software is for educational and research purposes only.

The C MCR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  

Please send your questions about this software to 
Eugene Zima <ezima@daisy.uwaterloo.ca>.

*/

#include"stdio.h"

/*type declarations                       */


typedef struct _CR* List [];

typedef double ValueList [];

/* Chain of recurrences (CR)              */
typedef struct _CR
 {
  int Type;
  int MainVar;
  double Value;
  List* Args;
  int Length;
  ValueList* Values;
 } CR;

/* node of a right-linked tree             */
typedef struct _Node
 {
  int Type;
  double Value;
  char Variable;
  int Sign;
  int PowSign;
  int Flag;
  int Complexity;
  struct _Node* Down;
  struct _Node* Right;
  CR* ECR;
 } Node;

/* variable (name, start_value, step_of_change, number_of_steps) */
typedef struct _Variable
 {
  char Name;
  double Start;
  double Step;
  int StepNum;
 } Variable;

typedef struct _VarPosition
 {
  int Pos;
  double Start;
  double Step;
  int StepNum;
 } VarPosition;
/******************************************/

/* global variables (defined in a source file core.c) */
extern int Error;
extern int Index;
extern int Count;
extern long Res_Index;
extern char c;
extern Variable Variables[100];
/******************************************/

/* Functions declarations                 */
extern void ClearError();
extern void StrongClearError();

/* creates a copy of the CR *Source and returns a pointer to it */
extern CR* CopyCR(CR* Source);
extern Node* Copy(Node* Source,
		              Node* UpSrce,
		              Node* UpRslt);

/* removes a *Subj CR from heap memory */
extern void DestroyCR(CR* Subj);

extern void Destroy(Node* Subj, Node* UpLevel);

/* spells the error situations */
extern void Speller();

/* evaluates a CR *Chain and fills array of values Reslt */
extern void EvalCR(CR* Chain, int Level, double* Reslt);

/* initializes a CR *Subj (call it before interpretation) */
extern int Prepare(CR* Subj);

/* registers a variable in a CR system */
extern void RegisterVariable(Variable NewVar);

/* initializes a variable */
extern void InitVariable(Variable Init);

/* removes all variables */
extern void ClearVarInfo();

/* constructs a CR from a closed form function, written in infix form in Arg */
extern CR* Analyse(char* Arg);

/* writes a CR *Subj to a file */
extern void ShowCR(CR* Subj, FILE* outp);

/* reads a number from a string Arg */
extern double GetDigit(char* Arg);
/******************************************/
