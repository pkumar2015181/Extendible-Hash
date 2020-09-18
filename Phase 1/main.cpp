/*
**************************** PRADEEP KUMAR and PRATHMESH TANKSALE ****************************
********************************  2019CSM1008 AND 2019CSM1017 ********************************
*************************************** PG SOFTWARE LAB **************************************
********************************** MAJOR PROJECT PHASE - A ***********************************
*/

#include <bits/stdc++.h>
#define alpha 2000     	// no. of recors in bucket

using namespace std;

int GD = 0; // GLOBAL DEPTH
int expansion = 0;   // count the no. of expansion
int file_count = 1;
int record_count = 0;
int max_TID = 0;

vector<int> bat;   			// vector for BAT
int max_bat = 1024;			// size of BAT (Bucket Address Table)
int current_bat_size = 0;  	// current size of BAT
vector<int> bat_bucket_details;
vector<int> temp_bat;

struct bucket{
	int LD;
	int size;
	int TID[alpha];
	string file_name[alpha];
	int overflow;
	int flag;
	vector<int> hash_table;
	int next_bucket;
};

vector<struct bucket> ssm;  // vector for bucket

struct temp_bucket{
	int TID;
	string file_name;
};

vector<struct temp_bucket> temp_records;   // vector for temperory records


class extendible_hashing{
	public:
		void add_in_file(int tid, int tsa, string custname, int cat, int maxRecordsPerBlock){
			if(record_count < maxRecordsPerBlock){
				char file_name[] = "DB";
				char mid[5];
				itoa(file_count, mid, 10);
				char extension[] = ".txt";
				strcat(file_name, mid);
				strcat(file_name, extension);
						
				ofstream f1;
		  		f1.open(file_name, ios::app);
				if(f1.is_open()){
					f1 << tid << " " << tsa << " " << custname << " " << cat << endl;
					f1.close();
				}
				else{
			        cout << "File is not opening!!\n";
			    }
				record_count++;
			}
			else{
				char file_name[] = "DB";
				char mid[5];
				itoa(file_count, mid, 10);
				char extension[] = ".txt";
				strcat(file_name, mid);
				strcat(file_name, extension);
				
				char next_file_name[] = "DB";
				char next_mid[5];
				itoa(file_count+1, next_mid, 10);
				char next_extension[] = ".txt";
				strcat(next_file_name, next_mid);
				strcat(next_file_name, next_extension);
				
				ofstream f1;
		  		f1.open(file_name, ios::app);
				if(f1.is_open()){
					f1 << -1 << " " << -1 << " " << next_file_name << " " << -1 << endl;
					f1.close();
				}
				else{
			        cout << "File is not opening!!\n";
			    }
			    	    
			    ofstream f2;
		  		f2.open(next_file_name, ios::app);
				if(f2.is_open()){
					f2 << tid << " " << tsa << " " << custname << " " << cat << endl;
					f2.close();
				}
				else{
			        cout << "File is not opening!!\n";
			    }
				
				record_count = 1;
				file_count++;		
			}
		}
		
		void file_open(char *file_name){
			fstream f1;
		  	f1.open(file_name, ios::out | ios::in );
		  	if(f1.is_open()){
				long int TID;
				long int amount;
				string customer_name;
				int category;
				string str;
				while ( getline(f1, str) ){
			    	cout << str << endl << endl;
				    std::istringstream iss(str);
					std::vector<std::string> result(std::istream_iterator<std::string>{iss},
		                                 std::istream_iterator<std::string>());
				    
				    std::stringstream  data(str);
		    		data >> TID >> amount >> customer_name >> category;
		    		//cout << TID << " " << amount << " " << customer_name << " " << category << " " << max_TID << endl;
	    			if(TID <= max_TID && TID > 0){
		    			int exist;
		    			exist = this->search_record(TID);
		    			if(exist == 0){
		    				expansion = 0;
		    				//cout << endl << "Insert" << endl;
			    			this->insert(TID, file_name);
			    			
							//cout << endl;
							/*
							cout << "START ==============================================================" << endl;
							cout << "Global Depth = " << GD << endl;
			    			this->display();
			    			cout << endl;
			    			cout << "END ==============================================================" << endl;
							cout << endl;
							*/
						}
			    		else{
			    			cout << endl << "Transaction ID is already Exist" << endl << endl;
						}	
		    			
					}
		    		if(TID == -1){
						char cstr[customer_name.size() + 1];
						strcpy(cstr, customer_name.c_str());
		    			char *y = "NULL";
		    			if(strcmp(cstr, y)){
			    			this->file_open(cstr);
						}
					}
				}
				f1.close();
			}
		}
		
