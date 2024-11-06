#ifndef SIM_PROC_H
#define SIM_PROC_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>

#define Architecural_reg 67
#define is_inARF false
#define is_inROB true
#define EMPTY false
#define NOT_EMPTY true
#define VALID true
#define INVALID false
#define READY true
#define notReady false

typedef struct proc_params{
    unsigned long int rob_size;
    unsigned long int iq_size;
    unsigned long int width;
                  int ex_size;
}proc_params;

typedef struct
{
	int tag;
	int dst;
	int dst_spl;
	bool dst_Ready;
	int src1;
	int src1_spl;
	bool src1_Ready;
	int src2;
	int src2_spl;
	bool src2_Ready;
	int latency;
	int lat;
	int Fetch_arrival;
	int Fetch_duration;
	int Decode_arrival;
	int Decode_duration;
	int Rename_arrival;
	int Rename_duration;
	int RegRead_arrival;
	int RegRead_duration;
	int Dispatch_arrival;
	int Dispatch_duration;
	int Issue_arrival;
	int Issue_duration;
	int Execute_arrival;
	int Execute_duration;
	int Writeback_arrival;
	int Writeback_duration;
	int Retire_arrival;
	int Retire_duration;	
	int age;
}instruction_variables;

typedef struct
{
	bool Instr_isvalid;
	instruction_variables instr;
}Instr_inPipelinestage;

typedef struct
{
	bool where;
	int tag;
}RMT_struct;

bool Advance_Cycle(void);
bool isEmpty(Instr_inPipelinestage *X, int size, char *x);
void Retire(void);
void Writeback(void);
void Execute(void);
void Issue(void);
void Dispatch(void);
void RegRead(void);
void Rename(void);
void Decode(void);
void Fetch(void);
bool RobAvailable(void);
int  Dispatch_Bundle_size(void);
int  free_IQ_Entries(void);

#endif
