#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>
#include"sim_proc.h"

extern FILE *trace_file;

extern int RobHead,RobTail;

extern int current_cycle, dynamicIC;

extern Instr_inPipelinestage *FE,*DE,*RN,*RR,*DI,*IS,*EX,*WB,*ROB;

extern RMT_struct RMT[Architecural_reg];

proc_params params;
extern int num_of_instr,total_cycles;
int i, j, k;

char line[256];
char *strtag, *strlatency, *strdst, *strsrc1, *strsrc2; 
int tag, latency, dst, src1, src2;
int age = 0;

bool RobAvailable(void)
{
	int Rob_space =0;
	for(i =0; i<params.rob_size;i++)
	{
		if(ROB[i].Instr_isvalid == INVALID)
		{
			Rob_space++;
		}
	}
	if(Rob_space>=params.width)
		return true;
	else
		return false;
}

int free_IQ_Entries(void)
{
         // to check how many free entries are there in issue queue.
	int j;
	int free_Entries = 0; 
	for(j = 0; j <params.iq_size; j++)
	{
		if(IS[j].Instr_isvalid == INVALID)
		{
			free_Entries++;
		}
	}
	return free_Entries;
}

int Dispatch_Bundle_size(void)
{
	int size = 0;
	for(i = 0; i < params.width; i++)
	{
		if(DI[i].Instr_isvalid == VALID)
		{
			size++;
		}
	}
	return size;
}
void sortIS(Instr_inPipelinestage *IS)
{
	Instr_inPipelinestage temp;
	for(i = 0; i<params.iq_size;i++)
	{
		for(j = i+1; j<params.iq_size; j++)
		{
			if(IS[i].instr.age > IS[j].instr.age)
			{
				temp = IS[i];
				IS[i] = IS[j];
				IS[j] = temp;
			}
		}
	}
}


//Fetch function
void Fetch(void)
{
        //start operation only if Decode is empty

	if(isEmpty(DE,0,"DE"))
	{
		for(i = 0; i< params.width; i++)
		{           
                        //extract op_type, source resgister and desitination registers from trace file. break the string using strok function.
			if(NULL != fgets(line,sizeof(line),trace_file))
			{
				dynamicIC++;
				strtag = strtok(line," ");
				if(strtag!=NULL)
				{
					sscanf(strtag,"%x",&tag);
				}	
				
				strlatency = strtok(NULL," ");
				if(strlatency!=NULL)
				{
					sscanf(strlatency,"%d",&latency);
				}					
				
				strdst = strtok(NULL," ");
				if(strdst!=NULL)
				{
					sscanf(strdst,"%d",&dst);
				}		
				
				strsrc1 = strtok(NULL," ");
				if(strsrc1!=NULL)
				{
					sscanf(strsrc1,"%d",&src1);
				}				
				
				strsrc2 = strtok(NULL," ");
				if(strsrc2!=NULL)
				{
					sscanf(strsrc2,"%d",&src2);
				}				
			
				//when decode is empty, change the valid bit of decode to valid and increment the age of instruction after every arrival
				DE[i].Instr_isvalid = VALID;
				DE[i].instr.tag = tag;
				DE[i].instr.dst = dst;
				DE[i].instr.dst_spl = dst;
				DE[i].instr.dst_Ready = notReady;
				DE[i].instr.src1 = src1;
				DE[i].instr.src1_spl = src1;
				DE[i].instr.age = age;
				age++;
				if(src1 == -1)
				{
					DE[i].instr.src1_Ready = READY;
				}
				else
				{
					DE[i].instr.src1_Ready = notReady;				
				}
				DE[i].instr.src2 = src2;
				DE[i].instr.src2_spl = src2;
				if(src2 == -1)
				{
					DE[i].instr.src2_Ready = READY;
				}
				else
				{
					DE[i].instr.src2_Ready = notReady;				
				}

                                //if both src reg are ready, the instruction is fetch and begin time is current cycle, duration in fetch will be one for inital state 
                                //and hence decode arrival will be current cycle + 1
				DE[i].instr.Fetch_arrival = current_cycle;
				DE[i].instr.Fetch_duration = 1;
				DE[i].instr.Decode_arrival = current_cycle+1;
				
                                //check for opertaion typr i.e. latency, if the execution would take 1, 2 or 5 cycle.
				switch(latency)
				{
					case 0:
						DE[i].instr.latency = 1;
						DE[i].instr.lat = 0;
					break;
					case 1:
						DE[i].instr.latency = 2;
						DE[i].instr.lat = 1;
					break;
					case 2: 
						DE[i].instr.latency = 5;
						DE[i].instr.lat = 2;
					break;
				}
				
				
			}
		}
	}
	current_cycle++;
}


