#include "rtree.h"
#include "errors.h"
#include<cstring>
#include<climits>
#include<cstdlib>

using namespace std;

int MAX_CAP; //maximum number of children
int DIM; //DIMensionality
int ROOT_ID;


int* get_entry(int node_id, FileHandler* fh, int node_size)
{
  int M = floor(PAGE_CONTENT_SIZE,node_size);
  PageHandler ph = fh->PageAt((node_id-1)/M);
  return (int*)((ph.GetData() + node_size*((node_id-1)%M)));
}

void update_MBR(int* node_data, int num_children)
{
  int pt_1[DIM], pt_2[DIM];
  int offset = 2 + 2*DIM;
  int child_size = 2*DIM + 1;
  for(int i=0; i<DIM; i++)
  {
    int buf_1 = node_data[offset + i];
    int buf_2 = node_data[offset + DIM + i];
    for(int j=1; j<num_children; j++)
    {
      if(buf_1 > node_data[offset + j*child_size + i]) buf_1 = node_data[offset + j*child_size + i];
      if(buf_2 < node_data[offset + j*child_size + DIM + i]) buf_2 = node_data[offset + j*child_size + DIM + i];
    }
    pt_1[i] = buf_1;
    pt_2[i] = buf_2;
  }
  memcpy(&node_data[1],&pt_1[0],DIM*sizeof(int));
  memcpy(&node_data[DIM+1],&pt_2[0],DIM*sizeof(int));
  return;
}

FileHandler str_bulkload(FileManager* fm, char* input, int num_points)
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

  in = fm->OpenFile(input);
  PageHandler ph = in.FirstPage();
  char* in_data = ph.GetData();

  cout <<"here1\n";
  out = fm->CreateFile("rtree.txt");
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
      int node[node_size/sizeof(int)];
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

      memcpy(out_data+j*node_size, &node[0], node_size);

      num_points -= MAX_CAP;
    }



  }
  cout << "end ID: "<<id<<endl;
  //Assign Parents
  assign_parents(&out, 1, id);

  //FLUSH PAGE
  PageHandler ph1 = out.FirstPage();
  cout << "yay" << endl;
  cout << fm->CloseFile(out) << endl;
  return out;
}

void assign_parents(FileHandler* fh , int start, int end)
{
  cout << "level" <<" start: "<<start<<" "<<"end: "<<end<<endl;

  int num_nodes = end - start;
  int node_size = sizeof(int)*(2*DIM + 2 + MAX_CAP*(2*DIM + 1));
  if(num_nodes==1)
  {
    ROOT_ID = start;
    //Get entry with a given node id
    int* child_node = get_entry(ROOT_ID, fh, node_size);
    cout << "id: "<<child_node[0] << endl;
    cout << "parent id: "<<child_node[1+2*DIM] << endl;
    for(int i=0; i<MAX_CAP; i++)
    {
      cout <<"child_id #"<<i<<": "<<child_node[2+4*DIM+i*(1+2*DIM)]<<endl;
    }
    return;
  }

  int id = end;

  for(int i=start; i<end; i+= MAX_CAP)
  {
    //Assign a node
    int node[node_size/sizeof(int)];
    int idx=0;
    //ID
    node[idx] = id;
    idx++;
    //MBR
    while(idx<=DIM) {node[idx] = INT_MAX; idx++;} //1st point
    while(idx<=2*DIM) {node[idx] = INT_MIN; idx++;} //2nd point
    //parent ID
    node[idx] = -1;
    idx++;
    //MAX_CAP number of children
    int k_max = MAX_CAP;
    if(i+k_max >= end) k_max = end-i;
    //if(num_points < MAX_CAP) {k_max=num_points;}
    for(int k=0; k<k_max; k++)
    {
      //Get entry with a given node id
      int* child_node = get_entry(i+k, fh, node_size);
      cout << "i: "<<*child_node<<endl;
      //Assign parent ID for the child
      memcpy(&child_node[1+2*DIM] ,&node[0], sizeof(int));
      //MBR
      memcpy(&node[idx],&child_node[1],DIM*sizeof(int)); //1st point
      idx+=DIM;
      memcpy(&node[idx],&child_node[1+DIM],DIM*sizeof(int)); //2nd point
      idx+=DIM;
      //child id
      node[idx] = i+k;
      idx++;
    }
    //Update parent MBR

    update_MBR(&node[0], k_max);

    if(k_max != MAX_CAP)
    {
      //for the remaining child slots
      for(int k=k_max; k<MAX_CAP; k++)
      {
        //MBR
        for(int ctr=0; ctr<DIM; ctr++) {node[idx] = INT_MIN; idx++;} //1st point
        memcpy(&node[idx], &node[idx-DIM], DIM*sizeof(int));//2nd point
        idx+=DIM;
        //child id
        node[idx] = -1;
        idx++;
      }
    }

    memcpy((char*)get_entry(id,fh, node_size),&node[0],node_size);
    id++; //increment the id for the non-leaf node allottment
  }

  assign_parents(fh, end, id);

}

// point query
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isInMBR(int *Mbr, int *P){
    for(int i=0; i< DIM; i++){
        if(Mbr[i] != INT_MIN && Mbr[i+DIM] != INT_MIN)
        {
          if (Mbr[i] < Mbr[i+DIM]){
              if (!(Mbr[i]<= P[i] <= Mbr[i+DIM]))
                  return false;
          }else{
              if (!(Mbr[i+DIM]<= P[i] <= Mbr[i]))
                  return false;
          }
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
    int start_index = 2+2*DIM;
    for(int i=0; i< MAX_CAP; i++){
        if (Node[2*DIM+start_index] != -1)
            return false;
        start_index = start_index + 1 + 2*DIM;
    }
    return true;
}



bool pointQuery(int *P, int NodeId, char *fileName, FileManager fm){
    cout <<  "Point Query " << NodeId << endl;
    int nodeSize = (((MAX_CAP+1)*(2*DIM+1)+1)*sizeof(int));
    int pageLimit = PAGE_CONTENT_SIZE/nodeSize;
    int pageIndex = (NodeId-1)/pageLimit;
    FileHandler fh = fm.OpenFile(fileName);
    PageHandler ph = fh.PageAt(pageIndex);
    cout << "access page " << pageIndex<< endl;
    char *data = ph.GetData ();
    int Node[((MAX_CAP+1)*(DIM+1)+1)];
    int nodeIndex = (NodeId - pageLimit*pageIndex - 1)*nodeSize;
    memcpy(&Node, &data[nodeIndex], nodeSize);
    bool result = false;
    if(!isLeaf(Node)){
        int startIndex = 2*DIM+2;
        for(int i=0; i<MAX_CAP; i++){
            cout << Node[startIndex+2*DIM] << endl;
            if(isInMBR(&Node[startIndex], P)){
                result = result || pointQuery(P, Node[startIndex+2*DIM], fileName, fm);
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


  string loadfile = "Testcases/TC_1/sortedData_2_10_100.txt"; //read from queries
  string rtree_file = "rtree.txt";
  int loadsize = 100; //read from queries

  FileManager fm;
  str_bulkload(&fm, &loadfile[0], loadsize);
  FileHandler fh;
  fh = fm.OpenFile("rtree.txt");
  PageHandler ph = fh.FirstPage();
  cout << "success" << endl;
  int point[] = {1890802246, 1488456800};
  bool result = pointQuery(point, ROOT_ID, &rtree_file[0], fm);
  cout <<result << endl;
  return 0;
}
