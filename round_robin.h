#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H

class Process {
public:

	Process();
	void setup(string name, int arrival, int burst, int burst_cnt, int io);

	string id;
	int arr_t,bur_t,num_bur,io_t;
	int burst_left, wait_t;
	int preempts, block_t;
	int status;
};

void get_queue(vector<Process> ready_queue, string& queue_out);

int RR(vector<string> id, vector<int> arr_t, vector<int> bur_t, vector<int> num_bur, vector<int> io_t, string rr_add, ofstream& outfile);

#endif
