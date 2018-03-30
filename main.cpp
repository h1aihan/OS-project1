#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <map>
#include <set>
#include <functional>
#include <typeinfo>
#include <stdlib.h>
#include <string>
#include <queue>
using namespace std; 

#define t_cs 8
int n;
/****************************************************************************************
****************************************************************************************
utilities section ***************************************************************************
****************************************************************************************
****************************************************************************************
**/
// Commparequeue is a class for sorting piorityqueue in SRT
class CompareQueue
{
public:
    bool operator()(pair<string,int> n1,pair<string, int> n2) {
        return n1.second>n2.second;
    }
};
priority_queue<pair<string,int>, vector<pair<string,int> >,CompareQueue>  q;
// file parser into vectors
void parse(string a, vector<string> &id, vector<int> &arr_t, vector<int> &bur_t, vector<int> &num_bur, vector<int> &io_t){
	int count = 0;
	size_t i = 0;
	string token;
	while((i = a.find("|")) != std::string::npos){
		token = a.substr(0, i);
		//cout << token << endl;
		a.erase(0, i + 1);
		count++;
		if(count == 1){
			id.push_back(token);
		}
		if(count == 2){
			arr_t.push_back(atoi(token.c_str()));
		}
		if(count == 3){
			bur_t.push_back(atoi(token.c_str()));
		}
		if(count == 4){
			num_bur.push_back(atoi(token.c_str()));
		}
	}
	//cout << a << endl;
	io_t.push_back(atoi(a.c_str()));
}
//file parser into map
void parse_m(string a, map<string, vector<int> > &m){
	vector<int> v;
	int count = 0;
	size_t i = 0;
	string token;
	string key;
	while((i = a.find("|")) != std::string::npos){
		token = a.substr(0, i);
		a.erase(0, i + 1);
		count++;
		if(count == 1){
			key = token;
		}
		else{
			v.push_back(atoi(token.c_str()));
		}
	}
	v.push_back(atoi(a.c_str()));
	m.insert(make_pair(key,v));
}
//priorityqueue printer
void print_pqueue(priority_queue<pair<string,int>,vector<pair<string,int> >,CompareQueue> q){
	if(!q.empty()){
		while (!q.empty())
		{
			cout << " " << q.top().first;
			q.pop();
		}
		cout << "]" << endl;			
	}
	else{
		cout << " <empty>]" << endl;;
	}
}
//vector queue printer
void printv(vector<string> q){
	if(!q.empty()){
		for (unsigned int j = 0; j < q.size(); ++j){
			cout << " " << q[j];
		}
		cout << "]" << endl;			
	}
	else{
		cout << " <empty>]" << endl;;
	}
}
/****************************************************************************************
****************************************************************************************
simulation section ***************************************************************************
****************************************************************************************
****************************************************************************************
**/
//fcfs
void fcfs(vector<string> id, vector<int> arr_t, vector<int> bur_t, vector<int> num_bur, vector<int> io_t, ofstream& outfile ){
	cout << "time 0ms: Simulator started for FCFS [Q <empty>]" <<  endl;
	int end = n;
	string current;
	vector<string> q;
	vector<int> s;
	vector<int> t;
	vector<int> io;
	vector<int> start_t;
	vector<int> num_bur_copy;
	vector<int> ave_turn;
	float wait = 0;
	int wait_t = 0;
	int count = -1;
	bool process=false;
	int flag = 0;
	int temp=0;

	for(unsigned int i=0; i<num_bur.size(); i++){
		num_bur_copy.push_back(num_bur[i]);
	}

	for (int i = 0; i<n; ++i){
		io.push_back(-1);
	}
	while(1){
		count++;	
		if(!process && !s.empty() && count == t[s[0]]){
			process = true;
			cout<< "time " << count << "ms: Process " << q[0] << " started using the CPU [Q";
			wait += count-start_t[s[0]]-t_cs/2;
			wait_t+=1;
			current = q[0];
			q.erase(q.begin());
			printv(q);
		} 
		if(process && count == bur_t[s[0]]+t[s[0]]){
			if(num_bur_copy[s[0]] > 1){
				num_bur_copy[s[0]] -= 1;
				if(num_bur_copy[s[0]] == 1){
					cout<< "time " << count << "ms: Process " << current << " completed a CPU burst; " << num_bur_copy[s[0]] <<  " burst to go [Q";
					printv(q);
				}
				else{
					cout<< "time " << count << "ms: Process " << current << " completed a CPU burst; " << num_bur_copy[s[0]] <<  " bursts to go [Q";
					printv(q);
				}
				t[s[0]] = count + io_t[s[0]]+t_cs/2;
				cout<< "time " << count << "ms: Process " << current << " switching out of CPU; will block on I/O until time " << t[s[0]] <<  "ms [Q";
				flag = 1;
				temp = count;
				ave_turn[s[0]] += count+t_cs/2;
				printv(q);
				io[s[0]] = t[s[0]];
				s.erase(s.begin());
				if(s.size() > 0){
					t[s[0]] = count + t_cs;
				}
				process = false;
			}
			else{
				cout<< "time " << count << "ms: Process " << current << " terminated [Q";
				printv(q);
				ave_turn[s[0]] += count+t_cs/2;
				s.erase(s.begin());
				if(s.size() > 0){
					t[s[0]] = count + t_cs;
				}
				process = false;
				end -= 1;
			}
		}
		for(unsigned int i =0; i<io.size(); ++i){
			if(count == io[i]){
				q.push_back(id[i]);
				s.push_back(i);
				cout << "time " << count << "ms: Process " << id[i] << " completed I/O; added to ready queue [Q";
				start_t[i] = count;
				ave_turn[i] -= count;
				printv(q);
				if(flag && count < temp+ 4){
					t[s[0]] += 4;
					flag=0;
				}
				if(q.size() == 1 && !process){
					t[s[0]] += t_cs/2;
				}
				io[i] = -1;
			}
		}
		for(unsigned int i = 0; i<arr_t.size(); ++i){
			if (arr_t[i] == count){
				q.push_back(id[i]);
				s.push_back(i);
				t.push_back(count+t_cs/2);
				start_t.push_back(count);
				ave_turn.push_back(count*(-1));
				cout<< "time " << count << "ms: Process " << id[i] << " arrived and added to ready queue [Q";
				for (unsigned int j = 0; j < q.size(); ++j){
					cout << " " << q[j];
				}
				cout << "]" << endl;
			}
		}
		if(end == 0){
			cout << "time "<< count+t_cs/2 <<"ms: Simulator ended for FCFS"<<endl;
			break;
		}
	}
	int switches = 0;
	for(unsigned int i=0; i< num_bur.size(); ++i){
		switches += num_bur[i];
	}

	float ave_b = 0.00;
	for(unsigned int i=0; i< bur_t.size(); ++i){
		ave_b += bur_t[i]*num_bur[i];
	}
	float ave_t = 0.00;
	for(unsigned int i=0; i< ave_turn.size(); ++i){
		ave_t += ave_turn[i];
	}
	ave_t = ave_t/switches;
	ave_b = ave_b/switches;
	wait = wait/wait_t;
	outfile << "Algorithm FCFS\n";
	outfile << "-- average CPU burst time: " << fixed << setprecision(2) << ave_b;
	outfile << " ms" << endl;
	outfile << "-- average wait time: " << fixed << setprecision(2) << wait;
	outfile << " ms" << endl;
	outfile << "-- average turnaround time: " << fixed << setprecision(2) << ave_t;
	outfile << " ms" << endl;
	outfile << "-- total number of context switches: " + to_string(switches) << endl;
	outfile << "-- total number of preemptions: 0\n";

}

