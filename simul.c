#include <stdio.h>
#include <stdlib.h>
#include "components.h"
#include "cycleindex.c"

#define REGISTER_SIZE 16

struct Config
{
	int debug;
	int width;
	int rob_size;
	int rs_size;
};


void fetch(int fetch_width, FILE* inst_mem, struct FQ* fetch_queue, struct Cycle_index* idx);
/*{
	for (int num_fetch = 0;
		num_fetch < fetch_width && num_fetch < ((*idx).length - (*idx).n_elem);//빈공간을 다 채우거나 N만큼 패치했다면 정지
		++num_fetch)
	{
		//fetch_queue[(*idx).end]에 instruction 채우기
		if ( ~ (read_instruction(inst_mem, (fetch_queue + (*idx).end) ) ) )
			break;//if is eof, fetch stop;

		(*idx).end = ((*idx).end + 1) % (*idx).length;//사이클릭 어레이
	}
}*/
//빈공간이 있다면,
//동주가 만든 read_inst 함수를 이용해서 받은 instruction을
//FQ형태로 변환해서 저장.



// fetch queue랑 RS, ROB, RT를 포인터로 받아서
// n_fq와n_rs, n_rob를 고려하여
// 가능한 갯수 만큼 인스트럭션을 fetch_queue에서 가져와
// RS와 ROB에 넣어준다
// (RS에 넣을 때, operand 둘 중 하나라도 Q면 time을 -1로 지정해 놓는다)
// delta_n_fq -= 이동한 인스트럭션 갯수; 를 해준다
// delta_n_rs += 이동한 인스트럭션 갯수; 를 해준다
// delta_n_rob += 이동한 인스트럭션 갯수; 를 해준다
// 그리고 output에 해당하는 RT의 RF_VALID를 수정해준다
void decode(struct FQ* fetch_queue,struct RS* rs, struct ROB* rob);

// RS를 포인터로 받아서
// time이 -1인 것들에 대해
// operand 모두가 V 상태이라면
// time을 -1에서 '수행에 걸리는 시간' 값으로 바꿔준다. ( 실행 시작 )
void issue( struct RS* rs, struct ROB* rob, bool* is_completed_this_cycle );

// RS를 포인터로 받아서
// execution을 한다 (operand가 전부 V 상태이면 time 하나 뺌)
void execution();


// RS랑 ROB를 포인터로 받아서
// time = 0인 인스트럭션을 RS에서 제거하고,
// 그 인스트럭션을 ROB에서 C 상태로 바꾼다
// 그리고 그 인스트럭션이 속한 ROB의 자리 (ex ROB3)
// 를 operand의 Q로 가지고 있는 RS의 인스트럭션한테
// 값을 전달해주고 그 operand를 Q에서 V로 바꾼다
// delta_n_rs -= 제거한 인스트럭션 갯수;
void rs_retire()
{
	if ((--rs[idx].time_left) < 0)//타이머를 1줄이고, 만약 0 이하라면 (ex 완료) ROB의 state를 C
	{//ex completed in this step
		rob[rs[idx].index_rob].status = 'C';
	}
}

// ROB랑 RAT를 포인터로 받아서
// ROB에서 C이고, 자기 앞의 인스트럭션이 모두 C인 인스트럭션들을
// 없애고 RAT에서 RF_VALID를 T로 바꾸어 준다
// delta_n_rob -= 제거한 인스트럭션 갯수;
void commit(struct RAT* rat , struct ROB* rob, struct Cycle_index* index_rob);

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
	enum Debug { None = 0, ROB = 1, RSROB = 2 } debug = (*config).debug; //set Debug_mode from config
	
	int N = (*config).width;//set fetch = decode = issue width

	struct FQ* fetch_queue = malloc(sizeof(struct FQ) * 2 * N);//set fetch_queue
	struct Cycle_index index_fq = {0,0,0,2*N};//fetch_queue의 인덱스

	struct ROB* rob = malloc(sizeof(struct ROB) * ((*config).rob_size));
	struct Cycle_index index_rob = { 0,0,0,(*config).rob_size};//rob의 인덱스
	bool is_completed_this_cycle = malloc(sizeof(bool) * (*config).rob_size);//이번 사이클 업데이트

	struct RS*	rs = malloc( sizeof(struct RS) * ((*config).rs_size) );
	struct Cycle_index index_rs = { 0,0,0,((*config).rs_size) };//rs의 인덱스

	int n_rs; // Reservation Station에 인스트럭션이 몇 개나 들어있는지 알려준다
	int delta_n_rs; // n_rs의 변화를 의미한다.  파이프라인 구현을 위함.

	struct RAT	rat[REGISTER_SIZE];// Architectural Register File


	//
	// Starting Simulation
	//

	int fetch_num;
	int decode_num;
	int issue_num;

	do
	{//do one cycle

		//# of elements update
		updatelen(&index_fq);//update len_of_fq
		updatelen(&index_rob);//update len_of_rob

		//fetch num and decode num and essue init
		fetch_num = 0;
		decode_num = 0;
		issue_num = 0;

		//in ROS
		commit(rat, rob, &index_rob);

		//in every RS
		for (int idx = 0; idx < index_rs.size; ++idx )
		{
			
			if (decode_num < N && (rs[idx].is_valid == false) )
			{//만약 빈 공간이라면, 디코드 최대치에 다다를때까지 디코드한다.
				++decode_num;
				decode(fetch_queue, rs, rob);
			}
			else if (issue_num < N && (rs[idx].time_left == -1))
			{//타이머 설정이 안됬다면 (P상태 Instruction이라면) 값이 준비되었는지 확인하고 issue한다.
				issue(rs, rob, is_completed_this_cycle);
			}
			else //timer seted mean it alredy issued
			{
				rs_retire()

			}

		}
		//in fetch queue
		for fetch_num_in_this_cycle < min ( N , fetch_que_size - fetch_queue_elem_num)
		{
			fetch(end_of_fetch_queue);
		}


	}
	while( ~is_fetch_eof && (rob_elem_num > 0) )

	free(fetch_queue)
	free(rob.data)
		free(rs
}

