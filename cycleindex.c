
struct Cycle_index
{
	int head;//시작점 (맨 처음 원소)
	int tail;// 끝점 (맨 끝 원소 + 1)
	int blank;// 빈공간 개수
	int size;// 어레이 크기
	int is_reversed;
};

inline void update_blank(struct Cycle_index* cidx)
{
	if ((*cidx).is_reversed == 0)
		(*cidx).blank = (*cidx).size - ((*cidx).tail - (*cidx).head); 
	else
		(*cidx).blank = (*cidx).head - (*cidx).tail;
}

inline void move_cidx_head(struct Cycle_index* cidx, int move_num)
{
	if (((*cidx).head = (*cidx).head + move_num) >= (*cidx).size)
	{
		(*cidx).head %= (*cidx).size;
		(*cidx).is_reversed = 0;
	}
}
inline void move_cidx_tail(struct Cycle_index* cidx, int move_num)
{
	if (((*cidx).tail = (*cidx).tail + move_num) >= (*cidx).size)
	{
		(*cidx).tail %= (*cidx).tail;
		(*cidx).is_reversed = 1;
	}
}