#include "rtree.h"
#include "errors.h"
#include<cstring>
#include<climits>
#include<cstdlib>

using namespace std;

int MAX_CAP; //maximum number of children
int DIM; //DIMensionality

char* get_entry(int node_id, FileHandler fh, int M)
{
  PageHandler ph = fh.PageAt(node_id/M);
  return (ph.GetData() + S*(node_id%M));
}

FileHandler str_bulkload(FileManager fm, char* input, int num_points)
{
  FileHandler in, out;

  int pieces = ceil(num_points, MAX_CAP);
  int id=1; //unique id
  int node_size = sizeof(int)*(2*DIM + 2 + MAX_CAP*(2*DIM + 1));
  cout << "node size: "<<node_size << endl;
  int S = node_size;
  int M = floor(PAGE_CONTENT_SIZE,S);
  cout <<"M: "<<M<<endl;

  int num_pages = ceil(pieces, M);
  cout << "num_pages: "<<num_pages << endl;

  in = fm.OpenFile(input);
  PageHandler ph = in.FirstPage();
  char* in_data = ph.GetData();

  cout <<"here1\n";
  out = fm.CreateFile("rtree.txt");
  cout <<"here2\n";
  for(int i=1; i<=num_pages; i++) //For each page
  {
    // Create a new page
    PageHandler ph = out.NewPage ();
    char* out_data = ph.GetData ();
    cout << "hi1" << endl;
    for(int j=0; j<M&&num_points!=0; j++) //For each node
    {
      cout << "hi2" << endl;
      int node[node_size];
      int idx = 0;
      //Assigning node ID
      node[idx] = id;
      idx++;
      id++;
      //MBR
      while(idx<=DIM) {node[idx] = INT_MAX; idx++;} //1st point
      while(idx<=2*DIM) {node[idx] = INT_MIN; idx++;} //2nd point
      //parent ID
      node[idx] = -1;
      idx++;
      //MAX_CAP number of children
      int k_max = MAX_CAP;
      if(num_points < MAX_CAP) {k_max=num_points;}
      for(int k=0; k<k_max; k++)
      {
        //MBR
        for(int ctr=0; ctr<DIM; ctr++) {node[idx] = INT_MIN; idx++;} //1st point
        for(int ctr=0; ctr<DIM; ctr++) //2nd point
        {
          int iter = ctr+1;
          int buf;
          memcpy(&buf, in_data, sizeof(int));
          if(node[iter]>buf) node[iter]=buf;
          if(node[iter+DIM]<buf) node[iter+DIM]=buf;

          memcpy(&(node[idx]), &buf, sizeof(int));
          in_data+=sizeof(int);

          idx++;
        }
        //child id
        node[idx] = -1;
        idx++;
      }
      if(k_max != MAX_CAP)
      {
        //for the remaining child slots
        for(int k=k_max; k<MAX_CAP; k++)
        {
          //MBR
          for(int ctr=0; ctr<DIM; ctr++) {node[idx] = INT_MIN; idx++;} //1st point
          for(int ctr=0; ctr<DIM; ctr++) {node[idx] = INT_MIN; idx++;}//2nd point
          //child id
          node[idx] = -1;
          idx++;
        }
      }
      cout << "index: "<<idx<<" num_points: "<<num_points << endl;

      memcpy(out_data+j*node_size, &node[0], node_size*sizeof(int));

      num_points -= MAX_CAP;
    }



  }
  cout << "end ID: "<<id+1<<endl;
  //Assign Parents
  assign_parents(&out, 1, id+1);

  //FLUSH PAGE
  cout << out.FlushPages() << endl;
  return out;
}

