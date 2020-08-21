#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "constants.h"
#include "file_manager.h"
#include "errors.h"
#include <climits>
#include <cstring>
#include <iostream>

using namespace std;

bool isInMBR(int *Mbr, int *P, int dimensionality){
    for(int i=0; i< dimensionality; i++){
        if (Mbr[i] < Mbr[i+dimensionality]){
            if (!(Mbr[i]<= P[i] <= Mbr[i+dimensionality]))
                return false;
        }else{
            if (!(Mbr[i+dimensionality]<= P[i] <= Mbr[i]))
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
bool isLeaf(int *Node, int dimensionality, int maxCap){
    int check = INT_MIN;
    int start_index = 1+dimensionality;
    for(int i = start_index; i++; i< (start_index+dimensionality)){
        if (Node[i] != check)
            return false;
    }
    start_index = 2+2*dimensionality;
    for(int i=0; i< maxCap; i++){
        for(int j=0; j< 2*dimensionality; j++){
            if (Node[j+start_index] != check)
                return false;
        }
        if (Node[2*dimensionality+start_index] != -1)
            return false;
        start_index = start_index + 1 + 2*dimensionality;
    }
    return true;
}



bool pointQuery(int *P, int NodeId, int dimensionality, char *fileName, int maxCap){
    FileManager fm;
    int nodeSize = (((maxCap+1)*(dimensionality+1)+1)*sizeof(int));
    int pageLimit = PAGE_CONTENT_SIZE/nodeSize;
    int pageIndex = NodeId/pageLimit;
    FileHandler fh = fm.OpenFile(fileName);
    PageHandler ph = fh.PageAt(pageIndex);
    char *data = ph.GetData ();
    int Node[((maxCap+1)*(dimensionality+1)+1)];
    int nodeIndex = (NodeId - pageLimit*pageIndex - 1)*nodeSize;
    memcpy(&Node, &data[nodeIndex], nodeSize);
    bool result = false;
    if(!isLeaf(Node, dimensionality, maxCap)){
        int startIndex = 2*dimensionality+2;
        for(int i=0; i<maxCap; i++){
            if(isInMBR(&Node[startIndex], P, dimensionality)){
                result = result || pointQuery(P, Node[startIndex+2*dimensionality], dimensionality, fileName, maxCap);
            }
            startIndex = startIndex + 1 + 2*dimensionality;
            if (result)
                return result;
        }
    }else{
        int startIndex = 1;
        for(int i=0; i< dimensionality; i++){
            if(P[i] != Node[i+startIndex])
                return false;
        }
        return true;
    }
    return result;
}

int main(int argc, char const *argv[]) {
    /* code */
    return 0;
}
