

#include <stdio.h>
#include "datatypes.h"

const char* instruction_name[3] = { "IntALU", "MemRead" , "MemWrite" };

void INST_printer(const struct INST* printed)
{//print inst
	printf("%-10s", instruction_name[printed->op]);
	printf("%5d", printed->dest);
	printf("%5d", printed->oprd1);
	printf("%5d\n", printed->oprd2);
}

void FQ_printer(const struct FQ* printed)
{
	INST_printer((struct INST*)printed);
}

void Config_printer(const struct Config* printed)
{
	printf("Dump = %d, ", printed->Dump);
	printf("Width =%5d, ", printed->Width);
	printf("ROB size =%5d, ", printed->ROB_size);
	printf("RS size =%5d\n", printed->RS_size);
}

void RAT_printer(const struct RAT* printed)
{
	printf("%s ", (printed->RF_valid)?"Valid":"Dirty");
	if (printed->RF_valid)
		printf("    -\n");
	else
		printf("%5d\n", printed->Q);
}

void RS_printer(const struct RS* printed)
{
	if (printed->is_valid)
	{
		printf("ROB%-5d ", printed->target_rob);
		printf("%c    ", (printed->oprd_1.state == V)?'V':'Q');
		printf("%c    ", (printed->oprd_2.state == V)?'V' :'Q');
		printf("Time left %2ds\n", printed->time_left);
	}
	else
		printf("RS BLANK\n");
}

void ROB_printer(const struct ROB* printed)
{
	if (printed->dest > 0)
	{
		printf("%s\t", instruction_name[printed->op]);
		printf("R%-5d ", printed->dest);
		printf("%c\n", (printed->status==C)?'C':'P');
	}
	else
		printf("ROB BLANK\n");
}


//for reporting
//void INST_printer(struct INST* printed, FILE* fileID) {}
//void FQ_printer(struct FQ* printed, FILE* fileID) {}
//void Config_printer(struct Config* printed, FILE* fileID) {}
//void RAT_printer(struct RAT* printed, FILE* fileID) {}
void RS_fprinter(const struct RS* printed, FILE* fileID)
{
	if (printed->is_valid)
	{
		fprintf(fileID,"ROB%-5d", printed->target_rob);
		(printed->oprd_1.state == V) ? fprintf(fileID,"    V") : fprintf(fileID,"%5d",printed->oprd_1.data.q);
		(printed->oprd_2.state == V) ? fprintf(fileID, "    V") : fprintf(fileID, "%5d", printed->oprd_2.data.q);
		fprintf(fileID, "\n");
	}
	else
		fprintf(fileID,"BLANK       -    -\n");
}

void ROB_fprinter(const struct ROB* printed, FILE* fileID)
{
	if (printed->dest > 0)
		fprintf(fileID,"%c\n", (printed->status==C)?'C':'P');
	else
		fprintf(fileID,"P\n");
}

//
//#include <stdlib.h>
//
//
//int main(void)
//{
//	struct INST inst = { 2,2,2,3 };
//	struct FQ fq = { 1,2,2,3 };
//	struct Config config = { 0 ,1 , 2 , 3 };
//	struct RAT rat1 = { Q , 12 };
//	struct RAT rat2 = { V , 12 };
//	struct RS rs1 = { 10, false ,1, 2, {V, 12},{ Q, 13 } };
//	struct RS rs2 = { 10, true ,2, -2,{ V, 12 },{ Q, 13 } };
//	struct ROB rob1 = { 0 ,1 , C };
//	struct ROB rob2 = { 0 ,2 , P };
//	struct ROB rob3 = { 1 ,-2 , P };
//
//	INST_printer(&inst);
//	FQ_printer(&fq);
//	Config_printer(&config);
//	RAT_printer(&rat1);
//	RAT_printer(&rat2);
//	RS_printer(&rs1);
//	RS_printer(&rs2);
//	ROB_printer(&rob1);
//	ROB_printer(&rob2);
//	ROB_printer(&rob3);
//
//	system("PAUSE");
//
//	FILE* fid=fopen("test.out", "w");
//	RS_fprinter(&rs1, fid);
//	RS_fprinter(&rs2, fid);
//	ROB_fprinter(&rob1, fid);
//	ROB_fprinter(&rob3, fid);
//	fclose(fid);
//}
//
