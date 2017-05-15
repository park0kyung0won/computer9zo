#include "simul.h"
#include "datatypes.h"

void fetch(int *pc, int fetch_width, int inst_length, struct INST *inst, struct FQ* fetch_queue, struct status_cyc_arr* idx)
{
	int fetch_num = (fetch_width > (*idx).size - (*idx).occupied) ? (*idx).size - (*idx).occupied : fetch_width;
	int i;
	for (i = 0; i < fetch_num && (*pc) < inst_length; i++)
	{
		(fetch_queue[(*idx).head]).op = (inst[*pc]).op;
		(fetch_queue[(*idx).head]).dest = (inst[*pc]).dest;
		(fetch_queue[(*idx).head]).oprd1 = (inst[*pc]).oprd1;
		(fetch_queue[(*idx).head]).oprd2 = (inst[*pc]).oprd2;
		++(*pc);
		(*idx).head = (((*idx).head + 1) % (*idx).size); 
	}

}


// fetch queue랑 RS, ROB, RT를 포인터로 받아서
// 인스트럭션을 fetch_queue에서 가져와
// RS와 ROB에 넣어준다
// (RS에 넣을 때, operand 둘 중 하나라도 Q면 time을 -1로 지정해 놓는다)
// 그리고 output에 해당하는 RT의 RF_VALID를 수정해준다
void decode(struct FQ* fq, struct Cycle_index* index_fq, struct RS* rs, struct RAT* rat, struct ROB* rob,struct Cycle_index* idx_rob)
{
	//regist in rob
	rob[(*idx_rob).tail].dest = fq[(*index_fq).head].dest;
	rob[(*idx_rob).tail].op = fq[(*index_fq).head].op;
	rob[(*idx_rob).tail].status = P;
		//rob[(*idx_rob).tail].value = 0;
		
	//rat change
	rat[fq[(*index_fq).head].dest].RF_valid = false;
	rat[fq[(*index_fq).head].dest].Q = (*idx_rob).tail;

	//regist in rs
	(*rs).index_rob = (*idx_rob).tail;
	(*rs).is_valid = true;
	(*rs).op = fq[(*index_fq).head].op;
	if (rat[fq[(*index_fq).head].oprd1].RF_valid)
	{//if rat true
		(*rs).oprd_1.state = V;
		(*rs).oprd_1.data.v = fq[(*index_fq).head].oprd1;
	}
	else
	{
		(*rs).oprd_1.state = Q;
		(*rs).oprd_1.data.q = rat[fq[(*index_fq).head].oprd1].Q;
	}

	if ((*rs).op != IntAlu || rat[fq[(*index_fq).head].oprd2].RF_valid)
	{//if rat true
		(*rs).oprd_2.state = V;
		(*rs).oprd_2.data.v = fq[(*index_fq).head].oprd2;
	}
	else
	{
		(*rs).oprd_2.state = Q;
		(*rs).oprd_2.data.q = rat[fq[(*index_fq).head].oprd2].Q;
	}
	(*rs).time_left = -1;
	
	//update head and tail
	move_cidx_tail(idx_rob, 1);
	move_cidx_head(index_fq, 1);
}

// RS를 포인터로 받아서
// time이 -1인 것들에 대해
// operand 모두가 V 상태이라면
// time을 -1에서 '수행에 걸리는 시간' 값으로 바꿔준다. ( 실행 시작 )
void issue( struct RS* rs, struct ROB* rob, bool* is_completed_this_cycle )
{
	if ((*rs).oprd_1.state == Q && rob[((*rs).oprd_1.data.q)].status == C)
	{//기다리던 값 계산이 끝났으면 받아온다.
		(*rs).oprd_1.state = V;
	}
	if ((*rs).oprd_2.state == Q && rob[((*rs).oprd_2.data.q)].status == C)
	{
		(*rs).oprd_2.state = V;
	}

	if ((*rs).oprd_1.state == V && (*rs).oprd_2.state == V)
	{//만약 값이 다 받아졌으면 이슈한다
		(*rs).time_left = ((*rs).op == MemRead) ? 4 : 1;
	}
}

