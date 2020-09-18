/*
**************************** PRADEEP KUMAR and PRATHMESH TANKSALE ****************************
********************************  2019CSM1008 AND 2019CSM1017 ********************************
*************************************** PG SOFTWARE LAB **************************************
********************************** MAJOR PROJECT PHASE - A ***********************************
*/

/*
		CHANGE THE VALUE OF 
		1. 'alpha' FOR NO. OF RECORDS PER BUCKET
		2. 'max_bat' FOR MAX. NO. OF RECORDS IN BUCKET ADDRESS TABLE
*/

#include <bits/stdc++.h>
#include <algorithm>
#define alpha 200    	// no. of records in bucket

using namespace std;

struct file_data{
	int TID;
	int amount;
	string name;
	int category;
	string filename;
};

vector<struct file_data> data_for_files;  // TO SPLIT LARGER FILE RECORDS INTO SMALL FILE RECORDS

struct index_record{
	int TID;
	string filename;
};

vector<struct index_record> index_details;
vector<struct index_record> temp_index_details;

int GD = 0; // GLOBAL DEPTH
int expansion = 0;   // count the no. of expansion
int max_TID = 0;     // LARGEST TID
int min_TID = -1;
int index_level = 0;   // NO. OF INDEX LEVEL OF PI
int NoDiskBlock = 0;	// TOTAL NO. OF DISK BLOCK FOR EX. HASH
int process_count = 0; 	// to check no. of processed elements
int b_bat;

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

bool compareByTID(const index_record &a, const index_record &b){
    return a.TID < b.TID;
}

int bin_size = 0;
int min_range_hist = 0;
int max_range_hist = 0;
vector<int> hist;	// FOR HISTOGRAM

int output_naive = 0;
int output_ex_hash = 0;
int output_sec = 0;

class extendible_hashing{
	public:
		void split_file(int maxRecordsPerBlock){
			int total_rec = data_for_files.size();
			int file_count = 1;
			int record_count = 0;
			while(record_count < total_rec){
				char file_name[] = "DB";
				char mid[5];
				sprintf(mid, "%d", file_count);
				//itoa(file_count, mid, 10);
				char extension[] = ".txt";
				strcat(file_name, mid);
				strcat(file_name, extension);
						
				ofstream f1;
		  		f1.open(file_name, ios::app);
				if(f1.is_open()){
					for(int j=0; j<maxRecordsPerBlock; j++){
						if(record_count < total_rec){
							f1 << data_for_files[record_count].TID << " " << data_for_files[record_count].amount << " " << data_for_files[record_count].name << " " << data_for_files[record_count].category << endl;
							//data_for_files[record_count].filename = file_name;
							index_details.push_back(index_record());
							index_details[index_details.size() - 1].TID = data_for_files[record_count].TID;
							index_details[index_details.size() - 1].filename = file_name;
							record_count++;
						}
						else{
							record_count++;
						}
					}
					if(record_count < total_rec){
						file_count++;
						char next_file_name[] = "DB";
						char next_mid[5];
						sprintf(next_mid, "%d", file_count);
						//itoa(file_count, next_mid, 10);
						char next_extension[] = ".txt";
						strcat(next_file_name, next_mid);
						strcat(next_file_name, next_extension);
						
						f1 << -1 << " " << -1 << " " << next_file_name << " " << -1 << endl;
					}
					f1.close();
				}
				else{
			        cout << "File is not opening!!\n";
			    }
			}
			NoDiskBlock = file_count;
		}
		
		/*
		void add_in_file(string all_rec_filename, int maxRecordsPerBlock){
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
		*/
		
