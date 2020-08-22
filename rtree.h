#include<iostream>
#include<vector>
#include "file_manager.h"

using namespace std;

extern int MAX_CAP; //maximum number of children
extern int DIM; //dimensionality

int ceil(int a, int b) {return (a/b) +(a%b >0);}
int floor(int a, int b) {return (a/b);}

int* get_entry(int node_id, FileHandler* fh, int node_size);
void update_MBR(int* node_data, int num_children);
FileHandler str_bulkload(FileManager fm, char* input, int num_points);
void assign_parents(FileHandler* fh ,int start, int end);