// RS랑 ROB를 포인터로 받아서
// time = 0인 인스트럭션을 RS에서 제거하고,
// 그 인스트럭션을 ROB에서 C 상태로 바꾼다 = 동일 의미 : RS의 인스트럭션한테 값을 전달해주고 그 operand를 Q에서 V로 바꾼다
void excute_retire(struct RS* rs, struct ROB* rob, bool* is_completed_this_cycle)
{
	if ( (--((*rs).time_left) ) < 0)//타이머를 1줄이고, 만약 0 이하라면 (ex 완료) ROB의 state를 C
	{//ex completed in this step
		rob[(*rs).index_rob].status = C;
		is_completed_this_cycle[(*rs).index_rob] = true;
		(*rs).is_valid = false;
	}
}

// ROB랑 RAT를 포인터로 받아서
// ROB에서 C이고, 자기 앞의 인스트럭션이 모두 C인 인스트럭션들을
// 없애고 RAT에서 RF_VALID를 T로 바꾸어 준다
// delta_n_rob -= 제거한 인스트럭션 갯수;
void commit(struct RAT* rat, struct ROB* rob, struct status_cyc_arr *stat_rob)
{
	int i;
	int num_inst_to_retire = 0;
	for (i = 0; i < (*stat_rob).occupied; i++)
	{
		if ( rob[i].status == C )
		{
			num_inst_to_retire++;
		}
		else
		{
			break;
		}	
	}

	for (i = 0; i < num_inst_to_retire; i++)
	{
		rat[rob[i].dest].RF_valid = true;
		(*stat_rob).head = (((*stat_rob).head + 1) % (*stat_rob).size);	
	}	
};