		void file_open(char *file_name){
			for(int i=0; i<data_for_files.size(); i++){
				long int TID = data_for_files[i].TID;
				long int amount = data_for_files[i].amount;
				string customer_name = data_for_files[i].name;
				int category = data_for_files[i].category;
				if(TID <= max_TID && TID > 0){
    				//process_count++;
	    			//cout << process_count << " " << file_name << endl;
	    			int exist;
	    			exist = this->search_record(TID);
	    			if(exist == 0){
	    				expansion = 0;
	    				//cout << endl << "Insert" << endl;
		    			this->insert(TID, file_name);
		    			
						//cout << endl;
						
//							cout << "START ==============================================================" << endl;
//							cout << "Global Depth = " << GD << endl;
//			    			this->display();
//			    			cout << endl;
//			    			cout << "END ==============================================================" << endl;
//							cout << endl;
						
					}
		    		else{
		    			//cout << endl << "Transaction ID is already Exist" << endl << endl;
					}	
	    			
				}
			}
		}
		/*
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
			    	//cout << str << endl << endl;
				    std::istringstream iss(str);
					std::vector<std::string> result(std::istream_iterator<std::string>{iss},
		                                 std::istream_iterator<std::string>());
				    
				    std::stringstream  data(str);
		    		data >> TID >> amount >> customer_name >> category;
		    		
		    		//cout << TID << " " << amount << " " << customer_name << " " << category << " " << max_TID << endl;
	    			if(TID <= max_TID && TID > 0){
	    				//process_count++;
		    			//cout << process_count << " " << file_name << endl;
		    			int exist;
		    			exist = this->search_record(TID);
		    			if(exist == 0){
		    				expansion = 0;
		    				//cout << endl << "Insert" << endl;
			    			this->insert(TID, file_name);
			    			
							//cout << endl;
							
//							cout << "START ==============================================================" << endl;
//							cout << "Global Depth = " << GD << endl;
//			    			this->display();
//			    			cout << endl;
//			    			cout << "END ==============================================================" << endl;
//							cout << endl;
							
						}
			    		else{
			    			//cout << endl << "Transaction ID is already Exist" << endl << endl;
						}	
		    			
					}
		    		if(TID == -1){
		    			f1.close();
						char cstr[customer_name.size() + 1];
						strcpy(cstr, customer_name.c_str());
		    			char *y = "NULL";
		    			if(strcmp(cstr, y)){
			    			this->file_open(cstr);
						}
						return;
					}
				}
				f1.close();
				return;
			}
		}
		*/
		
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
				//cout << "TID = " << TID << " MSB = " << TID_MSB << endl;
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
		
		void PI_create_file(int maxIndexRecord){
			
			/*
			// Unsorted
			cout << endl;
			cout << "Print all data " << endl << endl;
			for(int i=0; i<data_for_files.size(); i++){
				cout << data_for_files[i].TID << " " << data_for_files[i].filename << endl;
			}
			*/
			
			sort(index_details.begin(), index_details.end(), compareByTID);
						
			/*
			// Sorted
			cout << endl;
			cout << "Print all data after sorting" << endl << endl;
			for(int i=0; i<data_for_files.size(); i++){
				cout << data_for_files[i].TID << " " << data_for_files[i].filename << endl;
			}
			*/
			
			// FOR HISTOGRAM
			if(index_level == 0){
				
				int min_h = min_range_hist;				
				int middle = min_range_hist + bin_size;
				int max_h = max_range_hist;
				int total_count = 0;
				int last_bin = 0;
				//cout << endl << "min = " << min_h << " middle = " << middle << endl;
				
				for(int i=0; i<index_details.size(); i++){
					//cout << data_for_files[i].TID << " " << data_for_files[i].filename << endl;
					if(index_details[i].TID >= min_h && index_details[i].TID <= middle){
						total_count++;
					}
					
					if(index_details[i].TID > middle){
						hist.push_back(total_count);
						//cout << "Total count = " << total_count << endl;
						total_count = 1;
						min_h = middle + 1;
						middle = middle + bin_size;
						//cout << "min = " << min_h << " middle = " << middle << endl;
					}
					
					if(middle == max_h && last_bin == 0 && i == index_details.size()-1){
						last_bin = 1;
						hist.push_back(total_count);
						//cout << "Total count == " << total_count << endl;
					}
				}
			}				
			
			int PI_rec_total = index_details.size();
			int PI_file_count = 1;
			int PI_rec_count = 0;
			
			// FOR TEMP RECORDS
			temp_index_details.clear();
			
			while(PI_rec_count < PI_rec_total){
				
				char file_name[50] = "Leaf_";
				
				char mid1[5];
				sprintf(mid1, "%d", index_level);
				strcat(file_name, mid1);
				
				char middle[10] = "_file_";
				strcat(file_name, middle);
								
				char mid2[5];
				sprintf(mid2, "%d", PI_file_count);
				strcat(file_name, mid2);
								
				char extension[10] = ".txt";
				strcat(file_name, extension);
								
				ofstream f1;
		  		f1.open(file_name, ios::trunc | ios::out | ios::in);
				if(f1.is_open()){
					for(int j=0; j<maxIndexRecord; j++){
						if(PI_rec_count < PI_rec_total){
							if(j == 0){
								temp_index_details.push_back(index_record());
								temp_index_details[temp_index_details.size() - 1].TID = index_details[PI_rec_count].TID;
								temp_index_details[temp_index_details.size() - 1].filename = file_name;
							}
							f1 << index_details[PI_rec_count].TID << " " << index_details[PI_rec_count].filename << endl;
							PI_rec_count++;
						}
					}
					if(PI_rec_count < PI_rec_total){
											
						PI_file_count++;
						
						char next_file_name[50] = "Leaf_";
						
						char mid1[5];
						sprintf(mid1, "%d", index_level);
						strcat(next_file_name, mid1);
						
						char middle[10] = "_file_";
						strcat(next_file_name, middle);
						
						char next_mid2[5];
						sprintf(next_mid2, "%d", PI_file_count);
						strcat(next_file_name, next_mid2);
						
						char extension[10] = ".txt";
						strcat(next_file_name, extension);
						
						f1 << -1 << " " << next_file_name << endl;
					}
					f1.close();
				}
				else{
			        cout << "File is not opening!!\n";
			    }
			}
			
			cout << endl << "No. of files at level " << index_level << " = " << temp_index_details.size();
			if(temp_index_details.size() > 1){
				index_details.clear();
				index_level++;
				for(int k=0; k< temp_index_details.size(); k++){
					index_details.push_back(index_record());
					index_details[index_details.size() - 1].TID = temp_index_details[k].TID;
					index_details[index_details.size() - 1].filename = temp_index_details[k].filename;
				}
				temp_index_details.clear();
				this->PI_create_file(maxIndexRecord);
			}
			
		}
		