		void insert(int TID, string file_name){
			//cout << endl << "C1 ";
			if(GD == 0){
				//cout << "C2 ";
				if(ssm.size() == 0){
					//cout << "C3 ";
					ssm.push_back(bucket());
					ssm[0].LD = GD;
					ssm[0].size = 1;
					ssm[0].TID[0] = TID;
					ssm[0].file_name[0] = file_name;
					ssm[0].overflow = -1;
					ssm[0].flag = 0;
				}
				else{
					//cout << "C4 ";
					if(ssm[0].size < alpha){
						//cout << "C5 ";
						ssm[0].TID[ssm[0].size] = TID;
						ssm[0].file_name[ssm[0].size] = file_name;
						ssm[0].size = ssm[0].size + 1;
					}
					else{
						//cout << "C6 ";
						expansion = 1;
						this->bat_expansion(TID, file_name);
					}
				}
			}
			else{
				//cout << "C7 ";
				int TID_MSB;
				TID_MSB = this->MSB_TID(TID);			
				//cout << "TID = " << TID << " MSB = " << TID_MSB << endl;
				//int k = bat[TID_MSB];
				int k = this->return_bat(TID_MSB);
				//cout << k << endl;
				//cout << "C8 ";
				if(ssm[k].size < alpha){
					//cout << "C9 ";
					ssm[k].TID[ssm[k].size] = TID;
					ssm[k].file_name[ssm[k].size] = file_name;
					ssm[k].size = ssm[k].size + 1;
				}
				else{
					//cout << "C10 ";
					if(ssm[k].overflow == -1 && expansion == 1){
						//cout << "C11 ";
						if(ssm[k].LD == GD){
							//cout << "U1 ";
							ssm.push_back(bucket());
							int bucket_index = ssm.size()-1;
							ssm[bucket_index].LD = -1;
							ssm[bucket_index].size = 1;
							ssm[bucket_index].TID[0] = TID;
							ssm[bucket_index].file_name[0] = file_name;
							ssm[bucket_index].overflow = -1;
							ssm[bucket_index].flag = 0;
							ssm[k].overflow = bucket_index;
						}
						else if(ssm[k].LD < GD){
							//cout << "XY17 ";
							ssm[k].LD = ssm[k].LD+1;
							ssm.push_back(bucket());
							int bucket_index = ssm.size()-1;
							ssm[bucket_index].LD = ssm[k].LD;
							ssm[bucket_index].size = 0;
							ssm[bucket_index].overflow = -1;
							ssm[bucket_index].flag = 0;
							
							this->fetch_all_bat();
							
							int temp_bat_size = temp_bat.size();
							/*
							cout << endl << "TEMP BAT size : " << temp_bat_size << endl;
							for(int i=0; i<temp_bat_size; i++){
								cout << temp_bat[i] << endl;
							}
							*/
							
							int count = 0;
							int first = -1;
							for(int c=0; c<temp_bat.size(); c++){
								if(temp_bat[c] == k){
									if(first == -1){
										first = c;
									}
									count++;
								}
							}
							
							count = count/2;
							for(int c=0; c<temp_bat.size(); c++){
								if(temp_bat[c] == k && count > 0){
									count--;
								}
								else if(temp_bat[c] == k && count == 0){
									temp_bat[c] = bucket_index;
								}
							}
							
							temp_bat_size = temp_bat.size();
							/*
							cout << endl << "TEMP BAT size : " << temp_bat_size << endl;
							for(int i=0; i<temp_bat_size; i++){
								cout << temp_bat[i] << endl;
							}
							*/
							
							this->reinsert_bat();
							
							//cout << "U2 ";
							this->empty_bucket(this->return_bat(first), TID, file_name);
							//cout << endl << "Start ReInsert >>>>>>>>>>" << endl;
							this->reinsert_temp_records();
							//cout << "End ReInsert <<<<<<<<<<<" << endl;
							temp_records.clear();
							
						}
							
					}
					else if(ssm[k].overflow == -1 && expansion == 0){
						//cout << "U4 ";
						if(ssm[k].LD == GD){
							//cout << "H1 ";
							expansion = 1;
							this->bat_expansion(TID, file_name);
						}
						else if(ssm[k].LD < GD){
							//cout << "C17 ";
							expansion = 1;
							ssm[k].LD = ssm[k].LD+1;
							ssm.push_back(bucket());
							int bucket_index = ssm.size()-1;
							ssm[bucket_index].LD = ssm[k].LD;
							ssm[bucket_index].size = 0;
							ssm[bucket_index].overflow = -1;
							ssm[bucket_index].flag = 0;
							
							this->fetch_all_bat();
							
							int temp_bat_size = temp_bat.size();
							/*
							cout << endl << "TEMP BAT size : " << temp_bat_size << endl;
							for(int i=0; i<temp_bat_size; i++){
								cout << temp_bat[i] << endl;
							}
							*/
							
							int count = 0;
							int first = -1;
							for(int c=0; c<temp_bat.size(); c++){
								if(temp_bat[c] == k){
									if(first == -1){
										first = c;
									}
									count++;
								}
							}
							
							count = count/2;
							for(int c=0; c<temp_bat.size(); c++){
								if(temp_bat[c] == k && count > 0){
									count--;
								}
								else if(temp_bat[c] == k && count == 0){
									temp_bat[c] = bucket_index;
								}
							}
							
							temp_bat_size = temp_bat.size();
							/*
							cout << endl << "TEMP BAT size : " << temp_bat_size << endl;
							for(int i=0; i<temp_bat_size; i++){
								cout << temp_bat[i] << endl;
							}
							*/
							
							this->reinsert_bat();
							
							//cout << "U2 ";
							this->empty_bucket(this->return_bat(first), TID, file_name);
							//cout << endl << "Start ReInsert >>>>>>>>>>" << endl;
							this->reinsert_temp_records();
							//cout << "End ReInsert <<<<<<<<<<<" << endl;
							temp_records.clear();
							
						}
					}
					else{
						//cout << "C12 ";
						this->insert_in_bucket(ssm[k].overflow, TID, file_name);
					}
				}
				
			}
		}
		
