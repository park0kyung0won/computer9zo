#ifndef _CYCLE_INDEX_
#define _CYCLE_INDEX_

#include <stdbool.h>

struct Cycle_index;
int ci_at(struct Cycle_index* cindex, int ith);//get ith element in cycle_index

struct Cycle_index
{
	int head;//시작점 (맨 처음 원소)
	int size;// 어레이 크기
};

//void ci_move_head(struct Cycle_manage* cidx, int move_num)
//void ci_move_tail(struct Cycle_manage* cidx, int move_num)


#endif // !_CYCLE_INDEX_
