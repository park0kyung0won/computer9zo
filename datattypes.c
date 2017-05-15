

#include <stdio.h>
#include "datatypes.h"


void INST_printer(struct INST* printed)
{//print inst
	printf("%s\t", instruction_name[printed->op]);
	printf("%-5d", printed->dest);
	printf("%-5d", printed->oprd1);
	printf("%-5d\n", printed->oprd2);
}

void FQ_printer(struct FQ* printed)
{
	INST_printer(printed);
}

void Config_printer(struct Config* printed)
{
	printf("Dump = %d, ", printed->Dump);
	printf("Width = %-5d, ", printed->Width);
	printf("ROB size = %-5d, ", printed->ROB_size);
	printf("RS size = %-5d\n", printed->RS_size);
}

void RAT_printer(struct RAT* printed)
{
	printf("%s ", (printed->RF_valid)?"Valid":"Dirty");
	if (printed->RF_valid)
		printf("     \n");
	else
		printf("%-5d\n", printed->Q);
}

void RS_printer(struct RS* printed) 
{
	if (printed->is_valid)
	{
		printf("ROB%5d ", printed->target_rob);
		printf("%s", (printed->oprd_1.state==V)?'V':'Q');
		printf("%s", (printed->oprd_2.state == V) ? 'V' : 'Q',);
		printf("Time left %-2ds\n", printed->time_left);
	}
	else
		printf("-\n");
}

void ROB_printer(struct ROB* printed)
{
	if (printed->dest > 0)
	{
		printf("%s\t", instruction_name[printed->op]);
		printf("R%5d ", printed->dest);
		printf("%c\n", (printed->status==C)?'C':'P');
	}
	else
		printf("-\n");
}


//for reporting
void INST_printer(struct INST* printed, FILE* fileID) {}
void FQ_printer(struct FQ* printed, FILE* fileID) {}
void Config_printer(struct Config* printed, FILE* fileID) {}
void RAT_printer(struct RAT* printed, FILE* fileID) {}
void RS_printer(struct RS* printed, FILE* fileID) {}
void ROB_printer(struct ROB* printed, FILE* fileID) {}