void Decode(void)
{
	//Decode function
	if(isEmpty(RN,0,"RN"))
	{
		for(i = 0; i< params.width; i++)
		{
			if(DE[i].Instr_isvalid)
			{
                               
				RN[i] = DE[i];
				RN[i].Instr_isvalid = VALID;
				RN[i].instr.Rename_arrival = current_cycle+1;
				RN[i].instr.Decode_duration = (RN[i].instr.Rename_arrival) - (RN[i].instr.Decode_arrival);
				DE[i].Instr_isvalid = INVALID;
			}
		}
		
	}
}



void Rename(void)
{
	if(RobAvailable())
	{
		if(isEmpty(RR,0,"RR"))
		{
			for(i = 0; i< params.width; i++)
			{
				if(RN[i].Instr_isvalid)
				{
					ROB[RobTail] = RN[i];
								
					if(ROB[RobTail].instr.src1 != -1)
					{
                                                //check if the instr is in ROB table or not, if found in ROB then src1 is renamed with ROB tag.
						if( RMT[ROB[RobTail].instr.src1].where == is_inROB )
						{
							RN[i].instr.src1 = RMT[ROB[RobTail].instr.src1].tag;
						}
					}
					if(ROB[RobTail].instr.src2 != -1)
					{
					       // similar check for src2, and rename of src2
                                            	if( RMT[ROB[RobTail].instr.src2].where == is_inROB )
						{
							(RN[i].instr.src2) = RMT[ROB[RobTail].instr.src2].tag;
						}
					}
					if(ROB[RobTail].instr.dst != -1)
					{
                                                //destination reg is renamed
						RMT[ROB[RobTail].instr.dst].where = is_inROB;	
						RMT[ROB[RobTail].instr.dst].tag = (RobTail+100);
						RN[i].instr.dst = RobTail+100;
					}
					else
					{
						RN[i].instr.dst = RobTail+100;
					}
					RR[i] = RN[i];
					RR[i].Instr_isvalid = VALID;
					RR[i].instr.RegRead_arrival = current_cycle+1;
					RR[i].instr.Rename_duration = (RR[i].instr.RegRead_arrival)-(RR[i].instr.Rename_arrival); 
					RN[i].Instr_isvalid = INVALID;
				
                                        //once the rob table is comes to the end , it needs to be initalized to 0.
					if(RobTail == (params.rob_size-1) )
					{
						RobTail = 0;
					}
					else
					{
						RobTail ++;
					}
				}
			}
		}
	}
	
}



void RegRead(void)
{
	//printf("Regread\n");
	if(isEmpty(DI,0,"DI"))
	{
		for(i = 0; i< params.width; i++)
		{
			if(RR[i].Instr_isvalid)
			{
				if(RR[i].instr.src1 != -1)
				{
					if((RR[i].instr.src1<67))
					{
						RR[i].instr.src1_Ready = READY;
					}
					else if((RR[i].instr.src1>=100))
					{
						if(ROB[RR[i].instr.src1-100].instr.dst_Ready)
						{
							RR[i].instr.src1_Ready = READY;
						}
					}
				}
				else
				{
						RR[i].instr.src1_Ready = READY;
				}
				
				if(RR[i].instr.src2 != -1)
				{
					if((RR[i].instr.src2<67))
					{
						RR[i].instr.src2_Ready = READY;
					}
					else if((RR[i].instr.src2>=100))
					{
						if(ROB[RR[i].instr.src2-100].instr.dst_Ready)
						{
							RR[i].instr.src2_Ready = READY;
						}
					}
				}
				else
				{
						RR[i].instr.src2_Ready = READY;
				}
				
				DI[i] = RR[i];
				DI[i].Instr_isvalid = VALID;
				DI[i].instr.Dispatch_arrival = current_cycle+1;
				DI[i].instr.RegRead_duration = (DI[i].instr.Dispatch_arrival)-(DI[i].instr.RegRead_arrival); 
				RR[i].Instr_isvalid = INVALID;
			}
		}
	}
	
}


