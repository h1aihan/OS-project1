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
using namespace std; 

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

void fcfs(vector<string> id, vector<int> arr_t, vector<int> bur_t, vector<int> num_bur, vector<int> io_t){
	cout << "time 0ms: Simulator started for FCFS [Q <empty>]" <<  endl;
	int end = id.size();
	string current;
	vector<string> q;
	vector<int> s;
	vector<int> t;
	vector<int> io;
	int count = -1;
	bool process=false;
	for (unsigned int i = 0; i<id.size(); ++i){
		io.push_back(-1);
	}
	while(1){
		count++;	
		for(unsigned int i = 0; i<arr_t.size(); ++i){
			if (arr_t[i] == count){
				q.push_back(id[i]);
				s.push_back(i);
				t.push_back(count+4);
				cout<< "time " << count << "ms: Process " << id[i] << " arrived and added to ready queue [Q";
				for (unsigned int j = 0; j < q.size(); ++j){
					cout << " " << q[j];
				}
				cout << "]" << endl;
			}
		}
		if(!process && !s.empty() && count == t[s[0]]){
			process = true;
			cout<< "time " << count << "ms: Process " << q[0] << " started using the CPU [Q";
			current = q[0];
			q.erase(q.begin());
			printv(q);
		} 
		if(process && count == bur_t[s[0]]+t[s[0]]){
			if(num_bur[s[0]] > 1){
				num_bur[s[0]] -= 1;
				if(num_bur[s[0]] == 1){
					cout<< "time " << count << "ms: Process " << current << " completed a CPU burst; " << num_bur[s[0]] <<  " burst to go [Q";
					printv(q);
				}
				else{
					cout<< "time " << count << "ms: Process " << current << " completed a CPU burst; " << num_bur[s[0]] <<  " bursts to go [Q";
					printv(q);
				}
				t[s[0]] = count + io_t[s[0]]+4;
				cout<< "time " << count << "ms: Process " << current << " switching out of CPU; will block on I/O until time " << t[s[0]] <<  "ms [Q";
				printv(q);

				io[s[0]] = t[s[0]];
				s.erase(s.begin());
				if(s.size() > 0){
					t[s[0]] = count + 8;
				}
				process = false;
			}
			else{
				cout<< "time " << count << "ms: Process " << current << " terminated [Q";
				printv(q);
				s.erase(s.begin());
				if(s.size() > 0){
					t[s[0]] = count + 8;
				}
				process = false;
				end -= 1;
				//cout <<  "s size " <<  s.size() << " q size "<< q.size() << endl;
				
			}
			//cout << "s[0] "<< s[0] << " t " << t[s[0]] <<endl;
		}
		for(unsigned int i =0; i<io.size(); ++i){
			if(count == io[i]){
				q.push_back(id[i]);
				s.push_back(i);

				//cout << "io " << io[i] << " i " << i <<endl;
				cout << "time " << count << "ms: Process " << id[i] << " completed I/O; added to ready queue [Q";
				//t[s[0]] += 4;
				printv(q);
				if(q.size() == 1 && !process){
					t[s[0]] += 4;
				}
				//cout << t[s[0]] << endl;
				io[i] = -1;
			}
		}
		if(end == 0){
			cout << "time "<< count+4 <<"ms: Simulator ended for FCFS"<<endl;
			break;
		}

		/*
		if(!t.empty()){
			cout << "t is ";
			for(unsigned int i =0; i<t.size(); ++i){
				cout << t[i] << " ";
			}
			cout <<endl;
		}*/
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
	
	vector<string> id;
	vector<int> arr_t;
	vector<int> bur_t;
	vector<int> num_bur;
	vector<int> io_t;
	map<string, vector<int> > m;

	while(in_str>>a){
		if(a[0] != '#' && a[0] != 'p' && a[0] != '<'){
			//cout << a << endl;
			parse(a, id, arr_t, bur_t, num_bur, io_t);
			parse_m(a, m);
		}
	}

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

	fcfs(id, arr_t, bur_t, num_bur, io_t);

}
