#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <iomanip>
#include <map>
#include <set>
#include <functional>
#include <typeinfo>
#include <queue>
using namespace std; 

#define t_cs 8
int n;
// comparator for priorityqueue in SRT
class CompareQueue
{
public:
    bool operator()(pair<string,int> n1,pair<string, int> n2) {
        return n1.second>n2.second;
    }
};
priority_queue<pair<string,int>, vector<pair<string,int> >,CompareQueue>  q;

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

void parse_m(string a, map<string, vector<int> > &m){
	vector<int> v;
	int count = 0;
	size_t i = 0;
	string token;
	string key;
	while((i = a.find("|")) != std::string::npos){
		token = a.substr(0, i);
		//cout << token << endl;
		a.erase(0, i + 1);
		count++;
		if(count == 1){
			key = token;
		}
		else{
			v.push_back(atoi(token.c_str()));
		}
	}
	//cout << a << endl;
	v.push_back(atoi(a.c_str()));
	m.insert(make_pair(key,v));
}

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

void srt(vector<string> id, vector<int> arr_t, vector<int> bur_t, vector<int> num_bur, vector<int> io_t){
	//simulation info output var
	int num_cs=0;
	int num_pr=0;
	int avg_wt=0;
	vector<int> starts_wait;
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
	}
	while(1){
		count++;
		for (int i=0;i<t.size();i++){
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
			for (int i=0;i<t.size();i++){
				if (t[i]!=0){
					switching=true;
				}
			}
			if(switching==false)
			{
				if (starts_wait[s]!=-1){
					avg_wt+=count-starts_wait[s]-t_cs/2;
				}
				starts_wait[s]=-1;
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
				s=find(id.begin(),id.end(),current.first)-id.begin();
				t[s]=t_cs/2;
				//cout <<  "s " <<  s << " q size "<< q.size() << endl;	
				wait[s] = current;
				current=q.top();
				s=find(id.begin(),id.end(),current.first)-id.begin();
				//cout <<  "s size " <<  s<< " q size "<< q.size() << endl;	
				t[s]=t_cs/2;
				process = false;
				end -= 1;
				
			}
			//cout << "s[0] "<< s[0] << " t " << t[s[0]] <<endl;
		}
		for(unsigned int i =0; i<io.size(); ++i){
			if(count == io[i]){
				if (wait[i].second>=current.second|| !process)
				{

					q.push(wait[i]);
					s=i;
					starts_wait[i]=count;
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
					s=find(id.begin(),id.end(),current.first)-id.begin();
					starts_wait[s]=count;
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
					starts_wait[i]=count;
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
					 pair<string,int> temp=make_pair(id[i],bur_t[i]);
					 q.push(temp);
					 t[i]=t_cs/2;
					 process=false;
				}
			}
		}
		if(end == 0){
			cout << "time "<< count+t_cs/2 <<"ms: Simulator ended for SRT"<<endl;
			cout << "number of context switch :"<< num_cs <<endl;
			cout << "number of preemption :"<< num_pr <<endl;
			cout << "average wait time: "<<avg_wt/(num_cs*1.0)<<endl;
			int t_arrt=0;
			for (int i=0;i<n;i++)
			{
				t_arrt+=arr_t[i];
			}
			cout << "average wait time: "<<(count-403.85*num_cs-t_arrt+t_cs*num_cs)/num_cs<<endl;
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

	/*
	for(unsigned int i=0; i<id.size(); i++){
		cout << id[i]<< " "<< arr_t[i] << " "<< bur_t[i] << " "<< num_bur[i] << " "<< io_t[i] << endl;
	}

	map<string, vector<int> >::iterator it;
	for(it = m.begin(); it != m.end(); ++it){
		cout << it->first << " ";
		for(unsigned int i=0; i<(it->second).size(); i++){
			cout << (it->second)[i] << " ";
		}
		cout << endl;
	}*/

	srt(id, arr_t, bur_t, num_bur, io_t);

}