void srt(vector<string> id, vector<int> arr_t, vector<int> bur_t, vector<int> num_bur, vector<int> io_t, ofstream& outfile){
	//simulation info output var
	int num_cs=0;
	int num_pr=0;
	int avg_wt=0;
	int turnaround=0;
	vector<int> starts_wait;
	vector<int> ave_turn;
	//start simulation using srt
	cout << "time 0ms: Simulator started for SRT [Q <empty>]" <<  endl;
	int end = n;
	pair<string,int> current;
	vector<pair<string,int> > wait;
	priority_queue<pair<string,int>, vector<pair<string,int> >,CompareQueue>  q;
	int s=-1;
	vector<int> t;
	vector<int> io;
	int count = -1;
	bool process=false;
	float ave_b = 0.00;
	for(unsigned int i=0; i< bur_t.size(); ++i){
		ave_b += bur_t[i]*num_bur[i];
	}

	for (int i = 0; i<n; ++i){
		io.push_back(-1);
	}
	for (int i = 0; i<n; ++i){
		t.push_back(0);
	}
	for (int i = 0; i<n; ++i){
		pair<string,int> ety;
		wait.push_back(ety);
	}
	
	for (int i=0;i<n;i++){
		starts_wait.push_back(-1);
		ave_turn.push_back(0);
	}

	while(1){
		count++;
		for (unsigned int i=0;i<t.size();i++){
			if (t[i]!=0){
				t[i]-=1;
				break;
			}
		}
		if (process && current.second>0)
		{
			current.second-=1;
			s=find(id.begin(),id.end(),current.first)-id.begin();
		}	
		if(!process && !q.empty()){
			current = q.top();
			s = find(id.begin(),id.end(),current.first)-id.begin();
			bool switching=false;
			for (unsigned int i=0;i<t.size();i++){
				if (t[i]!=0){
					switching=true;
				}
			}
			if(switching==false)
			{
				if (starts_wait[s]!=-1){
					avg_wt+=count-starts_wait[s]-t_cs/2;
				}
				num_cs+=1;
				process = true;
				q.pop();
				cout<< "time " << count << "ms: Process " << current.first << " started using the CPU [Q";
				print_pqueue(q);

			}	
		}
		if(process && current.second==0)
		{
			if(num_bur[s] > 1){
				num_bur[s] -= 1;
				if(num_bur[s] == 1){
					cout<< "time " << count << "ms: Process " << current.first << " completed a CPU burst; " << num_bur[s] <<  " burst to go [Q";
					print_pqueue(q);
				}
				else{
					cout<< "time " << count << "ms: Process " << current.first << " completed a CPU burst; " << num_bur[s] <<  " bursts to go [Q";
					print_pqueue(q);
				}
				t[s] = t_cs/2;
				cout<< "time " << count << "ms: Process " << current.first << " switching out of CPU; will block on I/O until time " << count + io_t[s]+ t[s] <<  "ms [Q";
				print_pqueue(q);
				turnaround += count - ave_turn[s]+t_cs/2;
				io[s] = count + io_t[s]+ t[s];
				wait[s]=make_pair(current.first, bur_t[s]);
				current=q.top();
				s=find(id.begin(),id.end(),current.first)-id.begin();
				t[s]=t_cs/2;
				process = false;
			}
			else
			{
				cout<< "time " << count << "ms: Process " << current.first << " terminated [Q";
				print_pqueue(q);
				turnaround += count - ave_turn[s]+t_cs/2;
				s=find(id.begin(),id.end(),current.first)-id.begin();
				t[s]=t_cs/2;
				wait[s] = current;
				current=q.top();
				s=find(id.begin(),id.end(),current.first)-id.begin();	
				t[s]=t_cs/2;
				process = false;
				end -= 1;
				
			}
		}
		for(unsigned int i =0; i<io.size(); ++i){
			if(count == io[i]){
				if (wait[i].second>=current.second|| !process)
				{

					q.push(wait[i]);
					s=i;
					starts_wait[i]=count;
					ave_turn[i] = count;
					cout << "time " << count << "ms: Process " << id[i] << " completed I/O; added to ready queue [Q";
					print_pqueue(q);
					if(q.size() == 1 && !process){
						t[s] += t_cs/2;
					}
					io[i] = -1;
				}
				else
				{
					num_pr+=1;
					cout<< "time " << count <<"ms: Process "<< wait[i].first<<" completed I/O and will preempt "<< current.first <<
					"[Q";
					print_pqueue(q);
					q.push(wait[i]);
					q.push(current);
					t[i]=t_cs/2;
					turnaround += count - ave_turn[s]+t_cs/2;
					s=find(id.begin(),id.end(),current.first)-id.begin();
					ave_turn[s] = count+t_cs/2;
					ave_turn[i] = count;					
					starts_wait[s]=count+t_cs/2;
					starts_wait[i]=count+t_cs/2;
					t[s]=t_cs/2;
					process=false;
				}
			}
		}
		for(unsigned int i = 0; i<arr_t.size(); ++i)
		{
			if (arr_t[i] == count){
				if (!process || current.second <= bur_t[i] )
				{
					pair<string,int> temp=make_pair(id[i],bur_t[i]);
					q.push(temp);
					s=i;
					if (!process && (q.top().first==id[i])){
						t[i]=t_cs/2;
					}
					starts_wait[s]=count;
					ave_turn[s] = count;
					cout<< "time " << count << "ms: Process " << id[i] << " arrived and added to ready queue [Q";
					print_pqueue(q);
				}
				else
				{
					num_pr+=1;
					cout<<"time " << count <<"ms: Process " << id[i] << " arrived and will preempt " << current.first <<
					 "[Q";
					 print_pqueue(q);
					 s = find(id.begin(),id.end(),current.first)-id.begin();
					 starts_wait[s]=count+t_cs/2;
					 t[s]= t_cs/2;                                              
					 q.push(current);
					 starts_wait[i]=count+t_cs/2;
					 turnaround += count - ave_turn[i]+t_cs/2;
					 ave_turn[s] = count+t_cs/2;
					 ave_turn[i] = count;
					 pair<string,int> temp=make_pair(id[i],bur_t[i]);
					 q.push(temp);
					 t[i]=t_cs/2;
					 process=false;
				}
			}
		}
		if(end == 0){
			cout << "time "<< count+t_cs/2 <<"ms: Simulator ended for SRT"<<endl;

			outfile << "Algorithm SRT\n";
			outfile << "-- average CPU burst time: " << fixed << setprecision(2) << ave_b/(num_cs*1.0-num_pr);
			outfile << " ms" << endl;
			outfile << "-- average wait time: " << fixed << setprecision(2) << avg_wt/(num_cs*1.0-num_pr);
			outfile << " ms" << endl;
			outfile << "-- average turnaround time: " << fixed << setprecision(2) << turnaround/(num_cs*1.0-num_pr);
			outfile << " ms" << endl;
			outfile << "-- total number of context switches: " + to_string(num_cs) << endl;
			outfile << "-- total number of preemptions: " + to_string(num_pr) <<endl;
			break;
		}

		
	}

}

int main(int argc, char* argv[]){
	//read file
	ifstream in_str(argv[1]);

	if (!in_str.good()){
		// check if the file can be read
		cerr << "Can't open " << argv[1] << " to read.\n"; 
	}

	string a;
	//five vectors one for each column of the file
	vector<string> id;
	vector<int> arr_t;
	vector<int> bur_t;
	vector<int> num_bur;
	vector<int> io_t;

	//dictionary take id as key and other number from taht row as value
	map<string, vector<int> > m;

	while(in_str>>a){
		if(a[0] != '#' && a[0] != 'p' && a[0] != '<'){
			//cout << a << endl;
			parse(a, id, arr_t, bur_t, num_bur, io_t);
			parse_m(a, m);
		}
	}
	n = id.size();

	ofstream out_str(argv[2]); 

	fcfs(id, arr_t, bur_t, num_bur, io_t, out_str);
	cout<<endl;
	srt(id, arr_t, bur_t, num_bur, io_t, out_str);
}