		void search_record_PI(char *file_name, int x, int y, int *file_access, int *found){
			char next_level_filename[100];
			//cout << endl << "H1 " << file_name << endl;
			int flag = 0; // 1 for leaf level
			fstream f1;
		  	f1.open(file_name, ios::out | ios::in );
		  	if(f1.is_open()){
		  		(*file_access)++;
				long int TID;
				string next_filename;
				string str;
				while ( getline(f1, str) ){
					//cout << endl << "String = " << str << endl;
				    std::istringstream iss(str);
					std::vector<std::string> result(std::istream_iterator<std::string>{iss},
		                                 std::istream_iterator<std::string>());
				    
				    std::stringstream  data(str);
		    		data >> TID >> next_filename;
		    		
		    		//cout << "N FN1 = " << next_filename;
	    			char cstr[next_filename.size() + 1];
					strcpy(cstr, next_filename.c_str());
					
		    		char check[50] = "DB";
		    		char *p;
		    		p = strstr(cstr, check); 
					if(p){ 
					    flag = 1;
					    f1.close();
		    			this->search_in_leaf_level(file_name, x, y, file_access, found);
		    			return;
					}
		    		else{
		    			if(TID <= x){	
			    			strcpy(next_level_filename, cstr);
						}
						else{
							break;
						}
					}
				}
				f1.close();
			}
			else{
				cout << "File is not opening" << endl;
			}
			
			if(flag == 0){
				char *ptr = next_level_filename;
				//cout << "Next FN = " << *ptr;
				this->search_record_PI(ptr, x, y, file_access, found);
			}
		}
		
		void search_in_leaf_level(char *file_name, int x, int y, int *file_access, int *found){
			
			//cout << endl << "Search in LL => " << x << " => " << y << " " << *found << endl;
			fstream f1;
		  	f1.open(file_name, ios::out | ios::in );
		  	if(f1.is_open()){
		  		(*file_access)++;
				long int TID;
				string customer_filename;
				string str;
				while ( getline(f1, str) ){
			    	//cout << str << endl << endl;
				    std::istringstream iss(str);
					std::vector<std::string> result(std::istream_iterator<std::string>{iss},
		                                 std::istream_iterator<std::string>());
				    
				    std::stringstream  data(str);
		    		data >> TID >> customer_filename;
		    		
		    		if(TID == -1){
		    			f1.close();
						char cstr[customer_filename.size() + 1];
						strcpy(cstr, customer_filename.c_str());
		    			char *stry = "NULL";
		    			if(strcmp(cstr, stry)){
			    			this->search_in_leaf_level(cstr, x, y, file_access, found);
						}
						return;
					}
		    		
		    		if(TID >= x && TID <= y){
		    			(*found)++;
		    			if(output_sec){
		    				cout << "Found = " << TID << " " << customer_filename << endl;
						}
					}
					
					if(TID > y){
						f1.close();
						return;
					}
				}
				f1.close();
			}
			else{
				cout << "File is not opening" << endl;
			}
			return;
		}
		
