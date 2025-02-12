#include<iostream>
#include<vector>
#include "file_manager.h"

using namespace std;

extern int MAX_CAP; //maximum number of children
extern int DIM; //dimensionality
extern int ROOT_ID;//ROOT_ID
int ceil(int a, int b) {return (a/b) +(a%b >0);}
int floor(int a, int b) {return (a/b);}
bool isInMBR(int *Mbr, int *P);
bool isLeaf(int *Node);
bool pointQuery(int *P, int NodeId, char *fileName, FileManager fm);
int* get_entry(int node_id, FileHandler* fh, int node_size);
void update_MBR(int* node_data, int num_children);
FileHandler str_bulkload(FileManager fm, char* input, int num_points);
void assign_parents(FileHandler* fh ,int start, int end);
