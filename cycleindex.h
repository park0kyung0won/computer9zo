#ifndef _CYCLE_INDEX_
#define _CYCLE_INDEX_

#include <stdbool.h>

struct status_cyc_arr;
int ci_at(struct status_cyc_arr* cindex, int ith);//get ith element in cycle_index

struct status_cyc_arr
{
	int head;     // 시작점 (맨 처음 원소)
	int occupied; // 지금 들어있는 원소의 숫자
	int size;     // 어레이 크기
};

//void ci_move_head(struct Cycle_manage* cidx, int move_num)
//void ci_move_tail(struct Cycle_manage* cidx, int move_num)


#endif // !_CYCLE_INDEX_
