#include <iostream>
#include "newick_parser.h"
#include "triplet_dist.h"
#include <ctime>

using namespace std;

int main(int argc, char* argv[]) {
   
    T1read(argv[1]);
    T2read(argv[2]);

    double tt;
    startTimer();
    init();
    unsigned long long int res = computeTripletDistance();
    tt = endTimer();
    cout<<"Triplet distance: "<<res<<endl;
    cout<<"Execution time: "<<tt/BILLION<<" seconds"<<endl;

    delete[] T1;
    delete[] centroidTree;
    delete[] dfsIDs;
    delete[] T2;
    delete[] contractStack;
    delete[] countingStack;
    delete[] rContractStack;
    delete[] plContractStack;

    return 0;
}