		int return_bat(int index){
			//cout << "RB-1 ";
			//cout << "Index = " << index << endl;
			//cout << "Current bat size = " << current_bat_size << endl;
			if(index >= current_bat_size){
				return -1;
			}
			if(index < max_bat){
				//cout << "RB-2 ";
				return bat[index];
			}
			else{
				//cout << "RB-3 " << endl;
				int bucket_index, bat;
				bucket_index = (index-max_bat)/max_bat;
				//cout << "bucket index = " << bucket_index << endl;
				int bucket = bat_bucket_details[bucket_index];
				//cout << "bucket = " << bucket << endl;
				bat = ssm[bucket].hash_table[index%max_bat];
				//cout << "bat = " << bat << endl;
				return bat;
			}
		}
		
		void insert_in_bucket(int k, int TID, string file_name){
			if(ssm[k].size < alpha){
				//cout << "X9 ";
				ssm[k].TID[ssm[k].size] = TID;
				ssm[k].file_name[ssm[k].size] = file_name;
				ssm[k].size = ssm[k].size + 1;
			}
			else{
				if(ssm[k].overflow == -1){
					if(expansion == 0){
						//cout << "C15 ";
						
						int TID_MSB;
						TID_MSB = this->MSB_TID(TID);			
						//cout << "TID = " << TID << " MSB = " << TID_MSB << endl;
						int k1 = this->return_bat(TID_MSB);
						if(ssm[k1].LD == GD){
							//cout << "F1 ";
							expansion = 1;
							this->bat_expansion(TID, file_name);
						}
						else if(ssm[k1].LD < GD){
							//cout << "C17 ";
							expansion = 1;
							ssm[k1].LD = ssm[k1].LD+1;
							ssm.push_back(bucket());
							int bucket_index = ssm.size()-1;
							ssm[bucket_index].LD = ssm[k1].LD;
							ssm[bucket_index].size = 0;
							ssm[bucket_index].overflow = -1;
							ssm[bucket_index].flag = 0;
							
							this->fetch_all_bat();
							
							int temp_bat_size = temp_bat.size();
							/*
							cout << endl << "TEMP BAT size : " << temp_bat_size << endl;
							for(int i=0; i<temp_bat_size; i++){
								cout << temp_bat[i] << endl;
							}
							*/
							
							int count = 0;
							int first = -1;
							for(int c=0; c<temp_bat.size(); c++){
								if(temp_bat[c] == k1){
									if(first == -1){
										first = c;
									}
									count++;
								}
							}
							
							count = count/2;
							for(int c=0; c<temp_bat.size(); c++){
								if(temp_bat[c] == k1 && count > 0){
									count--;
								}
								else if(temp_bat[c] == k1 && count == 0){
									temp_bat[c] = bucket_index;
								}
							}
							
							temp_bat_size = temp_bat.size();
							/*
							cout << endl << "TEMP BAT size : " << temp_bat_size << endl;
							for(int i=0; i<temp_bat_size; i++){
								cout << temp_bat[i] << endl;
							}
							*/
							
							this->reinsert_bat();
							
							//cout << "U2 ";
							this->empty_bucket(this->return_bat(first), TID, file_name);
							//cout << endl << "Start ReInsert >>>>>>>>>>" << endl;
							this->reinsert_temp_records();
							//cout << "End ReInsert <<<<<<<<<<<" << endl;
							temp_records.clear();
						}
						
					}
					else{
						//cout << "C11 ";
						int TID_MSB;
						TID_MSB = this->MSB_TID(TID);			
						//cout << "TID = " << TID << " MSB = " << TID_MSB << endl;
						int k1 = this->return_bat(TID_MSB);
						if(ssm[k1].LD == GD){
							//cout << "F1 ";
							ssm.push_back(bucket());
							int bucket_index = ssm.size()-1;
							ssm[bucket_index].LD = -1;
							ssm[bucket_index].size = 1;
							ssm[bucket_index].TID[0] = TID;
							ssm[bucket_index].file_name[0] = file_name;
							ssm[bucket_index].overflow = -1;
							ssm[bucket_index].flag = 0;
							ssm[k].overflow = bucket_index;
						}
						else if(ssm[k1].LD < GD){
							//cout << "C17 ";
							
							ssm[k1].LD = ssm[k1].LD+1;
							ssm.push_back(bucket());
							int bucket_index = ssm.size()-1;
							ssm[bucket_index].LD = ssm[k1].LD;
							ssm[bucket_index].size = 0;
							ssm[bucket_index].overflow = -1;
							ssm[bucket_index].flag = 0;
							
							this->fetch_all_bat();
							
							int count = 0;
							int first = -1;
							for(int c=0; c<temp_bat.size(); c++){
								if(temp_bat[c] == k1){
									if(first == -1){
										first = c;
									}
									count++;
								}
							}
							
							count = count/2;
							for(int c=0; c<temp_bat.size(); c++){
								if(temp_bat[c] == k1 && count > 0){
									count--;
								}
								else if(temp_bat[c] == k1 && count == 0){
									temp_bat[c] = bucket_index;
								}
							}
							
							this->reinsert_bat();
							
							//cout << "U2 ";
							this->empty_bucket(this->return_bat(first), TID, file_name);
							//cout << endl << "Start ReInsert >>>>>>>>>>" << endl;
							this->reinsert_temp_records();
							//cout << "End ReInsert <<<<<<<<<<<" << endl;
							temp_records.clear();
							
						}
					}
				}
				else{
					//cout << "U11 ";
					this->insert_in_bucket(ssm[k].overflow, TID, file_name);
				}
			}
		}
		
