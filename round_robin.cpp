#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <iomanip>
#include <functional>
#include <typeinfo>
#include <round_robin.h>

using namespace std; 

#define t_cs 8


Process::Process() {
	wait_t = 0;
	preempts = 0;
	block_t = 0;
	status = 0;
	/* status:
	0: in the table
	1: in the ready queue
	2: in CPU
	3: in I/0
	4: in switch in
	5: in switch out
	6: terminated
	*/
}

void Process::setup(string name, int arrival, int burst, int burst_cnt, int io) {
	id = name;
	arr_t = arrival;
	bur_t = burst;
	num_bur = burst_cnt;
	io_t = io;
	burst_left = burst;
}

void get_queue(vector<Process> ready_queue, string& queue_out) {

	queue_out = "[Q ";
	if( ready_queue.size()>0 ){
		for (int i = 0; i < ready_queue.size(); ++i) {
			queue_out += ready_queue[i].id;
			if (i != ready_queue.size()-1) {
				queue_out += " ";
			}
		}
	}
	else{
		queue_out += "<empty>";
	}
	queue_out += "]\n";
}

int RR(vector<string> id, vector<int> arr_t, vector<int> bur_t, vector<int> num_bur, vector<int> io_t, string rr_add, ofstream& outfile) {
	vector<Process> table;
	vector<Process> ready_queue;
	vector<Process> running;
	vector<Process> blocked;
	vector<Process> table_calculation;

	int queue_mode = 0; // default add at the end of queue 0:END 1:BEGINNING
	int t_slice = 80;
	int total_num_burst = 0;
	float total_burst_time = 0.0;
	float total_wait_time = 0.0;
	float total_turnaround_time = 0.0;
	int context_switch_cnt = 0;
	int preempt_cnt = 0;
	string queue_out = "default string";
	// status indicator
	int switch_in_timer = 0;
	int switch_out_timer = 0;

	// determine queue mode
	if (rr_add == "BEGINNING") queue_mode = 1;
	else queue_mode = 0;

	for (int i = 0; i < id.size(); i++) {
		Process p;
		p.setup(id[i], arr_t[i], bur_t[i], num_bur[i], io_t[i]);
		table.push_back(p);
		table_calculation.push_back(p);
	}

	// first process
	table[0].status = 1; // to ready queue

	if (queue_mode==1) ready_queue.insert(ready_queue.begin(),table[0]);
	else ready_queue.push_back(table[0]);

	get_queue(ready_queue, queue_out);

	int t = ready_queue[0].arr_t;
	cout << "\ntime 0ms: Simulator started for RR [Q <empty>]\n";
	cout << "time " + to_string(t) + "ms: Process " +
			ready_queue[0].id + " arrived and added to ready queue " + queue_out;
	table.erase(table.begin());
	ready_queue[0].wait_t = t;

	int slice_counter = 0;
	while ( !(table.empty() && ready_queue.empty() && running.empty() && blocked.empty()) ) {

//--------------------------------------------------------------------

		if ( !table.empty() && running.empty() ) {
			int num_arr = 0;
			// check table to see if there is incoming process at this time t
			for (int i = 0; i < table.size(); i++) {
				// there is a process arriving
				if (t == table[i].arr_t) {
					num_arr++;
					table[i].wait_t = t;
					table[i].status = 1; // in ready queue
					if (queue_mode==1) ready_queue.insert(ready_queue.begin(),table[i]);
					else ready_queue.push_back(table[i]);
					get_queue(ready_queue, queue_out);
					cout << string("time ") + to_string(t)
                			+"ms: Process " + table[i].id
                			+" arrived and added to ready queue " + queue_out;
				}
			}
			for (int i = 0; i < num_arr; i++) {
				table.erase(table.begin());
			}
		} // end if ( !table.empty() )

//--------------------------------------------------------------------

		if ( !running.empty() ) {
			// slice time expeired
			if ( slice_counter == t_slice) { 
				slice_counter = 0; //reset
				if (ready_queue.empty() && running[0].bur_t != t_slice) { // continue run this process
					cout << "time " + to_string(t) +
							"ms: Time slice expired; no preemption because ready queue is empty [Q <empty>]\n";
					running[0].status = 2; // using cpu
				}
				else { // put back to ready queue
					if ( running[0].burst_left  != 0 ){
						get_queue(ready_queue, queue_out);
		         		cout << "time "+ to_string(t)
		                		+"ms: Time slice expired; process "+ running[0].id
		                		+" preempted with "+ to_string(running[0].burst_left)
		                		+"ms to go " + queue_out;
		                running[0].status = 5; // switching out;
		                preempt_cnt++;
					}
					else { // process finished burst at the time expired point
						if (running[0].num_bur == 0) { // this process terminated
							get_queue(ready_queue, queue_out);
							cout << "time "+to_string(t)
                    				+"ms: Process "+running[0].id+" terminated "+queue_out;
                    		running[0].status = 5; // go to switch out first
						}
						else if (running[0].num_bur > 0) { // still more bursts to go
							get_queue(ready_queue, queue_out);
							if (running[0].num_bur == 1) {
								cout << "time "+to_string(t)
	                      				+"ms: Process "+running[0].id
	                      				+" completed a CPU burst; "+to_string(running[0].num_bur)
	                      				+" burst to go "+queue_out;
							}
							else {
								cout << "time "+to_string(t)
	                      				+"ms: Process "+running[0].id
	                      				+" completed a CPU burst; "+to_string(running[0].num_bur)
	                      				+" bursts to go "+queue_out;
							}
                      		running[0].block_t = running[0].io_t + (t_cs/2) + t ;
                      		cout << "time "+ to_string(t)
                    				+"ms: Process "+running[0].id
                    				+" switching out of CPU; will block on I/O until time "+to_string(running[0].block_t)
                    				+"ms "+queue_out;

                    		running[0].status = 5; // switch out
						}
						else {
							cerr << "Problem: number of bursts < 0!!!";
							return EXIT_FAILURE;
						}
					}
				}
			} // end if (t == slice_point)
		}

//--------------------------------------------------------------------

		if ( !blocked.empty() && running.empty()) {
			vector<Process> finished;
			// check if blocked process completed
			for (int i = 0; i < blocked.size(); i++) {
				if (t == blocked[i].block_t) { // block ends
					finished.push_back(blocked[i]);
					blocked[i].wait_t = t;
					blocked[i].status = 1; // in ready queue
					if (queue_mode==1) ready_queue.insert(ready_queue.begin(),blocked[i]);
					else ready_queue.push_back(blocked[i]);
					get_queue(ready_queue, queue_out);
					cout << "time "+to_string(t)
                  			+"ms: Process "+blocked[i].id
                  			+" completed I/O; added to ready queue "+queue_out;
				}
			}
			// take out from blocked
			for (int i = 0; i < finished.size(); i++) {
				for (int j = 0; j < blocked.size(); j++) {
					if (finished[i].id == blocked[j].id) {
						blocked.erase(blocked.begin()+j);
						break;
					}
				}
			}
		} // end if ( !blocked.empty() )

//--------------------------------------------------------------------

		// add process to use cpu
		if ( running.empty() && !ready_queue.empty() ) {
			total_wait_time += (t - ready_queue[0].wait_t);
			ready_queue[0].status = 4; // go to switch in
			running.push_back(ready_queue[0]);
			ready_queue.erase(ready_queue.begin());
			context_switch_cnt++;
		} // end if ( running.empty() && !ready_queue.empty() )


//--------------------------------------------------------------------

		// Process running in CPU
		if ( !running.empty() ) {

			/* status:
			0: in the table
			1: in the ready queue
			2: in CPU
			3: in I/0
			4: in switch in
			5: in switch out
			6: terminated
			*/

			if (running[0].status == 2) { // using CPU
				// need to check whether it finishes in the time slice or not
				if (running[0].burst_left == 0) { // finish in the time slice
					
					if ( running[0].num_bur == 0 ) { // this process is done
						get_queue(ready_queue, queue_out);
            			cout << "time "+to_string(t)
                    			+"ms: Process "+running[0].id+" terminated "+queue_out;
            			running[0].status = 5; // go to switch out first
            			slice_counter = 0; // reset
					}
					else if (running[0].num_bur > 0) {
						get_queue(ready_queue, queue_out);
						if (running[0].num_bur == 1) {
							cout << "time "+to_string(t)
                      				+"ms: Process "+running[0].id
                      				+" completed a CPU burst; "+to_string(running[0].num_bur)
                      				+" burst to go "+queue_out;
						}
						else {
							cout << "time "+to_string(t)
                      				+"ms: Process "+running[0].id
                      				+" completed a CPU burst; "+to_string(running[0].num_bur)
                      				+" bursts to go "+queue_out;
						}
                      	running[0].block_t = running[0].io_t + (t_cs/2) + t;
                      	cout << "time "+ to_string(t)
                    			+"ms: Process "+running[0].id
                    			+" switching out of CPU; will block on I/O until time "+to_string(running[0].block_t)
                    			+"ms "+queue_out;
                    	slice_counter = 0; // reset
                    	running[0].status = 5; // switching out
					}
					else {
						cerr << "Problem: number of bursts is negative!!\n";
						return EXIT_FAILURE;
					}
				}
				else if (running[0].burst_left > 0) {

					slice_counter++;
					running[0].burst_left--;
				}
			}

			if (running[0].status == 5) { // in switch out
				if (switch_out_timer == t_cs/2) { // switching out completed
					switch_out_timer = 0; // reset

					if (running[0].burst_left == 0 && running[0].num_bur == 0) { //terminated
						running[0].status = 6;
						total_turnaround_time += t - running[0].arr_t;
						running.erase(running.begin());
					}
					else if (running[0].burst_left>0) { // still more time to go
						running[0].status = 1; // in reay queue
						running[0].wait_t = t;
						if (queue_mode==1) ready_queue.insert(ready_queue.begin(),running[0]);
						else ready_queue.push_back(running[0]);
						running.erase(running.begin());
					}
					else { // reday to do I/O
						running[0].status = 3; // in I/O
						running[0].burst_left = running[0].bur_t;
						blocked.push_back(running[0]);
						running.erase(running.begin());
					}

					// add process to use cpu
					if ( running.empty() && !ready_queue.empty() ) { // immediately follow a switch in
						total_wait_time += (t - ready_queue[0].wait_t);
						ready_queue[0].status = 4; // go to switch in
						running.push_back(ready_queue[0]);
						ready_queue.erase(ready_queue.begin());
						context_switch_cnt++; 
					} // end if ( running.empty() && !ready_queue.empty() )
				}
				else {
					switch_out_timer++;
				}
			}

			if (running[0].status == 4) { // in switch in  *optional argument adds here*

				// cout << "t: " << t << " --> in switching *in*" << switch_in_timer << endl;
				if (switch_in_timer == t_cs/2) { // switching in completed
					switch_in_timer = 0; // reset
					//reday to use cpu
					running[0].status = 2; // start using cpu
					// running at the beginning of the burst
					if (running[0].bur_t == running[0].burst_left) {
						running[0].num_bur -= 1;
						get_queue(ready_queue, queue_out);
						cout << "time "+to_string(t)
		              			+"ms: Process "+running[0].id
		              			+" started using the CPU "+queue_out;
					}
					else {
						get_queue(ready_queue, queue_out);
						cout << "time "+to_string(t)
                  				+"ms: Process "+running[0].id
                  				+" started using the CPU with "+ to_string(running[0].burst_left)
                  				+"ms remaining "+queue_out;
					}
					slice_counter++;
					running[0].burst_left--;
				}
				else {
					switch_in_timer++;
				}
			}
		} // end if ( !running.empty() )

//--------------------------------------------------------------------
		// check second time
		if ( !blocked.empty() ) {
			vector<Process> finished;
			// check if blocked process completed
			for (int i = 0; i < blocked.size(); i++) {
				if (t == blocked[i].block_t) { // block ends
					finished.push_back(blocked[i]);
					blocked[i].wait_t = t;
					blocked[i].status = 1; // in ready queue
					if (queue_mode==1) ready_queue.insert(ready_queue.begin(),blocked[i]);
					else ready_queue.push_back(blocked[i]);
					get_queue(ready_queue, queue_out);
					cout << "time "+to_string(t)
                  			+"ms: Process "+blocked[i].id
                  			+" completed I/O; added to ready queue "+queue_out;
				}
			}
			// take out from blocked
			for (int i = 0; i < finished.size(); i++) {
				for (int j = 0; j < blocked.size(); j++) {
					if (finished[i].id == blocked[j].id) {
						blocked.erase(blocked.begin()+j);
						break;
					}
				}
			}
		} // end if ( !blocked.empty() )

//--------------------------------------------------------------------
		// check second time
		if ( !table.empty() ) {
			int num_arr = 0;
			// check table to see if there is incoming process at this time t
			for (int i = 0; i < table.size(); i++) {
				// there is a process arriving
				if (t == table[i].arr_t) {
					num_arr++;
					table[i].wait_t = t;
					table[i].status = 1; // in ready queue
					if (queue_mode==1) ready_queue.insert(ready_queue.begin(),table[i]);
					else ready_queue.push_back(table[i]);
					get_queue(ready_queue, queue_out);
					cout << string("time ") + to_string(t)
                			+"ms: Process " + table[i].id
                			+" arrived and added to ready queue " + queue_out;
				}
			}
			for (int i = 0; i < num_arr; i++) {
				table.erase(table.begin());
			}
		} // end if ( !table.empty() )

//--------------------------------------------------------------------

		if (table.empty() && ready_queue.empty() && running.empty() && blocked.empty()) {
			// finished exitting loop
			break;
		}

		t++; // time step
	} // end while loop

	cout << "time " + to_string(t) + "ms: Simulator ended for RR" << endl;

	for (int i = 0; i < table_calculation.size(); i++) {
		total_burst_time += table_calculation[i].bur_t * table_calculation[i].num_bur;
		total_num_burst += table_calculation[i].num_bur;
		total_turnaround_time -= ( table_calculation[i].io_t * (table_calculation[i].num_bur-1) );
	}

	float avg_burst_time = total_burst_time/total_num_burst;
	float avg_wait_time = total_wait_time/total_num_burst;
	float avg_turnaround_time = total_turnaround_time/total_num_burst;

	outfile << "Algorithm RR\n";
	outfile << "-- average CPU burst time: ";
	outfile << fixed << setprecision(2) << avg_burst_time; 
	outfile << " ms\n";
	outfile << "-- average wait time: ";
	outfile << fixed << setprecision(2) << avg_wait_time;
	outfile << " ms\n";
	outfile << "-- average turnaround time: ";
	outfile << fixed << setprecision(2) << avg_turnaround_time;
	outfile << " ms\n";
	outfile << "-- total number of context switches: " + to_string(context_switch_cnt) << endl;
	outfile << "-- total number of preemptions: " + to_string(preempt_cnt) << endl;

	return EXIT_SUCCESS;
}