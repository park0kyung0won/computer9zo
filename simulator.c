#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "data_structures.h"

//파일 내 글로벌로 선언
static int inst_length;
static int pc = 0;
static int cycle = 1;

static int decoded = 0;
static int issued = 0;
static int executed = 0;
static int issueWaiting = 0;
static int cnt_IntAlu = 0;
static int cnt_MemRead = 0;
static int cnt_MemWrite = 0;

struct REPORT *core_simulator(struct CONFIG *config, struct INST *arr_inst, int arr_inst_len);

void dump_1(struct ROB *rob, struct CA_status rob_status);
void dump_2(struct ROB *rob, struct CA_status rob_status, struct RS *rs, int rs_size);

void fetch(struct CONFIG *config, struct FQ *fetch_queue, struct CA_status *fq_status, struct INST *arr_inst);
void decode(struct CONFIG *config, struct FQ *fetch_queue, struct CA_status *fq_status, struct RS *rs_ele, struct RAT *rat, struct ROB *rob, struct CA_status *rob_status);
void issue(struct CONFIG *config, struct RS *rs_ele, struct RS *issuewait_list, struct ROB *rob, bool *is_completed_this_cycle, int *issuewait_reference, int i);
void execute(struct CONFIG *config, struct RS *rs_ele, bool *is_issued_this_cycle, int i);
void wait(void);
void rs_retire(struct RS *rs_ele, struct ROB *rob);
void commit(struct CONFIG *config, struct ROB *rob, struct CA_status *rob_status, struct RAT *rat);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct REPORT *core_simulator(struct CONFIG *config, struct INST *arr_inst, int arr_inst_len, FILE* f_report)
{
	//파일 내 글로벌 초기화
	pc = 0;
	cycle = 1;
	inst_length = arr_inst_len;

	decoded = 0;
	issued = 0;
	executed = 0;
	issueWaiting = 0;
	cnt_IntAlu = 0;
	cnt_MemRead = 0;
	cnt_MemWrite = 0;

	// Initializing ...
	int i;
	// RAT
	struct RAT rat[17]; // 0 means no. rat[1] ~ rat[16] are Arch Register entries
	// Fetch Queue
	struct FQ* fetch_queue = calloc(2 * (*config).Width, sizeof(struct FQ) );//struct FQ fetch_queue[2 * (*config).Width]
	struct CA_status fq_status;
	fq_status.size = 2 * (*config).Width;
	fq_status.head = 0;
	fq_status.occupied = 0;
	// Reservation Station
	struct RS* rs = calloc((*config).RS_size, sizeof(struct RS)); //struct RS rs[(*config).RS_size];
	for (i = 0; i < (*config).RS_size; i++)
	{
		rs[i].is_valid = false;
	}
	// Issue waiting list
	struct RS* issuewait_list = calloc((*config).RS_size, sizeof(struct RS)); //struct RS issuewait_list[(*config).RS_size];
	int* issuewait_reference = calloc((*config).RS_size, sizeof(int));//int issuewait_reference[(*config).RS_size];
	// is_completed_this_cycle? to prevent issue of insturction in RS which has V value promoted from Q in current cycle.
	bool* is_completed_this_cycle = calloc((*config).ROB_size, sizeof(bool));//bool is_completed_this_cycle[(*config).ROB_size];
	bool* is_issued_this_cycle = calloc((*config).ROB_size, sizeof(bool));//bool is_issued_this_cycle[(*config).RS_size];
	// ReOrdering Budder
	struct ROB* rob = calloc((*config).ROB_size, sizeof(struct ROB));//struct ROB rob[(*config).ROB_size];
	struct CA_status rob_status;
	rob_status.size = (*config).ROB_size;
	rob_status.head = 0;
	rob_status.occupied = 0;
	
	while (pc < inst_length)
	{	
		decoded = 0;
		issued = 0;
		executed = 0;
		// Flush is_completed_this_cycle array
		for (i = 0; i < (*config).ROB_size; i++)
		{
			is_completed_this_cycle[i] = false;
		}
		// Flush is_issued_this_cycle aaray
		for (i = 0; i < (*config).RS_size; i++)
		{
			is_issued_this_cycle[i] = false;
		}

		// Issue older instructions first
		for (i = 0; i < issueWaiting && i < (*config).Width; i++)
		{
			// Find oldest among them
			struct RS *oldest = &issuewait_list[0];
			int oldest_reference = issuewait_reference[0];
			for (int j = 1; j < issueWaiting; j++)
			{
				if ( issuewait_list[j].time_decoded < (*oldest).time_decoded )
				{
					oldest = &issuewait_list[j];
					oldest_reference = issuewait_reference[j];
				}
			}

			// Issue oldest one
			issue(config, &rs[oldest_reference], issuewait_list, rob, is_completed_this_cycle, issuewait_reference, -1);
			is_issued_this_cycle[oldest_reference] = true;
			issued++;
		}
		issueWaiting = 0;
		
		// Commit ROB -> RAT
		commit(config, rob, &rob_status, rat);
		
		// For every entries in Reservation Station,
		for (i = 0; i < (*config).RS_size; i++)
		{
			if ( rs[i].is_valid ) // Instruction is inside this entry of RS
			{
				switch ( rs[i].time_left)
				{
					case 0:
						rs_retire(&rs[i], rob);
						break;
					case -1:
						issue(config, &rs[i], issuewait_list, rob, is_completed_this_cycle, issuewait_reference, i);
						break;
					default:
						execute(config, &rs[i], is_issued_this_cycle, i);
						break;
				}	
			}
			else                  // This entry of RS is empty now
			{
				// Try to decode instruction into empty place
				decode(config, fetch_queue, &fq_status, &rs[i], rat, rob, &rob_status);
			}	
		}

		// Fetch instructions
		fetch(config, fetch_queue, &fq_status, arr_inst);

		// Dump
		switch ((*config).Dump)
		{
			case 0:
				break;
			case 1:
				fprintf(f_report,"= Cycle %-5d\n", cycle);
				ROB_arr_fprinter(rob, rob_status, f_report);
				break;
			case 2:
				fprintf("= Cycle %-5d\n", cycle);
				RS_arr_fprinter(rs, (*config).RS_size,f_report);
				ROB_arr_fprinter(rob, rob_status, f_report);
				break;
			default:
				printf("= Cycle %-5d\n", cycle);
				//debug mode
				FQ_arr_printer(fetch_queue, fq_status);
				RAT_arr_printer(rat, 17);
				RS_arr_printer(rs, (*config).RS_size);
				ROB_arr_printer(rob, rob_status);
				getchar();
				break;
		}
		cycle++;
	}
	// Simulation finished
	
	//free array
	free(fetch_queue);
	free(rs);
	free(issuewait_list);
	free(issuewait_reference);
	free(is_completed_this_cycle);
	free(is_issued_this_cycle);
	free(rob);
	
	// Write a report
	struct REPORT *ptr_report = malloc(sizeof(struct REPORT));
	(*ptr_report).Cycles = cycle;
	(*ptr_report).Total_Insts = inst_length;
    (*ptr_report).IntAlu = cnt_IntAlu;
	(*ptr_report).MemRead = cnt_MemRead;
	(*ptr_report).MemWrite = cnt_MemWrite;
	(*ptr_report).IPC = ((double)inst_length / (double) cycle);

	return ptr_report;	
}