		void bat_expansion(int TID, string file_name){
			if(GD == 0){
				//cout << "C18 ";
				if(bat.size() == 0){
					//cout << "C18 ";
					//cout << endl << "Pre size : 1" << endl; 
					//cout << "0" << endl;
					GD = 1;
					bat.push_back(0);
					bat.push_back(0);
					current_bat_size = 2;
					int new_size = bat.size();
					//cout << "New size : " << new_size << endl;
					//cout << "0" << endl;
					//cout << "0" << endl;
				}
				this->empty_bucket(0, TID, file_name);
				//cout << endl << "Start ReInsert >>>>>>>>>>" << endl;
				this->reinsert_temp_records();
				//cout << "End ReInsert <<<<<<<<<<<" << endl;
				temp_records.clear();
			}
			else{
				//cout << "C20 ";
				
				int TID_MSB;
				TID_MSB = this->MSB_TID(TID);
				//cout << "MSB of TID : " << TID_MSB << endl;
				
				this->empty_bucket(this->return_bat(TID_MSB), TID, file_name);
				
				int pre_size = bat.size();				
				
				if(2*current_bat_size > max_bat){
					/*
					cout << endl << " S %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%";
					cout << endl << "Pre size : " << pre_size << endl;
					for(int i=0; i<pre_size; i++){
						cout << bat[i] << endl;
					}
					cout << endl;
					*/
					
					this->fetch_all_bat();
					
					int temp_bat_size = temp_bat.size();
					/*
					cout << endl << "TEMP BAT size : " << temp_bat_size << endl;
					for(int i=0; i<temp_bat_size; i++){
						cout << temp_bat[i] << endl;
					}
					*/
					
					temp_bat_size = temp_bat.size();
					for(int i=0; i<temp_bat_size; i++){
						temp_bat.push_back(-1);
					}
					
					int new_temp_bat_size = temp_bat.size();
					for(int i = temp_bat_size-1; i >= 0; i--){
						temp_bat[new_temp_bat_size-1] = temp_bat[i];
						temp_bat[new_temp_bat_size-2] = temp_bat[i];
						new_temp_bat_size = new_temp_bat_size - 2;
					}
					
					temp_bat_size = temp_bat.size();
					/*
					cout << endl << "TEMP BAT size : " << temp_bat_size << endl;
					for(int i=0; i<temp_bat_size; i++){
						cout << temp_bat[i] << endl;
					}
					*/
					
					this->reinsert_bat();
					//cout << endl << " E %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%";
				}
				else{
					for(int i=0; i<pre_size; i++){
						bat.push_back(-1);
					}
					
					int new_size = bat.size();
					//cout << "New size1 : " << new_size << endl;
					for(int i = pre_size-1; i >= 0; i--){
						bat[new_size-1] = bat[i];
						bat[new_size-2] = bat[i];
						new_size = new_size - 2;
					}
					/*	
					for(int i=0; i<bat.size(); i++){
						cout << bat[i] << " " << endl;
					}
					cout << endl;
					*/
				}
				/*
				int new_size = bat.size();
				cout << "New size1 : " << new_size << endl;
				for(int i = pre_size-1; i >= 0; i--){
					bat[new_size-1] = bat[i];
					bat[new_size-2] = bat[i];
					new_size = new_size - 2;
				}
					
				for(int i=0; i<bat.size(); i++){
					cout << bat[i] << " " << endl;
				}
				cout << endl;
				*/
				
				GD = GD + 1;
				current_bat_size = 2*current_bat_size;
				//cout << endl << "Start ReInsert >>>>>>>>>>" << endl;
				this->reinsert_temp_records();
				//cout << "End ReInsert <<<<<<<<<<<" << endl;
				temp_records.clear();
			}
		}
		
