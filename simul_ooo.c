#include <stdio.h>
#include <stdbool.h>

// enum용 변수들
enum flag { Q = 0, V = 1 }; // has V? or Q?
enum instruction { IntAlu = 0, MemRead = 1, MemWrite = 2 }; // Instruction Type
enum is_complete { P = 0, C = 1 }; // P means pending, C means completed

// 동주쿤이 보내는 어레이랑 fetch_queue를 포인터로 받아서
// n_fq를 고려하여 최대 N개의 인스트럭션을 어레이에서 가져와
// fetch_queue 에다 넣는다
// delta_n_fq += 이동한 인스트럭션 갯수; 를 해준다
void fetch();

// fetch queue랑 RS, ROB, RT를 포인터로 받아서
// n_fq와n_rs, n_rob를 고려하여
// 가능한 갯수 만큼 인스트럭션을 fetch_queue에서 가져와
// RS와 ROB에 넣어준다
// (RS에 넣을 때, operand 둘 중 하나라도 Q면 time을 -1로 지정해 놓는다)
// delta_n_fq -= 이동한 인스트럭션 갯수; 를 해준다
// delta_n_rs += 이동한 인스트럭션 갯수; 를 해준다
// delta_n_rob += 이동한 인스트럭션 갯수; 를 해준다
// 그리고 output에 해당하는 RT의 RF_VALID를 수정해준다
void decode();

// RS를 포인터로 받아서
// time이 -1인 것들에 대해
// operand 모두가 V 상태이라면
// time을 -1에서 '수행에 걸리는 시간' 값으로 바꿔준다. ( 실행 시작 )
void issue();

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
void rs_retire();

// ROB랑 RAT를 포인터로 받아서
// ROB에서 C이고, 자기 앞의 인스트럭션이 모두 C인 인스트럭션들을
// 없애고 RAT에서 RF_VALID를 T로 바꾸어 준다
// delta_n_rob -= 제거한 인스트럭션 갯수;
void commit();

int N; // OoO 윈도우 사이즈를 의미함

int n_fq; // fetch queue에 인스트럭션이 몇 개나 들어있는지 알려준다
int delta_n_fq; // n_fq의 변화를 의미한다. 파이프라인 구현을 위함.

int n_rs; // Reservation Station에 인스트럭션이 몇 개나 들어있는지 알려준다
int delta_n_rs; // n_rs의 변화를 의미한다.  파이프라인 구현을 위함.

int n_rob; // ROB에 인스트럭션이 몇 개나 들어있는지 알려준다
int delta_n_rob; // n_rob의 변화를 의미한다 . 파이프라인 구현을 위함.

int simul_ooo(void)
{
	int reg[16]; // Architectural Register File

	int fetch_queue[N];

	struct
	{
		bool RF_valid;
		int q;
	} rat[16];
	
	struct
	{
		bool is_valid;
		enum instruction op;
		int time_left;
		struct
		{
			enum flag state;
			union
			{
				int v;
				int q;
			} data;
		} oprd_1;
		struct
		{
			enum flag state;
			union
			{
				int v;
				int q;
			} data;
		} oprd_2;
	} resv_station[N];

	int point = 0; // Points the first element in rob
	struct
	{
		enum instruction op;
		int value;
		int dest;
		enum is_complete status;
	} rob[N];

	// 메인 루프
	while(1)
	{
		n_fq += delta_n_fq;
		n_rs += delta_n_rs;
		n_rob += delta_n_rob;

		fetch();
		decode();
		issue();
		execution();
		rs_retire();
		commit();
	}


	return 0;
};

