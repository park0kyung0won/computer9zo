#include <stdlib.h>
#include "components.h"


#define REGISTER_SIZE 16

struct Config
{
	int debug;
	int width;
	int rob_size;
	int rs_size;
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
	enum Debug { None = 0, ROB = 1, RSROB = 2 } debug = (*config).debug; //set Debug_mode from config
	
	int N = (*config).width;//set fetch = decode = issue width

	struct FI* fetch_queue = malloc( sizeof(struct FI) * N )//set fetch_queue

	struct {
		int start = 0;
		int end = 0;
		int num_elem = 0;
		struct ROB data* = malloc( sizeof(struct ROB) * ((*config).rob_size) );
	} rob;
	
	struct RS*	rs = malloc( sizeof(struct RS) * ((*config).rs_size) );
	
	struct RAT	rat[REGISTER_SIZE];

	//
	// Starting Simulation
	//
	do
	{//do one cycle

		//# of elements update
		fetch number update;
		ros number updata;

		//in ROS
		commit(elem) // when end elem not C state
		//it just moving head pointer to end of C state

		//in every RS
		for each elem in RS
		{
			if elem is blank and decode_(in_this_cycle < N) ;
				decode;

			else if elem time is - 1;
				issue
				//if have q value, access Ros[q] and if Ros[q].state == c, set timer

			else //timer seted mean it alredy issued
				--(elem.time)//one step forward
				if elem.time<0//ex completed in this step
					Ros(elem.ROS_index).state = C

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

}

struct cycle_index{
	int start = 0;
	int end = 0;
	int num_elem = 0;
	bool is_reversed = false;
}

inline void cindex_num_calc(*cycle_index)
{
	int 
}