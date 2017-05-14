
#include "cycleindex.h"


int getblank(struct Cycle_index* cidx)
{
	if ((*cidx).is_reversed == 0)
		return (*cidx).size - ((*cidx).tail - (*cidx).head);
	else
		return (*cidx).head - (*cidx).tail;
}


void update_blank(struct Cycle_index* cidx)
{
	(*cidx).blank = getblank(cidx);
}

void move_cidx_head(struct Cycle_index* cidx, int move_num)
{
	if (((*cidx).head = (*cidx).head + move_num) >= (*cidx).size)
	{
		(*cidx).head %= (*cidx).size;
		(*cidx).is_reversed = 0;
	}
}
void move_cidx_tail(struct Cycle_index* cidx, int move_num)
{
	if (((*cidx).tail = (*cidx).tail + move_num) >= (*cidx).size)
	{
		(*cidx).tail %= (*cidx).tail;
		(*cidx).is_reversed = 1;
	}
}