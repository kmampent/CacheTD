#ifndef DTRIPLET_COMMON_H
#define DTRIPLET_COMMON_H

#define BILLION  1000000000LL
timespec startT, endT;

void startTimer(){
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startT);
}

double endTimer(){
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endT);
    return endT.tv_sec * BILLION + endT.tv_nsec - (startT.tv_sec * BILLION + startT.tv_nsec);
}

//an array that stores for each label i its dfs id as found in T_{1}
int* dfsIDs;
struct colorInfo {
    int redMin, redMax; //range of dfs ids that correspond to nodes that should have the color red right now
    int blueMin, blueMax;//range of dfs ids that correspond to nodes that should have the color blue right now
    int greenMin, greenMax; //range of dfs ids that correspond to nodes that should have the color green right now
    colorInfo(){
        greenMin = greenMax = -1;
    }
};

colorInfo colors;

#endif //DTRIPLET_COMMON_H