void simul_ooo(struct Config* config)
{
	//
	// Init simulator
	//

	//Init report data
	struct Status {
		int cycle;//num of cycle
		struct
		{
			int total;//totalnum
			int intalu;//alu num
			int memread;//read mem
			int memwrite;//write mem
		} inst;//num of instructions
	} status = { 0,{0,0,0,0} };

	// Init OoO
	enum Debug { None = 0, ROB = 1, RSROB = 2 } debug = (*config).Dump; //set Debug_mode from config

	int N = (*config).Width;//set fetch = decode = issue width

	struct FQ* fetch_queue = malloc(sizeof(struct FQ) * 2 * N);//set fetch_queue
	struct Cycle_index index_fq = {0,0,0,2*N,0};//fetch_queue의 인덱스

	struct ROB* rob = malloc(sizeof(struct ROB) * ((*config).ROB_size));
	struct Cycle_index index_rob = { 0,0,0,(*config).ROB_size,0 };//rob의 인덱스
	bool* is_completed_this_cycle = malloc(sizeof(bool) * (*config).ROB_size);//이번 사이클 업데이트

	struct RS*	rs = malloc( sizeof(struct RS) * ((*config).Res_size) );
	int size_rs = (*config).Res_size;

	struct RAT	rat[REGISTER_SIZE];// Architectural Register File

	int Instruction_length;
	struct INST** inst_arr = 0;
	Instruction_length = make_inst_array(TRACE_FILENAME, inst_arr);
	
	char output_title[50];
	sprintf(output_title, "%d_%d_%d_%d_output.out", config->Dump, config->Width, config->ROB_size, config->Res_size);
	FILE* report = fopen(output_title, "w");
	//
	// Starting Simulation
	//

	//declear some value
	int fetch_num;
	int decode_num;
	int issue_num;
	bool fetch_eof = false;
	int pc = 0;
	
	

	do
	{//do one cycle
		++(status.cycle);

		//# of elements update
		update_blank(&index_fq);//update len_of_fq
		update_blank(&index_rob);//update len_of_rob

		//fetch num and decode num and essue init
		fetch_eof = false;
		fetch_num = 0;
		decode_num = 0;
		issue_num = 0;
		for (int idx = 0; idx < index_rob.size; ++idx)
		{
			is_completed_this_cycle[idx] = false;
		}

		//in ROS
		commit(rat, rob, &index_rob);

		//in every RS
		for (int idx = 0; idx < size_rs; ++idx )
		{
			
			if (rs[idx].is_valid == false)
			{//만약 빈 공간이라면,
				if (decode_num < N && index_rob.blank != 0 && 
					(pc!= Instruction_length || index_fq.size !=getblank(&index_fq)) )
				{// rob가 다차지 않았다면 , N개를 decode하지 않았다면 
				 // fetch가 다 떨어지지 않았다면 디코드 최대치에 다다를때까지 디코드한다.
					++decode_num;
					decode(fetch_queue, &index_fq, rs + idx, rat, rob, &index_rob);


					//디코드 시 들어간 펑션 종류를 체크한다.
					switch (fetch_queue[index_fq.head].op)
					{
					case IntAlu: ++(status.inst.intalu); break;
					case MemRead: ++(status.inst.memread); break;
					case MemWrite: ++(status.inst.memwrite); break;
					}
					++(status.inst.total);
				}
			}
			else if (issue_num < N && (rs[idx].time_left == -1))
			{//타이머 설정이 안됬다면 (P상태 Instruction이라면) 값이 준비되었는지 확인하고 issue한다.
				++issue_num;
				issue(rs + idx, rob, is_completed_this_cycle);
			}
			else //timer seted mean it alredy issued
			{
				excute_retire(rs + idx, rob, is_completed_this_cycle);
			}

		}

		//in fetch queue
		fetch(&pc, N, Instruction_length, inst_arr, fetch_queue, &index_fq);
		

		//for print report
		if (debug != None)
		{
			fprintf(report,"= Cycle %d\n", status.cycle);
			printf("= Cycle %d\n", status.cycle);
			if (debug == RSROB)
			{
				for (int idx = 0; idx < size_rs; ++idx)
				{
					if (rs[idx].is_valid)
					{
						fprintf(report,"RS%d\t:ROB%d\t", idx + 1, rs[idx].index_rob);
						if (rs[idx].oprd_1.state == V)
							fprintf(report,"V\t");
						else
							fprintf(report,"%d\t", rs[idx].oprd_1.data.q );

						if (rs[idx].oprd_2.state == V)
							fprintf(report,"V\t");
						else
							fprintf(report,"%d\n", rs[idx].oprd_2.data.q);
					}
					else
					{
						fprintf(report,"RS%d\t:EMPTY\n", idx + 1);
					}
				}
			}
			for (int idx = 0; idx < index_rob.size; ++idx)
			{
				if (idx >= index_rob.size - index_rob.blank)
				{
					fprintf(report,"ROB%d\t:P\n", idx + 1);
				}
				else
				{
					fprintf(report,"RS%d\t:%c\n", idx + 1, (rob[idx].status==P)?'P':'C');
				}
			}
		}


	} while ( (index_fq.blank != index_fq.size) || (index_rob.blank != index_rob.size) );

	//print final stat
	fprintf(report, "Cycles\t%d\n", status.cycle);
	fprintf(report, "IPC\t%.2f\n", ((float)status.inst.total)/status.cycle);
	fprintf(report, "Total Insts\t%d\n", status.inst.total);
	fprintf(report, "IntAlu\t%d\n", status.inst.intalu);
	fprintf(report, "MemRead\t%d\n", status.inst.memread);
	fprintf(report, "MemWrite\t%d\n", status.inst.memwrite);

	fclose(report);


	//free used data storage


	free(fetch_queue);
	free(rob);
	free(is_completed_this_cycle);
	free(rs);
	free(inst_arr);
}

