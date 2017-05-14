#include <stdio.h>
#include <stdlib.h>
#include "components.h"
#include "cycleindex.c"
#include "isnt.c"

#define REGISTER_SIZE 16
#define TRACE_FILENAME "trace.out"

void fetch(int *pc, int fetch_width, int inst_length, struct INSTRUCTION *inst, struct FQ* fetch_queue, struct Cycle_index* idx)
{
	int fetch_num = (fetch_width > (*idx).blank) ? (*idx).blank : fetch_width;
	int i;
	for (i = 0; i < fetch_num && (*pc) < inst_length; i++)
	{
		(fetch_queue[(*idx).tail]).op = (inst[*pc]).inst_type;
		(fetch_queue[(*idx).tail]).dest = (inst[*pc]).destination;
		(fetch_queue[(*idx).tail]).op = (inst[*pc]).src1;
		(fetch_queue[(*idx).tail]).op = (inst[*pc]).src2;
		++(*pc);
		move_cidx_tail(idx, 1);
	}

}


// fetch queue랑 RS, ROB, RT를 포인터로 받아서
// 인스트럭션을 fetch_queue에서 가져와
// RS와 ROB에 넣어준다
// (RS에 넣을 때, operand 둘 중 하나라도 Q면 time을 -1로 지정해 놓는다)
// 그리고 output에 해당하는 RT의 RF_VALID를 수정해준다
void decode(struct FQ* fq, struct Cycle_index* index_fq, struct RS* rs, struct RAT* rat, struct ROB* rob,struct Cycle_index* index_rob)
{
	//regist in rob
	rob[(*index_rob).tail].dest = fq[(*index_fq).head].dest;
	rob[(*index_rob).tail].op = fq[(*index_fq).head].op;
	rob[(*index_rob).tail].status = P;
		//rob[(*index_rob).tail].value = 0;
		
	//rat change
	rat[fq[(*index_fq).head].dest].RF_valid = false;
	rat[fq[(*index_fq).head].dest].Q = (*index_rob).tail;

	//regist in rs
	(*rs).index_rob = (*index_rob).tail;
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
	move_cidx_tail(index_rob, 1);
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
void commit(struct RAT* rat, struct ROB* rob, struct Cycle_index* index_rob)
{
	int i;
	int num_inst_to_retire = 0;
	for (i = 0; (((*index_rob).head + i) % (*index_rob).size) < (*index_rob).tail; i++)
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
		move_cidx_tail(index_rob, 1);	
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
	};

	// Init OoO
	enum Debug { None = 0, ROB = 1, RSROB = 2 } debug = (*config).Dump; //set Debug_mode from config
	
	int N = (*config).Width;//set fetch = decode = issue width

	struct FQ* fetch_queue = malloc(sizeof(struct FQ) * 2 * N);//set fetch_queue
	struct Cycle_index index_fq = {0,0,0,2*N};//fetch_queue의 인덱스

	struct ROB* rob = malloc(sizeof(struct ROB) * ((*config).ROB_size));
	struct Cycle_index index_rob = { 0,0,0,(*config).ROB_size };//rob의 인덱스
	bool* is_completed_this_cycle = malloc(sizeof(bool) * (*config).ROB_size);//이번 사이클 업데이트

	struct RS*	rs = malloc( sizeof(struct RS) * ((*config).Res_size) );
	int size_rs = (*config).Res_size;

	struct RAT	rat[REGISTER_SIZE];// Architectural Register File

	int Instruction_length;
	struct Instruction* inst_arr;
	Instruction_length = make_inst_array(TRACE_FILENAME, inst_arr);
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


	} while ( (index_fq.blank != index_fq.size) || (index_rob.blank != index_rob.size) );

	//free used data storage
	free(fetch_queue);
	free(rob);
	free(is_completed_this_cycle);
	free(rs);
}