		void fetch_all_bat(){
			//cout << endl << "Fetch all" << endl;
			temp_bat.clear();
			for(int i=0; i<bat.size(); i++){
				temp_bat.push_back(bat[i]);
			}
			bat.clear();
			int BBD_size = bat_bucket_details.size();
			if(BBD_size != 0){
				int bucket_no;
				for(int i=0; i<BBD_size; i++){
					bucket_no = bat_bucket_details[i];
					if(ssm[bucket_no].flag == 1){
						int HT_size = ssm[bucket_no].hash_table.size();
						/*
						cout << "Pre Size of HT = " << HT_size << endl;
						for(int j=0; j<HT_size; j++){
							cout << ssm[bucket_no].hash_table[j] << endl;
						}
						*/
						for(int j=0; j<HT_size; j++){
							temp_bat.push_back(ssm[bucket_no].hash_table[j]);
						}
						ssm[bucket_no].hash_table.clear();
						HT_size = ssm[bucket_no].hash_table.size();
						//cout << "New Size of HT = " << HT_size << endl;
					}
				}
			}
			return;
		}
		
		void reinsert_bat(){
			//cout << endl << "Reinsert BAT" << endl;
			int current = 0;
			int size_temp_bat = temp_bat.size();
			if(size_temp_bat > 0){
				int size_bat = bat.size();
				if(size_bat != 0){
					bat.clear();
				}
				
				if(size_temp_bat >= max_bat){
					for(int i=0; i<max_bat; i++){
						bat.push_back(temp_bat[current]);
						current++;
					}
				}
				else{
					for(int i=0; i<size_temp_bat; i++){
						bat.push_back(temp_bat[current]);
						current++;
					}
				}
				
				if(current < size_temp_bat){
					int size_BBD = bat_bucket_details.size();
					//cout << endl << "Pre BBD = " << size_BBD << endl;
					int req_bucket;
					req_bucket = (size_temp_bat - current - max_bat*size_BBD)/max_bat;
					//cout << "req = " << req_bucket << endl;
					for(int k=0; k<req_bucket; k++){
						ssm.push_back(bucket());
						int bucket_index = ssm.size()-1;
						ssm[bucket_index].LD = -1;
						ssm[bucket_index].size = 0;
						ssm[bucket_index].overflow = -1;
						ssm[bucket_index].flag = 1;
						ssm[bucket_index].next_bucket = -1;
						size_BBD = bat_bucket_details.size();
						if(size_BBD > 0){
							if(ssm[bat_bucket_details[size_BBD-1]].next_bucket == -1){
								ssm[bat_bucket_details[size_BBD-1]].next_bucket = bucket_index;
							}
						}
						bat_bucket_details.push_back(bucket_index);
					}
					size_BBD = bat_bucket_details.size();
					//cout << endl << "Next BBD = " << size_BBD << endl;
					if( (size_temp_bat - current - max_bat*size_BBD) == 0){
						//cout << "B1 ";
						for(int k=0; k<size_BBD; k++){
							int bucket = bat_bucket_details[k];
							//cout << "Size of HT = " << ssm[bucket].hash_table.size() << endl;
							for(int y=0; y<max_bat; y++){
								ssm[bucket].hash_table.push_back(temp_bat[current]);
								current++;
							}
							/*
							cout << "Size of HT = " << ssm[bucket].hash_table.size() << endl;
							for(int j=0; j<ssm[bucket].hash_table.size(); j++){
								cout << ssm[bucket].hash_table[j] << endl;
							}
							*/
						}
					}
				}
					
				if(current == size_temp_bat){
					//cout << endl << endl << "Successful" << endl << endl;
				}
			}
		}
		
		
		