void fetch(struct CONFIG *config, struct FQ *fetch_queue, struct CA_status *fq_status, struct INST *arr_inst)
{
	int fetch_num = ((*config).Width > (*fq_status).size - (*fq_status).occupied) ? (*fq_status).size - (*fq_status).occupied : (*config).Width;
	int i;
	for (i = 0; i < fetch_num && pc < inst_length; i++)
	{
		fetch_queue[ca_next_pos(fq_status)].opcode = arr_inst[pc].opcode;
		fetch_queue[ca_next_pos(fq_status)].dest = arr_inst[pc].dest;
		fetch_queue[ca_next_pos(fq_status)].oprd_1 = arr_inst[pc].oprd_1;
		fetch_queue[ca_next_pos(fq_status)].oprd_2 = arr_inst[pc].oprd_2;
		ca_cnt_push(fq_status);
		pc++;
	}
}

void decode(struct CONFIG *config, struct FQ *fetch_queue, struct CA_status *fq_status, struct RS *rs_ele, struct RAT *rat, struct ROB *rob, struct CA_status *rob_status)
{
	if ((*fq_status).occupied > 0 && decoded < (*config).Width && (*rob_status).occupied < (*rob_status).size)
	{
		// Putting first element of Fetch Queue to Reservation Station	
		(*rs_ele).is_valid = true;
		(*rs_ele).time_decoded = cycle;
		(*rs_ele).opcode = fetch_queue[(*fq_status).head].opcode;
		// Count Instruction number
		switch ((*rs_ele).opcode)
		{
		case 0:
			cnt_IntAlu++;
			break;
		case 1:
			cnt_MemRead++;
			break;
		case 2:
			cnt_MemWrite++;
			break;
		}

		int oprd_1 = fetch_queue[(*fq_status).head].oprd_1;
		int oprd_2 = fetch_queue[(*fq_status).head].oprd_2;
		if (oprd_1 == 0 || rat[oprd_1].RF_valid)
		{
			(*rs_ele).oprd_1.state = V;
		}
		else
		{
			(*rs_ele).oprd_1.state = Q;
			(*rs_ele).oprd_1.data.q = rat[fetch_queue[(*fq_status).head].oprd_1].Q;
		}

		if (oprd_2 == 0 || rat[oprd_2].RF_valid)
		{
			(*rs_ele).oprd_2.state = V;
		}
		else
		{
			(*rs_ele).oprd_2.state = Q;
			(*rs_ele).oprd_2.data.q = rat[fetch_queue[(*fq_status).head].oprd_2].Q;
		}

		(*rs_ele).time_left = -1; // Waiting to be issued

								  // Putting first element of Fetch Queue to ROB
		rob[ca_next_pos(rob_status)].opcode = fetch_queue[(*fq_status).head].opcode;
		rob[ca_next_pos(rob_status)].dest = fetch_queue[(*fq_status).head].dest;
		rob[ca_next_pos(rob_status)].status = P;

		// Modify RAT status
		if (fetch_queue[(*fq_status).head].dest != 0)
		{
			rat[fetch_queue[(*fq_status).head].dest].RF_valid = false;		// Now data isn't inside the Arch Register file anymore
			rat[fetch_queue[(*fq_status).head].dest].Q = ca_next_pos(rob_status);   // So leave reference to ROB entry in RAT
		}

		ca_cnt_push(rob_status); // Element has been pushed to ROB
		ca_cnt_pop(fq_status);   // Element has been poped from Fetch Queue
		decoded++;
	}
}

