#include<iostream>
#include<vector>
#include "file_manager.h"

using namespace std;

extern int MAX_CAP; //maximum number of children
extern int DIM; //dimensionality


struct child_data{
  int id;
  int* mbr_1; //1st bound of all dimensions
  int* mbr_2; //2nd bound of all dimensions
};

struct node{
  int id;
  int id_parent;
  int* mbr_1; //1st bound of all dimensions
  int* mbr_2; //2nd bound of all dimensions
  child_data* children;
};

FileHandler str_bulkload(FileManager fm, char* input, int num_points);
void assign_parents(FileHandler* fh ,int start, int end);