void Dispatch(void)
{
	if(free_IQ_Entries()>= Dispatch_Bundle_size())
	{
		for(i = 0; i< params.width; i++)
		{
			if(DI[i].Instr_isvalid == VALID)
			{
				for(j = 0; j< params.iq_size; j++ )
				{
	
					if((IS[j].Instr_isvalid == INVALID))
					{
						IS[j] = DI[i];
						IS[j].Instr_isvalid = VALID; 
						IS[j].instr.Issue_arrival = current_cycle+1;
						IS[j].instr.Dispatch_duration = (IS[j].instr.Issue_arrival)-(IS[j].instr.Dispatch_arrival); 
						DI[i].Instr_isvalid = INVALID;
						break;
					}
				}
			}
		}
	}
}


void Issue(void)
{
	int issue_count = 0;
	sortIS(IS);
	//printf("sorted ISSUE \n");
		for(j = 0; j< params.iq_size ; j++)
		{
			if(IS[j].Instr_isvalid)
			{
				if((IS[j].instr.src1_Ready == READY) && (IS[j].instr.src2_Ready == READY) )
				{
					for(k = 0; k< params.ex_size; k++)
					{
						if(EX[k].Instr_isvalid == INVALID)
						{
							issue_count++;
							EX[k] = IS[j];
							EX[k].Instr_isvalid = VALID ;
							EX[k].instr.Execute_arrival = current_cycle+1;
							EX[k].instr.Issue_duration = (EX[k].instr.Execute_arrival)-(EX[k].instr.Issue_arrival); 
							IS[j].Instr_isvalid = INVALID;
							
							if(issue_count == params.width)
							{	
								return;
							}
							break;
						}
					}
				}
			}
		}
}



void Execute(void)
{
	//printf("Execute\n");
	for(i = 0; i< params.ex_size; i++)
	{
		if(EX[i].Instr_isvalid)
		{
			EX[i].instr.latency--;
			if(EX[i].instr.latency == 0)
			{
				for(k = 0; k<params.iq_size; k++)
                                     //Wakeup is sent after the execution to IQ that in next cycle EX would be able to issue an instr from Issue queue
				    //for wakeup, check the valid bit if issue stage, and if there is a RAW dependency, set ready bit for src1
                               {
					if(IS[k].Instr_isvalid && (EX[i].instr.dst != -1) && (EX[i].instr.dst == IS[k].instr.src1))
					{
						IS[k].instr.src1_Ready = READY;
					}
					if(IS[k].Instr_isvalid && (EX[i].instr.dst != -1) && (EX[i].instr.dst == IS[k].instr.src2))
					{
						IS[k].instr.src2_Ready = READY;
					}
				}
				
                                //EX also sends wakeup to dispatch bundle
				for(k = 0; k<params.width; k++) 
				{
					if(DI[k].Instr_isvalid && (EX[i].instr.dst != -1) && (EX[i].instr.dst == DI[k].instr.src1))
					{
						DI[k].instr.src1_Ready = READY;
					}
					if(DI[k].Instr_isvalid &&(EX[i].instr.dst != -1) && (EX[i].instr.dst == DI[k].instr.src2))
					{
						DI[k].instr.src2_Ready = READY;
					}
				}
				//Wakeup to RR
				for(k = 0; k<params.width; k++) 
				{
					if(RR[k].Instr_isvalid && (EX[i].instr.dst != -1) &&(EX[i].instr.dst == RR[k].instr.src1))
					{
						RR[k].instr.src1_Ready = READY;
					}
					if(RR[k].Instr_isvalid && (EX[i].instr.dst != -1) && (EX[i].instr.dst == RR[k].instr.src2))
					{
						RR[k].instr.src2_Ready = READY;
					}
				}

				for(j = 0; j<params.ex_size; j++)
				{
					if(WB[j].Instr_isvalid == INVALID)
					{
						WB[j] = EX[i];
						WB[j].Instr_isvalid = VALID;
						WB[j].instr.Writeback_arrival = current_cycle+1;
						WB[j].instr.Execute_duration = (WB[j].instr.Writeback_arrival)-(WB[j].instr.Execute_arrival); 
						WB[j].instr.dst_Ready = READY;
						EX[i].Instr_isvalid = INVALID;
						break;
					}
				}
			}
		}		
	}
}