		int MSB_TID(unsigned int TID){
			int max_bits, TID_bits, extra_zeros;
			max_bits = (int)log2(max_TID) + 1;
			TID_bits = (int)log2(TID) + 1;
			extra_zeros = max_bits - TID_bits;
						
			if(GD <= extra_zeros){
				return 0;
			}
			else{
				int req, shift, TID_MSB;
				req = GD;
				req = req - extra_zeros;
				shift = TID_bits - req;
				TID_MSB = TID >> shift;
				return TID_MSB;
			}
		}
		
		void empty_bucket(int k, int TID, string file_name){
			int x = temp_records.size();
			if(ssm[k].size != 0){
				//cout << "C21 ";
				int check;
				for(int i=0; i < ssm[k].size; i++){
					check = 0;
					check = check_record_temp_bucket(ssm[k].TID[i]);
					if(check == 0){
						temp_records.push_back(temp_bucket());
						temp_records[x].TID = ssm[k].TID[i];
						temp_records[x].file_name = ssm[k].file_name[i];
						x++;
					}
						
					ssm[k].TID[i] = -1;
					ssm[k].file_name[i] = "";
				}
				ssm[k].size = 0;
				if(ssm[k].overflow != -1){
					//cout << "C22 ";
					empty_bucket(ssm[k].overflow, TID, file_name);
				}
				else{
					//cout << "C23 ";
					check = 0;
					check = check_record_temp_bucket(TID);
					if(check == 0){
						temp_records.push_back(temp_bucket());
						temp_records[x].TID = TID;
						temp_records[x].file_name = file_name;
					}
					
					/*
					cout << endl << "Temp record size : " << temp_records.size() << endl;
					for(int i=0; i<temp_records.size(); i++){
						cout << temp_records[i].TID << "\t";
						cout << temp_records[i].file_name << endl;
					}
					*/
					
				}
											
			}		
			
		}
		
		int check_record_temp_bucket(int TID){
			int found = 0;
			for(int i=0; i < temp_records.size(); i++){
				if(temp_records[i].TID == TID){
					found = 1;
					return found;
				}
			}
			return found;
		}
		
		void reinsert_temp_records(){
			for(int i=0; i < temp_records.size(); i++){
				insert(temp_records[i].TID, temp_records[i].file_name);
			}
		}
		
