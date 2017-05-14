#ifndef CYCLE
#define CYCLE

struct Cycle_index
{
	int head;//시작점 (맨 처음 원소)
	int tail;// 끝점 (맨 끝 원소 + 1)
	int blank;// 빈공간 개수
	int size;// 어레이 크기
	int is_reversed;
};

int getblank(struct Cycle_index* cidx);
void update_blank(struct Cycle_index* cidx);
void move_cidx_head(struct Cycle_index* cidx, int move_num);
void move_cidx_tail(struct Cycle_index* cidx, int move_num);

#endif // !CYCLE