void Writeback(void)
{

        for(i = 0; i< params.ex_size; i++)

        {
        if(WB[i].Instr_isvalid)
		{
			ROB[WB[i].instr.dst-100]= WB[i];
			ROB[WB[i].instr.dst-100].Instr_isvalid = VALID; 
			ROB[WB[i].instr.dst-100].instr.Retire_arrival = current_cycle+1;
			ROB[WB[i].instr.dst-100].instr.Writeback_duration = (ROB[WB[i].instr.dst-100].instr.Retire_arrival)-(ROB[WB[i].instr.dst-100].instr.Writeback_arrival); 
			ROB[WB[i].instr.dst-100].instr.dst_Ready = READY;
			WB[i].Instr_isvalid = INVALID;
		}

        }   


}


void Retire(void)
{
	//printf("Retire\n");
	int retire_count = 0;
	for(i = 0; i< params.width;i++)
	{
		if((ROB[RobHead].instr.dst_Ready) && (ROB[RobHead].Instr_isvalid == VALID))
		{	
			ROB[RobHead].instr.Retire_duration = (current_cycle+1) - ROB[RobHead].instr.Retire_arrival;
			if(ROB[RobHead].instr.dst_spl != -1)
			{
				if(RMT[ROB[RobHead].instr.dst_spl].where == is_inROB)
				{
					if(RMT[ROB[RobHead].instr.dst_spl].tag == (RobHead+100))
					{
						RMT[ROB[RobHead].instr.dst_spl].where = is_inARF;
					}
				}
			}
			ROB[RobHead].Instr_isvalid = INVALID;
			retire_count++;
			printf("%d fu{%d} src{%d,%d} dst{%d} FE{%d,%d} DE{%d,%d} RN{%d,%d} RR{%d,%d} DI{%d,%d} IS{%d,%d} EX{%d,%d} WB{%d,%d} RT{%d,%d}\n",
					num_of_instr,ROB[RobHead].instr.lat,ROB[RobHead].instr.src1_spl,ROB[RobHead].instr.src2_spl,ROB[RobHead].instr.dst_spl,
					ROB[RobHead].instr.Fetch_arrival,ROB[RobHead].instr.Fetch_duration, 
					ROB[RobHead].instr.Decode_arrival, ROB[RobHead].instr.Decode_duration,
					ROB[RobHead].instr.Rename_arrival,ROB[RobHead].instr.Rename_duration,
					ROB[RobHead].instr.RegRead_arrival,ROB[RobHead].instr.RegRead_duration,
					ROB[RobHead].instr.Dispatch_arrival,ROB[RobHead].instr.Dispatch_duration,
					ROB[RobHead].instr.Issue_arrival,ROB[RobHead].instr.Issue_duration,
					ROB[RobHead].instr.Execute_arrival,ROB[RobHead].instr.Execute_duration,
					ROB[RobHead].instr.Writeback_arrival,ROB[RobHead].instr.Writeback_duration,
					ROB[RobHead].instr.Retire_arrival,ROB[RobHead].instr.Retire_duration);	
					num_of_instr++;
					total_cycles = ROB[RobHead].instr.Retire_arrival+ROB[RobHead].instr.Retire_duration;
			
                        //if Rob head reaches the last location of table, we need to reset the RobHead to 0.
			if(RobHead == (params.rob_size-1))
			{
				RobHead = 0;
			}
			else
			{
				RobHead++;
			}
		}
		else
		{
			return;
		}
		
	}
}