void assign_parents(FileHandler* fh ,int start, int end)
{
  int id = end;
  int num_nodes = end - start;
  int node_size = sizeof(int)*(2*DIM + 2 + MAX_CAP*(2*DIM + 1));

  for(int i=start; i<end; i+= MAX_CAP)
  {
    //Get entry with a given node id
    int*
    //Assign a node
    int node[node_size];
    int idx=0;
    //ID
    node[idx] = id;
    idx++;
    id++;
    //MBR
    while(idx<=DIM) {node[idx] = INT_MAX; idx++;} //1st point
    while(idx<=2*DIM) {node[idx] = INT_MIN; idx++;} //2nd point
    //parent ID
    node[idx] = -1;
    idx++;
    //MAX_CAP number of children
    int k_max = MAX_CAP;
    //if(num_points < MAX_CAP) {k_max=num_points;}
    for(int k=0; k<k_max; k++)
    {
      //MBR
      for(int ctr=0; ctr<DIM; ctr++) //1st point
      {
        memcpy(node[idx] = INT_MIN;
        idx++;
      }
      for(int ctr=0; ctr<DIM; ctr++) //2nd point
      {
        int iter = ctr+1;
        int buf;
        memcpy(&buf, in_data, sizeof(int));
        if(node[iter]>buf) node[iter]=buf;
        if(node[iter+DIM]<buf) node[iter+DIM]=buf;

        memcpy(&(node[idx]), &buf, sizeof(int));
        in_data+=sizeof(int);

        idx++;
      }
      //child id
      node[idx] = i;
      idx++;
    }

  }
  cout << "assign parents function\n";
  return;
}

// point query
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isInMBR(int *Mbr, int *P){
    for(int i=0; i< DIM; i++){
        if (Mbr[i] < Mbr[i+DIM]){
            if (!(Mbr[i]<= P[i] <= Mbr[i+DIM]))
                return false;
        }else{
            if (!(Mbr[i+DIM]<= P[i] <= Mbr[i]))
                return false;
        }
    }
    return true;
}

// size of each node = (maxCap+1)*(2*d+1) + 1

    // an int for ID (1)
    // MBR (2*d)
    // parent ID (1)
    // ID and MBR for each child (maxCap*(2*d+1))

// chosen ID for child of a leaf node = '-1'
bool isLeaf(int *Node){
    int check = INT_MIN;
    int start_index = 1+DIM;
    for(int i = start_index; i++; i< (start_index+DIM)){
        if (Node[i] != check)
            return false;
    }
    start_index = 2+2*DIM;
    for(int i=0; i< MAX_CAP; i++){
        for(int j=0; j< 2*DIM; j++){
            if (Node[j+start_index] != check)
                return false;
        }
        if (Node[2*DIM+start_index] != -1)
            return false;
        start_index = start_index + 1 + 2*DIM;
    }
    return true;
}



bool pointQuery(int *P, int NodeId, char *fileName){
    FileManager fm;
    int nodeSize = (((MAX_CAP+1)*(DIM+1)+1)*sizeof(int));
    int pageLimit = PAGE_CONTENT_SIZE/nodeSize;
    int pageIndex = NodeId/pageLimit;
    FileHandler fh = fm.OpenFile(fileName);
    PageHandler ph = fh.PageAt(pageIndex);
    char *data = ph.GetData ();
    int Node[((MAX_CAP+1)*(DIM+1)+1)];
    int nodeIndex = (NodeId - pageLimit*pageIndex - 1)*nodeSize;
    memcpy(&Node, &data[nodeIndex], nodeSize);
    bool result = false;
    if(!isLeaf(Node)){
        int startIndex = 2*DIM+2;
        for(int i=0; i<MAX_CAP; i++){
            if(isInMBR(&Node[startIndex], P)){
                result = result || pointQuery(P, Node[startIndex+2*DIM], fileName);
            }
            startIndex = startIndex + 1 + 2*DIM;
            if (result)
                return result;
        }
    }else{
        int startIndex = 1;
        for(int i=0; i< DIM; i++){
            if(P[i] != Node[i+startIndex])
                return false;
        }
        return true;
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]){
  if(argc != 5)
  {
    cout <<"Incorrect format" << endl;
    return 0;
  }
  MAX_CAP = atoi(argv[2]);
  DIM = atoi(argv[3]);


  string loadfile = "sortedData_2_10_100.txt"; //read from queries
  int loadsize = 100; //read from queries

  FileManager fm;
  str_bulkload(fm, &loadfile[0], loadsize);
  cout <<"----------------------------\n";
  //Open the file
	FileHandler fh = fm.OpenFile(&loadfile[0]);
	cout << "loadfile opened" << endl;
  PageHandler ph = fh.FirstPage();
  char* data = ph.GetData();
  cout << ((int*)data)[1] << endl;
  cout << sizeof(vector<int>)<< sizeof(struct child_data) << " "<< sizeof(struct node) << endl;
  return 0;
}