		void naive_algo(char *file_name, int x, int y, int *file_access, int *found){
			
			for(int i=1; i<=NoDiskBlock; i++){
				
				char root_filename[50] = "DB";
				
				char mid1[5];
				sprintf(mid1, "%d", i);
				strcat(root_filename, mid1);
				
				char middle[20] = ".txt";
				strcat(root_filename, middle);
							
				char *file_name = root_filename;
				//cout << root_fn;
				fstream f1;
			  	f1.open(file_name, ios::out | ios::in );
			  	if(f1.is_open()){
			  		(*file_access)++;
					long int TID;
					long int amount;
					string customer_name;
					int category;
					string str;
					while ( getline(f1, str) ){
				    	//cout << str << endl << endl;
					    std::istringstream iss(str);
						std::vector<std::string> result(std::istream_iterator<std::string>{iss},
			                                 std::istream_iterator<std::string>());
					    
					    std::stringstream  data(str);
			    		data >> TID >> amount >> customer_name >> category;
			    		//cout << TID << " " << amount << " " << customer_name << " " << category << " " << max_TID << endl;
		    			if(TID >= x && TID <= y){
		    				(*found)++;
		    				if(output_naive){
		    					cout << TID << " " << amount << " " << customer_name << " " << category << " " << file_name << endl;
							}
			    			
						}
						
			    		if(TID == -1){
			    			// do nothing
			    			break;
						}
					}
					f1.close();
				}
				else{
					cout << "File is not opening" << endl;
				}
			}				
		}
		
		void RQ_search_record(int TID, int *access, int *found){
			if(GD == 0){
				if(ssm.size() > 0){
					this->RQ_search_in_bucket(0, TID, access, found);
				}
			}
			else{
				int TID_MSB;
				TID_MSB = this->MSB_TID(TID);
				if(bat.size() > 0){
					int k = this->return_bat(TID_MSB);
					//cout << " k = " << k << endl;
										
					if(TID_MSB >= max_bat){
						int bucket_index;
						bucket_index = ceil((TID_MSB-max_bat)/b_bat);
						(*access) = (*access) + bucket_index + 1;
					}
					
					if(k != -1){
						this->RQ_search_in_bucket(k, TID, access, found);
					}
				}
			}
		}
		
		void RQ_search_in_bucket(int k, int TID, int *access, int *found){
			//cout << "Search in Bucket no. = " << k << endl;
			(*access)++;
			if(ssm[k].size > 0){
				for(int i=0; i<ssm[k].size; i++){
					if(ssm[k].TID[i] == TID){
						if(output_ex_hash){
							cout << "Found = " << ssm[k].TID[i] << " " << ssm[k].file_name[i] << endl;
						}
						(*found)++;
						return;
					}
				}
				
				if(ssm[k].overflow != -1){
					this->RQ_search_in_bucket(ssm[k].overflow, TID, access, found);
				}
				else{
					return;
				}
			}
			else{
				return;
			}
		}
		
		void count_total_bucket(int *bucket, int *bat){
			int count_bucket = 0;
			int count_bat = 0;
			int ssm_size;
			ssm_size = ssm.size();
			for(int i=0; i<ssm_size; i++){
				if(ssm[i].flag != 1 && ssm[i].size > 0){
					count_bucket++;
				}
				
				if(ssm[i].flag == 1){
					count_bat++;
				}
			}
			*bucket = count_bucket;
			*bat = count_bat;
		}
		