void issue(struct CONFIG *config, struct RS *rs_ele, struct RS *issuewait_list, struct ROB *rob, bool *is_completed_this_cycle, int *issuewait_reference, int i)
{
	if (issued < (*config).Width)
	{
		// Q ->V Promotion
		if ((*rs_ele).oprd_1.state == Q && rob[((*rs_ele).oprd_1.data.q)].status)
		{
			(*rs_ele).oprd_1.state = V;
		}

		if ((*rs_ele).oprd_2.state == Q && rob[((*rs_ele).oprd_2.data.q)].status)
		{
			(*rs_ele).oprd_2.state = V;
		}

		// Issue
		if ((*rs_ele).oprd_1.state && (*rs_ele).oprd_2.state && !is_completed_this_cycle[(*rs_ele).rob_dest])
		{
			(*rs_ele).time_left = ((*rs_ele).opcode == MemRead) ? 3 : 1;
		}
		issued++;
	}
	else
	{
		issuewait_list[issueWaiting] = (*rs_ele);
		issuewait_reference[issueWaiting] = i;
		issueWaiting++;
	}
}

void execute(struct CONFIG *config, struct RS *rs_ele, bool *is_issued_this_cycle, int i)
{
	if (!is_issued_this_cycle[i] && executed < (*config).Width)
	{
		(*rs_ele).time_left--;
		executed++;
	}
}

void wait(void)
{
}

void rs_retire(struct RS *rs_ele, struct ROB *rob)
{
	rob[(*rs_ele).rob_dest].status = C;
	(*rs_ele).is_valid = false;
}

void commit(struct CONFIG *config, struct ROB *rob, struct CA_status *rob_status, struct RAT *rat)
{
	int i;
	int num_of_retire = 0;
	for (i = 0; i < (*rob_status).occupied; i++)
	{
		if (rob[i].status == C)
		{
			num_of_retire++;
		}
		else
		{
			break;
		}
	}

	// Only permits upto N commits
	num_of_retire = (*config).Width > num_of_retire ? num_of_retire : (*config).Width;

	for (i = 0; i < num_of_retire; i++)
	{
		rat[rob[i].dest].RF_valid = true;
		ca_cnt_pop(rob_status);
	}
}