		void display(){
			if(bat.size() == 0){
				cout << "BAT is empty" << endl;
				if(ssm.size() == 0){
					cout << "No Bucket" << endl;
				}
				else{
					display_bucket_records(0);
				}
			}
			else{
				int i;
				for(i=0; i < bat.size(); i++){
					cout << endl << "BAT = " << i << endl;
					if(bat[i] == bat[i-1]){
						cout << "Both BAT " << i-1 << " and BAT " << i << " pointing to same bucket." << endl; 
					}
					else{
						this->display_bucket_records(bat[i]);
					}
				}
				
				int size_BBD = bat_bucket_details.size();
				cout << "Size of BBD = " << size_BBD << endl;
				
				
				if(size_BBD > 0){
					for(int f=0; f<size_BBD; f++){
						cout << bat_bucket_details[f] << endl;
					}
					
					for(int k=0; k<size_BBD; k++){
						int size_HT = ssm[bat_bucket_details[k]].hash_table.size();
						cout << "Size of HT = " << size_HT << endl;
						
						for(int y=0; y<size_HT; y++){
							cout << ssm[bat_bucket_details[k]].hash_table[y] << endl;
						}
						
						for(int y=0; y<size_HT; y++){
							cout << endl << "BAT = " << i << endl;
							if(y == 0){
								int pre_bat;
								if(k == 0){
									pre_bat = bat[max_bat-1];
								}
								else{
									pre_bat = ssm[bat_bucket_details[k-1]].hash_table[max_bat-1];
								}
								
								if(ssm[bat_bucket_details[k]].hash_table[y] == pre_bat){
									cout << "Both BAT " << i-1 << " and BAT " << i << " pointing to same bucket." << endl;
								}
								else{
									this->display_bucket_records(ssm[bat_bucket_details[k]].hash_table[y]);
								}
							}
							else{
								if(ssm[bat_bucket_details[k]].hash_table[y] == ssm[bat_bucket_details[k]].hash_table[y-1]){
									cout << "Both BAT " << i-1 << " and BAT " << i << " pointing to same bucket." << endl;
								}
								else{
									this->display_bucket_records(ssm[bat_bucket_details[k]].hash_table[y]);
								}
							}
							
							i++;
						}
					}
				}
			}
		}
		
		void display_bucket_records(int k){
			if(ssm[k].size != 0){
				cout << "Bucket Size :" << ssm[k].size;
				cout << ", Empty Spaces :" << alpha-ssm[k].size << endl;
				for(int i=0; i < ssm[k].size; i++){
					cout << ssm[k].TID[i] << "\t";
					cout << ssm[k].file_name[i] << endl;
				}
			}
			else{
				cout << "Bucket is empty";
				cout << ", Empty Spaces :" << alpha << endl;				
			}
			
			if(ssm[k].overflow != -1){
				cout << endl << "Overflow bucket" << endl;
				display_bucket_records(ssm[k].overflow);
			}
		}
		
		int search_record(int TID){
			int found = 0;
			//cout << "SEARCH ";
			if(GD == 0){
				if(ssm.size() > 0){
					this->search_in_bucket(0, TID, &found);
				}
			}
			else{
				int TID_MSB;
				TID_MSB = this->MSB_TID(TID);		
				cout << "TID = " << TID << " MSB = " << TID_MSB << endl;
				if(bat.size() > 0){
					int k = this->return_bat(TID_MSB);
					//cout << " k = " << k << endl;
					if(k != -1){
						this->search_in_bucket(k, TID, &found);
					}
				}
			}
			return found;
		}
		
		void search_in_bucket(int k, int TID, int *found){
			//cout << "Search in Bucket no. = " << k << endl;
			if(ssm[k].size > 0){
				for(int i=0; i<ssm[k].size; i++){
					if(ssm[k].TID[i] == TID){
						*found = 1;
						return;
					}
				}
				
				if(ssm[k].overflow != -1){
					this->search_in_bucket(ssm[k].overflow, TID, found);
				}
				else{
					return;
				}
			}
			else{
				return;
			}
		}
  
};