		void best_algo(int x, int y, int bucket_as_bat, int bucket_as_bucket, int index_block_factor){
			int cost_ex_hash = 0;
			int cost_naive = 0;
			int cost_PI = 0;
			
			double theta = 0;
			
			int min_h = min_range_hist;				
			int middle = min_range_hist + bin_size;
			int max_h = max_range_hist;
			
			//cout << endl << "min = " << min_h << " middle = " << middle << endl;
				
			for(int i=0; i<hist.size(); i++){
				
				if(x > middle || y < min_h){
					//cout << " C1 ";
					// do nothing
				}
				else if(x >= min_h && x <= middle){
					//cout << " C2 ";
					theta = theta + ceil((( (double)middle - (double)x)/(double)bin_size)*(double)hist[i]);
				}
				else if(x < min_h && middle <= y){
					//cout << " C3 ";
					theta += hist[i];
				}
				else if(y >= min_h && y <= middle){
					//cout << " C4 ";
					theta = theta + ceil((((double)y - (double)min_h)/(double)bin_size)*(double)hist[i]);
				}
				
				min_h = middle + 1;
				middle = middle + bin_size;
				//cout << "theta = " << theta << endl;
				//cout << "min = " << min_h << " middle = " << middle << endl;
			}
			
			cout << endl << "Calculated value of theta = " <<  theta << endl;
			
			//cout << "Cost of Ex. Hash 1 = " << cost_ex_hash << endl;
			cost_ex_hash += ceil((double)(y-x+1)*(double)((double)bucket_as_bat/(double)2));
			//cout << "Cost of Ex. Hash 2 = " << cost_ex_hash << endl;
			cost_ex_hash += ceil((double)(y-x+1)*((double)bucket_as_bucket/(double)(pow(2,GD))));
			//cout << "Cost of Ex. Hash 3 = " << cost_ex_hash << endl;
			cost_ex_hash += (int)theta;
			cout << "Cost of Ex. Hash = " << cost_ex_hash << endl;
			
			cost_PI = index_level + ceil((double)theta/(double)index_block_factor) + (int)theta;
			cout << "Cost of Sec. index = " << cost_PI << endl;
			
			cost_naive = NoDiskBlock;
			cout << "Cost of Naive = " << cost_naive << endl;
			
			if(cost_naive <= cost_ex_hash && cost_naive <= cost_PI){
				cout << "Naive algo will best method for range query between " << x << " and " << y << endl;
			}
			else if(cost_ex_hash <= cost_naive && cost_ex_hash <= cost_PI){
				cout << "Extendible hash algo will best method for range query between " << x << " and " << y << endl;
			}
			else if(cost_PI <= cost_naive && cost_PI <= cost_ex_hash){
				cout << "Secondary memory based index algo will best method for range query between " << x << " and " << y << endl;
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
				//tid = i+1;
				if(tid > max_TID){
					max_TID = tid;
				}
				
				if(min_TID == -1){
					min_TID = tid;
				}
				else{
					if(tid < min_TID){
						min_TID = tid;
					}
				}
				
				tsa = 1 + rand()%50000;
				string str1(1, (char)('A' + rand()%26));
		        string str2(1, (char)('a' + rand()%26));
		        string str3(1, (char)('a' + rand()%26));
				custname = str1 + str2 + str3;
				cat = 1 + rand()%1500;						
				file << tid << " " << tsa << " " << custname << " " << cat << endl;
				
				data_for_files.push_back(file_data());
				int file_rec_index = data_for_files.size() -1;
				data_for_files[file_rec_index].TID = tid;
				data_for_files[file_rec_index].amount = tsa;
				data_for_files[file_rec_index].name = custname;
				data_for_files[file_rec_index].category = cat;
			}
			file.close();
	    }
	    else{
	        cout << "File is not opening!!\n";
	    }
	    
	    a.split_file(maxRecordsPerBlock);
	    
	    /*
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
	    */
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
				
				if(min_TID == -1){
					min_TID = tid;
				}
				else{
					if(tid < min_TID){
						min_TID = tid;
					}
				}
				
