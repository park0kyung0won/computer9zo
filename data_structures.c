#define DUMP_WIDTH 4  //어레이 형태로 레지스터 출력시 가로 줄 개수

#include <stdio.h>
#include "data_structures.h"

const char* instruction_name[3] = { "IntALU", "MemRead" , "MemWrite" };

void INST_printer(const struct INST* printed)
{//print inst
	printf("%-10s", instruction_name[printed->opcode]);
	printf("%5d", printed->dest);
	printf("%5d", printed->oprd_1);
	printf("%5d", printed->oprd_2);
}

void FQ_printer(const struct FQ* printed)
{
	INST_printer((struct INST*)printed);
}

void Config_printer(const struct CONFIG* printed)
{
	printf("Dump = %d, ", printed->Dump);
	printf("Width =%5d, ", printed->Width);
	printf("ROB size =%5d, ", printed->ROB_size);
	printf("RS size =%5d", printed->RS_size);
}

void RAT_printer(const struct RAT* printed)
{
	printf("%s ", (printed->RF_valid)?"Valid":"Dirty");
	if (printed->RF_valid)
		printf("    -");
	else
		printf("%5d", printed->Q);
}

void RS_printer(const struct RS* printed)
{
	if (printed->is_valid)
	{
		printf("ROB%-5d", printed->rob_dest+1);
		
		if (printed->oprd_1.state == V){printf("V     ");}
		else{printf("Q:%-3d ", printed->oprd_1.data.q);}		
		
		if (printed->oprd_2.state == V) { printf("V     "); }
		else { printf("Q:%-3d ", printed->oprd_2.data.q); }

		printf("left%2d", printed->time_left);
	}
	else
		printf("                          ");
}

void ROB_printer(const struct ROB* printed)
{
	if (printed->dest > 0)
	{
		printf("%-10s  ", instruction_name[printed->opcode]);
		printf("R%-5d   ", printed->dest);
		printf("%c    ", (printed->status==C)?'C':'P');
	}
	else
		printf("                          ");
}

void FQ_arr_printer(const struct FQ* fq, struct CA_status fq_status)
{
	printf("Fetch queue\n");

	struct FQ* fq_idx = NULL;
	int idx;
	for (idx = 0; idx < fq_status.size; ++idx)
	{
		if (idx < fq_status.occupied)
		{
			fq_idx = fq + ((fq_status.head + idx) % fq_status.size);
			printf("| FQ%-4d: ", idx + 1);
			FQ_printer(fq_idx);
		}
		else
		{
			printf("| FQ%-4d:                          ", idx + 1);
		}
		printf(" ", idx + 1);
		
		if (idx % DUMP_WIDTH == DUMP_WIDTH - 1)
		{
			printf("|\n");
		}
	}
	if (idx % DUMP_WIDTH != 0)
	{
		printf("\n");
	}
}

void RAT_arr_printer(const struct RAT* rat, int rat_size)
{
	printf("RAT\n");
	struct RAT *rat_idx = NULL;
	int idx;
	for (idx = 0; idx < rat_size-1; ++idx)
	{
		rat_idx = rat + (idx+1);
		printf("| R%-4d : ", idx);
		RAT_printer(rat_idx);

		printf(" ", idx + 1);
		if (idx % DUMP_WIDTH == DUMP_WIDTH - 1)
		{
			printf("|\n");
		}
	}
	if (idx % DUMP_WIDTH != 0)
	{
		printf("\n");
	}
}

void RS_arr_printer(struct RS *rs, int rs_size)
{
	printf("Reservation station\n");
	struct RS *rs_idx = NULL;
	int idx;
	for (idx = 0; idx < rs_size; ++idx)
	{
		rs_idx = rs + (idx);
		printf("| RS%-4d : ", idx + 1);
		RS_printer(rs_idx);

		printf("        ", idx + 1);
		if (idx % DUMP_WIDTH == DUMP_WIDTH - 1)
		{
			printf("|\n");
		}
	}
	if (idx % DUMP_WIDTH != 0)
	{
		printf("\n");
	}
}

void ROB_arr_printer(const struct ROB *rob, struct CA_status rob_status)
{
	printf("Reorder buffer\n");

	struct ROB *rob_idx = NULL;
	int idx;
	for (idx = 0; idx < rob_status.size; ++idx)
	{
		rob_idx = rob + ((rob_status.head + idx) % rob_status.size);
		printf("| ROB%-4d: ", idx + 1);
		ROB_printer(rob_idx);
		printf(" ID%-4d", (rob_status.head + idx) % rob_status.size);

		printf(" ", idx + 1);
		if (idx % DUMP_WIDTH == DUMP_WIDTH - 1)
		{
			printf("|\n");
		}
	}
	if (idx % DUMP_WIDTH != 0)
	{
		printf("\n");
	}
}

//for reporting
void RS_fprinter(const struct RS* printed, FILE* fileID)
{
	if (printed->is_valid)
	{
		fprintf(fileID,"ROB%-5d", printed->rob_dest+1);
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

void REPORT_fprinter(const struct REPORT* printed, FILE* fileID)
{
	fprintf(fileID, "%15s%d\n", "Cycles",printed->Cycles);
	fprintf(fileID, "%15s%f\n", "IPC", printed->IPC);
	fprintf(fileID, "%15s%d\n", "Total Insts", printed->Total_Insts);
	fprintf(fileID, "%15s%d\n", "IntAlu", printed->IntAlu);
	fprintf(fileID, "%15s%d\n", "MemRead", printed->MemRead);
	fprintf(fileID, "%15s%d\n", "MemWrite", printed->MemWrite);
}

void RS_arr_fprinter(struct RS *rs, int rs_size, FILE* fileID)
{
	struct RS *rs_idx = NULL;
	int idx;
	for (idx = 0; idx < rs_size; ++idx)
	{
		rs_idx = rs + (idx);
		fprintf(fileID,"RS%-4d : ", idx + 1);
		RS_fprinter(rs_idx, fileID);
		fprintf(fileID, "\n");
	}
}

void ROB_arr_fprinter(const struct ROB *rob, struct CA_status rob_status, FILE* fileID)
{
	struct ROB *rob_idx = NULL;
	int idx;
	for (idx = 0; idx < rob_status.size; ++idx)
	{
		rob_idx = rob + ((rob_status.head + idx) % rob_status.size);
		printf(fileID,"ROB%-4d: ", idx + 1);
		ROB_fprinter(rob_idx, fileID);
		fprintf(fileID, "\n");
	}
}

//for ca using
void ca_cnt_push(struct CA_status *status)
{
	(*status).occupied++;
}

void ca_cnt_pop(struct CA_status *status)
{
	(*status).head = ((*status).head + 1) % (*status).size;
	(*status).occupied--;
}

int ca_next_pos(struct CA_status *status)
{
	return (*status).head + (*status).occupied;
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
