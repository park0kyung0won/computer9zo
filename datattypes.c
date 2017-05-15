

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
		printf("ROB%3d ", printed->target_rob);
		printf("%c1:%4d ", (printed->oprd_1.state==V)?'V':'Q', (printed->oprd_1.state == V) ? printed->oprd_1.data.v : printed->oprd_1.data.q);
		printf("%c2:%4d ", (printed->oprd_2.state == V) ? 'V' : 'Q', (printed->oprd_2.state == V) ? printed->oprd_2.data.v : printed->oprd_2.data.q);
		printf("Time left %-1ds", printed->time_left);
	}
	else
		printf("-");
}

void ROB_printer(struct ROB* printed) { INST_printer(printed, false); }


//for reporting
void INST_printer(struct Config* config, bool is_file_print)
{
	
}
void FQ_printer(struct Config* config, bool is_file_print);
void Config_printer(struct Config* config, bool is_file_print);
void RAT_printer(struct Config* config, bool is_file_print );
void RS_printer(struct Config* config, bool is_file_print);
void ROB_printer(struct Config* config, bool is_file_print);