#include "cycleindex.h"

int ci_at(struct status_cyc_arr* cindex, int ith)
{//get ith element in cycle_index
	return ((cindex->head) + ith) % (cindex->size);
}












//int getblank(struct status_cyc_arr* cidx)
//{
//	if ((*cidx).is_reversed == 0)
//		return (*cidx).size - ((*cidx).tail - (*cidx).head);
//	else
//		return (*cidx).head - (*cidx).tail;
//}
//
//
//void update_blank(struct status_cyc_arr* cidx)
//{
//	(*cidx).blank = getblank(cidx);
//}
//
//void move_cidx_head(struct status_cyc_arr* cidx, int move_num)
//{
//	if (((*cidx).head = (*cidx).head + move_num) >= (*cidx).size)
//	{
//		(*cidx).head %= (*cidx).size;
//		(*cidx).is_reversed = 0;
//	}
//}
//void move_cidx_tail(struct status_cyc_arr* cidx, int move_num)
//{
//	if (((*cidx).tail = (*cidx).tail + move_num) >= (*cidx).size)
//	{
//		(*cidx).tail %= (*cidx).tail;
//		(*cidx).is_reversed = 1;
//	}
//}
