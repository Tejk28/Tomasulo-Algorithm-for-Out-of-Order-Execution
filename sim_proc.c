#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>
#include"sim_proc.h"



FILE *trace_file;

int RobHead,RobTail;
int i;
int current_cycle = 0;
Instr_inPipelinestage *FE,*DE,*RN,*RR,*DI,*IS,*EX,*WB,*ROB;

RMT_struct RMT[Architecural_reg];
int num_of_instr = 0;
int total_cycles = 0;
proc_params params;
int dynamicIC = 0;
double IPC;
int xx = 100;

bool isEmpty(Instr_inPipelinestage *X, int size, char *x)
{
	int i; 
	switch(size)
	{
		case 0:
			for(i = 0; i< params.width; i++)
			{
				if(X[i].Instr_isvalid == NOT_EMPTY)
				{
					return false;
				}
			}
			return true;
		case 1:
			for(i = 0; i< params.iq_size; i++)
			{
				if(X[i].Instr_isvalid == NOT_EMPTY)
				{
					return false;
				}
			}
			return true;
		case 2:
			for(i = 0; i< params.ex_size; i++)
			{
				if(X[i].Instr_isvalid == NOT_EMPTY)
				{
					return false;
				}
			}
			return true;
		case 3:
			for(i = 0; i< params.rob_size; i++)
			{
				if(X[i].Instr_isvalid == NOT_EMPTY)
				{
					return false;
				}
			}
			return true;
		default:
			printf("ERROR: Wrong argument to function isEmpty\n"); 
			return false;
	}
}
bool Advance_Cycle(void)
{
	if(isEmpty(FE,0,"FE") && isEmpty(DE,0,"DE") && isEmpty(RN,0,"RN") && isEmpty(RR,0,"RR") && isEmpty(DI,0,"DI") &&
	   isEmpty(IS,1,"IS") && isEmpty(EX,2,"EX") && isEmpty(WB,2,"WB") && isEmpty(ROB,3,"ROB")/*isEmpty(RT,2)*/)
	{
		return false;
	}
	else
		return true;
}
int main(int argc, char *argv[])
{
	if(argc!=5)
	{
		printf("ERROR: Less or more no of argument than expected\n");
		exit(0);
	}	
	
	params.rob_size = atoi(argv[1]);
	params.iq_size  = atoi(argv[2]);
	params.width    = atoi(argv[3]);
	params.ex_size 	 = params.width*5;
	RobHead	 = 0;
	RobTail	 = 0;
	
	trace_file = fopen(argv[4],"r");
	if(trace_file == NULL)
	{
		printf("ERROR: Unable to open file %s\n",argv[4]);
		return 1;
	}
	//ROB is created and initalized to invalid
	ROB = malloc(params.rob_size* sizeof(Instr_inPipelinestage));
	for(i = 0; i< params.rob_size;i++)
	{
		ROB[i].Instr_isvalid = INVALID;  
	}
	
        //all values of registers are stored in ARF
	for(i = 0; i< Architecural_reg; i++)
	{
		RMT[i].where = is_inARF; 
		RMT[i].tag   = i;
	}

        //fetch stage is initialized to invalid instructions
	FE = malloc(params.width*sizeof(Instr_inPipelinestage));
	for(i = 0; i<params.width; i++)
	{
		FE[i].Instr_isvalid = INVALID;  
	}

        	
        //decode stage is initialized to invalid instructions
	DE = malloc(params.width*sizeof(Instr_inPipelinestage));
	for(i = 0; i<params.width; i++)
	{
		DE[i].Instr_isvalid = INVALID;  
	}

        
        //rename stage is initialized to invalid instructions
	RN = malloc(params.width*sizeof(Instr_inPipelinestage));
	for(i = 0; i<params.width; i++)
	{
		RN[i].Instr_isvalid = INVALID; 
	}
        
        //regread stage is initialized to invalid instructions
	RR = malloc(params.width*sizeof(Instr_inPipelinestage));
	for(i = 0; i<params.width; i++)
	{
		RR[i].Instr_isvalid = INVALID;  
	}
       
        //dispatch stage is initialized to invalid instructions
	DI = malloc(params.width*sizeof(Instr_inPipelinestage));
	for(i = 0; i<params.width; i++)
	{
		DI[i].Instr_isvalid = INVALID;
	}
       
        //issue stage is initialized to invalid instructions
	IS = malloc(params.iq_size*sizeof(Instr_inPipelinestage));
	{
		IS[i].Instr_isvalid = INVALID; 		
	}
        
        //execute stage is initialized to invalid instructions
	EX = malloc(params.ex_size*sizeof(Instr_inPipelinestage));
	{
		EX[i].Instr_isvalid = INVALID; 
	}
       
        //writeback stage is initialized to invalid instructions
	WB = malloc(params.ex_size*sizeof(Instr_inPipelinestage));
	{
		WB[i].Instr_isvalid = INVALID; 		
	}
	
	do
	{
		Retire();
		Writeback();
		Execute();
		Issue();
		Dispatch();
		RegRead();
		Rename();
		Decode();
		Fetch();
	}
	while(Advance_Cycle());
		
	IPC = (double)(num_of_instr)/total_cycles;
	printf("# === Simulator Command =========\n");
	printf("# ./sim %lu %lu %lu %s\n",params.rob_size,params.iq_size,params.width,argv[4]);
	printf("# === Processor Configuration ===\n");
	printf("# ROB_SIZE = %lu\n",params.rob_size);
	printf("# IQ_SIZE  = %lu\n",params.iq_size);
	printf("# WIDTH    = %lu\n",params.width);
	printf("# === Simulation Results ========\n");
	printf("# Dynamic Instruction Count    = %0d\n",num_of_instr);
	printf("# Cycles                       = %0d\n",total_cycles);
	printf("# Instructions Per Cycle (IPC) = %.2f\n",IPC);
	return 0;
}
