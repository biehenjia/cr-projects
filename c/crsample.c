/* crsample.c - a sample of C MCR Library usage.

This file is part of the C MCR Library.
Copyright (C) 1998,1999 Eugene Zima, Vassili Mitrofanov, Vladimir Kislenkov

This software is for educational and research purposes only.

The C MCR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  

Please send your questions about this software to 
Eugene Zima <ezima@daisy.uwaterloo.ca>.

-----------------------------------

This file contains a sample program, which computes values of 
a given closed-form expression over a regular grid, using MCR technique,
and shows the time of the computation performed. 

User has to provide a function and a grid.

This file is compiled and linked into the "cmcr" executable file using a makefile.

  Examples of command line:

  cmcr "x,0,0.01,100 y,0,0.01,100 ;" "1.3^(1.2*x-1)*cos(1.5*y)" 

    this computational task is equivalent to the following one:

    for x from 0 to 1 by 0.01 do
      from y from 0 to 1 by 0.01 do
        compute_and_store( 1.3^(1.2*x-1)*cos(1.5*y) );
      od;
    od;

  cmcr "x,0,0.01,1000 y,-1,0.1,20 ;" "x^5+2*x^4-4.2*x^3+1" "x^10-2.34*x^8+2.1*x^3-11" 

    this computational task is equivalent to the following one:

    for x from 0 to 10 by 0.01 do
      from y from -1 to 1 by 0.1 do
        compute_and_store( x^5+2*x^4-4.2*x^3+1 );
      od;
    od;

    for x from 0 to 10 by 0.01 do
      from y from -1 to 1 by 0.1 do
        compute_and_store( x^10-2.34*x^8+2.1*x^3-11 );
      od;
    od;


  To use C MCR in your programs:

    1) Put an include directive #include"core.h" into your source file.
    2) Register all variables by a call to RegisterVariable(Variable var).
       For each variable the following properties should be defined:
         name, start value, step, number of steps
    3) Construct a CR by a call to Analyze(char* str). str must contain a 
         closed-form function in infix form: "sin(2*y)-cos(1.5*x)"
       All variables, which are used in str, must be already registered
    4) Call Prepare(CR* cr) to initialize a CR before interpretation
    5) Allocate memory to store a result of computation
    6) Call EvalCR(CR* Chain, int Level, double* Reslt) to interprete CR *Chain
          and store results in array Reslt. Set Level to 0.
        
*/

#include<stdlib.h>
#include<math.h>

#ifndef _MSC_VER     /* Non-MFC include directives. */
#include<unistd.h>
#include<sys/time.h> 
#endif

#ifdef _MSC_VER
#include<time.h>
#endif

#include"core.h"

#ifdef CLOCKS_PER_SEC
#define time_coeff (1000.0/CLOCKS_PER_SEC)
#else
#define time_coeff (1)
#define UNKNOWN_CLOCK_PER_SEC
#endif

int main(int argc,char* argv[])
{
  Variable X;
  CR* Res;
  List* CR_Results;
  Node* ResNode;
  clock_t my_time1,my_time2;
  double* CR_Result;
  int i;
  char* In_Vars;
  long Max_Points;

  Error = 0;

  Index = 0;
  Res_Index = 0;
  c = 0;

  if(argc < 3)
  {
    printf("Not enough parameters specified, exiting...");
    return -1;
  }

  /* parse a command line and get grid parameters. */
  Count=0;
  In_Vars=argv[1];
  c=In_Vars[0];
  Count++;
  while (c!=';')
  {
    X.Name=c;
    Count++;
    c=In_Vars[Count];
    Count++;
    if ((c!='-')&&(c>='0')&&(c<='9'))
      X.Start=GetDigit(In_Vars);
    else
    {
      c=In_Vars[Count];
      Count++;
      X.Start=-GetDigit(In_Vars);
    };
    c=In_Vars[Count];
    Count++;
    if ((c!='-')&&(c>='0')&&(c<='9'))
      X.Step=GetDigit(In_Vars);
    else
    {
      c=In_Vars[Count];
      Count++;
      X.Step=-GetDigit(In_Vars);
    };
    c=In_Vars[Count];
    Count++;
    X.StepNum=(int)GetDigit(In_Vars);
    RegisterVariable(X);
    printf("%c , %f , %f , %d\n",X.Name,X.Start,X.Step,X.StepNum);
    c=In_Vars[Count];
    Count++;
  };

#ifdef UNKNOWN_CLOCK_PER_SEC
  printf("Timings below are given in processor ticks only.\n");
#endif

  /* allocate memory for an array of CRs */
  CR_Results=(List*)malloc(sizeof(CR*)*(argc-2));

  Count=0;

  /* for each function, defined in a command line, 
     construct a chain of recurrences */
  for(i=2;i<argc;i++)
  {
    printf("%s\n",argv[i]);

    /* construct a chain of recurrences and get timing */
    my_time1 = clock();
    Res = Analyse(argv[i]);
    my_time2 = clock();

#ifdef UNKNOWN_CLOCK_PER_SEC
    printf("Construction takes %li ticks.\n",(long) (my_time2-my_time1));
#else
    printf("Construction takes %li ticks ( %i ms )\n",(long) (my_time2-my_time1),
        (int) ceil((double)(my_time2-my_time1)*time_coeff));
#endif

    /*  copy a constructed CR to the array, print it and 
        free memory of temporary CR */
    printf("Constructed CR: ");
    (*CR_Results)[i-2] = CopyCR(Res);
    ShowCR((*CR_Results)[i-2],stdout);
    printf("\n");
    DestroyCR(Res);
    ResNode=NULL;
    Count=0;
  };

  /* Interpretation of the constructed CRs. */
  Max_Points=1;
  for( i=0; i < Index; i++ )
    Max_Points=Max_Points*((long)(Variables[i].StepNum)+1);

  /* allocating memory for the CR interpretation results */
  printf("Allocating memory for results, %ld bytes.\n",(long)Max_Points*sizeof(double));
  CR_Result=(double*)malloc((long)Max_Points*sizeof(double));
  if(!CR_Result)
  {
    printf("Memory allocation failed!\n");
    return -1;
  }

  for(i=2;i<argc;i++)
  {
    /* call a Prepare function before interpretation to initialize CR */
    if (Prepare( (*CR_Results)[i-2] ) && ( CR_Result != NULL ))
    {
	    my_time1=clock();
           /* compute values of the function over the grid and store in CR_Result */
	    EvalCR((*CR_Results)[i-2],0,CR_Result);
	    my_time2=clock();

#ifdef UNKNOWN_CLOCK_PER_SEC
            printf("Calculation takes %li ticks.\n",(long) (my_time2-my_time1));
#else
	    printf("Calculation takes %li ticks ( %i ms )\n",(long)(my_time2-my_time1),
	        (int) ceil((double)(my_time2-my_time1)*time_coeff));
#endif

	    printf("%f\n",CR_Result[0]);
	    printf("%f\n",CR_Result[Max_Points-1]);
	    Res_Index=0;
    }
    else 
      printf("Error!\n");
  }
  Speller();
  return 0;
}
