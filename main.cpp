#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <iomanip>
using namespace std; 

void parse(string &a, vector<string> &id, vector<int> &arr_t, vector<int> &bur_t, vector<int> &num_bur, vector<int> &io_t){
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
	

	while(in_str>>a){
		if(a[0] != '#' && a[0] != 'p' && a[0] != '<'){
			//cout << a << endl;
			parse(a, id, arr_t, bur_t, num_bur, io_t);
		}
	}

	for(unsigned int i=0; i<id.size(); i++){
		cout << id[i]<< " "<< arr_t[i] << " "<< bur_t[i] << " "<< num_bur[i] << " "<< io_t[i] << endl;
	}
	
}