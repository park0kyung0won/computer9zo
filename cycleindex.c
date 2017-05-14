
struct Cycle_index
{
	int head;//시작점 (맨 처음 원소)
	int tail;// 끝점 (맨 끝 원소 + 1)
	int len;// 원소 갯수
	int size;// 어레이 크기
	int is_reversed;
};

inline void updatelen(struct Cycle_index* cidx)
{
	if ((*cidx).is_reversed)
		(*cidx).len = (*cidx).size - ((*cidx).head - (*cidx).tail);
	else
		(*cidx).len = (*cidx).tail - (*cidx).head;
}

inline void foward_head