int main(){	

    extendible_hashing a;
    
    cout << "Enter your choice" << endl;
	cout << "Press '0' for Generate random data or" << endl;
	cout << "Press '1' for Read from given file" << endl;
	
	int read;
	cin >> read;
	if(read == 0){
		
		int maxNumOfRecords = 100000;
		int min_range = 50000;
		int max_range = 200000;
		int maxRecordsPerBlock = 300;
		
		cout << endl << "Default Input to generate data" << endl;
		cout << "Total no. records = " << maxNumOfRecords << endl;
		cout << "Minimum of range of Transaction ID = " << min_range << endl;
		cout << "Maximum of range of Transaction ID = " << max_range << endl;
		cout << "Total no. of records per block = " << maxRecordsPerBlock << endl;
		
		cout << endl << "Do you want to change ??" << endl;
		cout << "Press '0' for continue or" << endl;
		cout << "Press '1' for change" << endl;
		int change;
		cin >> change;
		if(change == 1){
			cout << "Enter Total no. records" << endl;
			cin >> maxNumOfRecords;
			
			cout << "Enter minimum of range of Transaction ID" << endl;
			cin >> min_range;
			
			cout << "Enter maximum of range of Transaction ID" << endl;
			cin >> max_range;
			
			cout << "Enter no. of records per block" << endl;
			cin >> maxRecordsPerBlock;
		}
		
		fstream file;
	    file.open("syntheticTable.txt", ios::trunc | ios::out | ios::in);
	    if(file.is_open()){
	    	int tid;
		    int tsa;
		    string custname;
		    int cat;
			for(int i=0; i<maxNumOfRecords; i++){
				tid = min_range + rand()%(max_range-min_range+1);
				if(tid > max_TID){
					max_TID = tid;
				}
				tsa = 1 + rand()%50000;
				string str1(1, (char)('A' + rand()%26));
		        string str2(1, (char)('a' + rand()%26));
		        string str3(1, (char)('a' + rand()%26));
				custname = str1 + str2 + str3;
				cat = 1 + rand()%1500;
				file << tid << " " << tsa << " " << custname << " " << cat << endl;
			}
			file.close();
	    }
	    else{
	        cout << "File is not opening!!\n";
	    }
	    
	    fstream f1;
	  	f1.open("syntheticTable.txt", ios::out | ios::in );
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
			    //cout << tid << " " << tsa << " " << custname << " " << cat << endl;
			    a.add_in_file(tid, tsa, custname, cat, maxRecordsPerBlock);
			}
			f1.close();
		}
	    else{
	        cout << "File not opening!!\n";
	        exit(0);
	    }
	}
	else{
		cout << "Enter name of file with extension" << endl;
		string name;
		cin >> name;
		char given_filename[name.size()];
		for(int i=0; i<name.size(); i++){
			given_filename[i] = name[i];
		}
		
		int maxRecordsPerBlock;
		int max_range = 0;
		cout << "Enter no. of records per block" << endl;
		cin >> maxRecordsPerBlock;
		
		fstream f1;
	  	f1.open(given_filename, ios::out | ios::in );
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
			    //cout << tid << " " << tsa << " " << custname << " " << cat << endl;
			    if(tid > max_TID){
			    	max_TID = tid;
				}
			    a.add_in_file(tid, tsa, custname, cat, maxRecordsPerBlock);
			}
			f1.close();
		}
	    else{
	        cout << "Given File is not opening!!\n";
	        exit(0);
	    }
	}
    
    char *first_file = "DB1.txt";
    a.file_open(first_file);
    
    int ch, TID, found;
    
    cout << endl << "Menu:" << endl;
    cout << "0 for exit" << endl;
    cout << "1 for display" << endl;
    cout << "2 for search" << endl;
    cout << "3 for Maximum Transaction ID" << endl;
    
    while(1){
    	cout << endl << "Enter your choice" << endl; 
    	cin >> ch;
		switch(ch){
			case 0:
				exit(0);
				break;
			case 1:
				cout << endl << "Global Depth = " << GD << endl;
				cout << endl << "Display records according to increasing order of BAT" << endl;
				a.display();
				cout << endl;
				break;
			case 2:
				cout << "Enter Transaction ID for search" << endl;
				cin >> TID;
				found = a.search_record(TID);
				cout << endl;
				if(found == 1){
					cout << "Transaction ID " << TID << " is exist." << endl;
				}
				else{
					cout << "Transaction ID " << TID << " is not exist." << endl;
				}
				break;
			case 3:
				cout << "Maximum Transaction ID = " << max_TID << endl;
				break;
			default:
				cout << "Wrong choice! Please enter your choice again." << endl;
				break; 
		}
	}
	
    return 0;
}
