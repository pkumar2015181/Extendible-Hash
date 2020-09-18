#include <bits/stdc++.h>
using namespace std;

int main(){
    int n, c, k, size, bits;
    int i = 1;
    size = 31;
    bits = (int)log2(size);
    
    fstream f1;
  	f1.open("s.txt", ios::out | ios::in );
  	if(f1.is_open()){
		int tid;
	    int tsa;
	    string custname;
	    int cat;
		string str;
		while ( getline(f1, str) ){
	    	//cout << str << endl;
		    std::istringstream iss(str);
			std::vector<std::string> result(std::istream_iterator<std::string>{iss},
                                 std::istream_iterator<std::string>());
		    
		    std::stringstream   data(str);
    		data >> tid >> tsa >> custname >> cat;
		    n = tid;
	        printf("%d %d ", i, tid);
	        for (c = bits; c >= 0; c--){
	            k = n >> c;
	            if (k & 1)
	              printf("1");
	            else
	              printf("0");
	        }
	        printf("\n");
	        i++;
		}
		f1.close();
	}
    else{
        cout << "File not opening!!\n";
        exit(0);
    }
    
  	return 0;
}