			    data_for_files.push_back(file_data());
				int file_rec_index = data_for_files.size() -1;
				data_for_files[file_rec_index].TID = tid;
				data_for_files[file_rec_index].amount = tsa;
				data_for_files[file_rec_index].name = custname;
				data_for_files[file_rec_index].category = cat;
			}
			f1.close();
		}
	    else{
	        cout << "Given File is not opening!!\n";
	        exit(0);
	    }
	    
	    a.split_file(maxRecordsPerBlock);
	}
	
	
	cout << "Enter bat bucket size" << endl;
	cin >> b_bat;
    
    char *first_file = "DB1.txt";
    a.file_open(first_file);
    
    int total_bucket_for_bucket = 0;
    int total_bucket_for_bat = 0;
    
    a.count_total_bucket(&total_bucket_for_bucket, &total_bucket_for_bat);
    total_bucket_for_bat =  ceil((pow(2, GD)-max_bat)/b_bat);
    
    cout << endl << "Total no. of disk blocks = " << NoDiskBlock << endl;
    cout << "Total no. of buckets = " << total_bucket_for_bucket << endl;
    cout << "Total no. of buckets used by BAT = " << total_bucket_for_bat << endl;
    cout << "Total size of BAT = " << pow(2, GD) << endl;
    
    cout << endl << "Extendible hash index structure have completed" << endl;
    
    int max_index;
    cout << endl << "Enter no. of records per Index Block" << endl;
	cin >> max_index;
	
	cout << "Enter size of bin for histogram" << endl;
	cin >> bin_size;
	cout << "Enter min. range of histogram" << endl;
	cin >> min_range_hist;
	cout << "Enter max. range of histogram" << endl;
	cin >> max_range_hist;
	
    a.PI_create_file(max_index);
    
    cout << endl << "Total index level = " << index_level << endl;
    
    cout << endl << "Secondary memory based index structure have completed" << endl;
    
    /*
    cout << endl << endl;
    cout << "Hist value => ";
    for(int h=0; h<hist.size(); h++){
    	cout << hist[h] << " ";
	}
	
	cout << endl << endl;
	*/
    
    int search_x, search_y;
    
    char root_filename[50] = "Leaf_";
				
	char mid1[5];
	sprintf(mid1, "%d", index_level);
	strcat(root_filename, mid1);
	
	char middle[20] = "_file_1.txt";
	strcat(root_filename, middle);
				
	char *root_fn = root_filename;
    //cout << root_fn;
    
    int total_file_access = 0;
	int total_found = 0;
				  
    int ch, TID, found;
    
    cout << endl << "Menu:" << endl;
    cout << "0 for exit" << endl;
    cout << "1 for display" << endl;
    cout << "2 for search" << endl;
    cout << "3 for minimum transaction ID" << endl;
    cout << "4 for maximum transaction ID" << endl;
    cout << "5 for range query" << endl;
    
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
				cout << "Minimum Transaction ID = " << min_TID << endl;
				break;
			case 4:
				cout << "Maximum Transaction ID = " << max_TID << endl;
				break;
			case 5:
				cout << endl << "Enter the value of x for search" << endl;
			    cin >> search_x;
			    cout << "Enter the value of y for search" << endl;
			    cin >> search_y;
			    
			    a.best_algo(search_x, search_y, total_bucket_for_bat, total_bucket_for_bucket, max_index);
			    
			    cout << endl << "Do you want the output of Extendible hash index structure (1 for yes and 0 for no)" << endl;
			    cin >> output_ex_hash;
			    
			    cout << "Do you want the output of Secondary memory based index (1 for yes and 0 for no)" << endl;
			    cin >> output_sec;
			    
			    cout << "Do you want the output of naive algo (1 for yes and 0 for no)" << endl;
			    cin >> output_naive;
			    
			    
			    cout << endl << "Search using Extendible hash index structure";
				total_file_access = 0;
				total_found = 0;
				for(int m = search_x; m <= search_y; m++){
					a.RQ_search_record(m, &total_file_access, &total_found);
				}
			    cout << endl << "Total Found records = " << total_found << endl;
			    cout << "Total Files access = " << total_file_access+total_found << endl;
			    
			    
			    cout << endl << "Search using Range Query of secondary memory index structure" << endl << endl;
			    total_file_access = 0;
				total_found = 0;
			    if(search_x < min_TID){
			    	a.search_record_PI(root_fn, min_TID, search_y, &total_file_access, &total_found);
				}
				else{
					a.search_record_PI(root_fn, search_x, search_y, &total_file_access, &total_found);
				}
			    cout << "Total Found records = " << total_found << endl;
			    cout << "Total Files access = " << total_file_access-1+total_found << endl;
			    
			    
			    cout << endl << "Search using Naive Algorithm" << endl << endl;
				total_file_access = 0;
				total_found = 0;
			    a.naive_algo(first_file, search_x, search_y, &total_file_access, &total_found);
			    cout << "Total Found records = " << total_found << endl;
			    cout << "Total Files access = " << total_file_access << endl;
			    			    
			    break;
			default:
				cout << "Wrong choice! Please enter your choice again." << endl;
				break; 
		}
	}
	
    return 0;
}
