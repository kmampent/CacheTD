#ifndef DTRIPLET_TRIPLET_DIST_H
#define DTRIPLET_TRIPLET_DIST_H

#include "centroid_component.h"
#include "contract_component.h"

unsigned long long int shared_triplets; //counter accumulating all the shared triplets
unsigned long long int n; //number of leafs

/*
 *
 * Initializing components -------------- START
 *
 *
 */

void init(){
    colors.redMax = colors.redMin = colors.blueMax = colors.blueMin = -1;
    n = T1numLeafs;
    centroidInit();
    contractInit();

}

/*
 *
 * Initializing components -------------- END
 *
 *
 */


/*
 *
 * Finding the triplet distance -------------- START
 *
 *
 */

void computeTripletDistanceHelper(int centroidRoot, int prevCentroidTreeID){

    //find centroid
    centroid = centroidFindCentroid(centroidRoot);
    centroid_node *curCentroidNode = &centroidTree[centroid];

    //color the centroid component
    centroid_node *curCentroidLeftChild = &centroidTree[curCentroidNode->lpos];
    centroid_node *curCentroidRightChild = &centroidTree[curCentroidNode->rpos];
    colorInfo ci;
    ci.redMin = curCentroidLeftChild->minDFSid;
    ci.redMax = curCentroidLeftChild->maxDFSid;
    ci.blueMin = curCentroidRightChild->minDFSid;
    ci.blueMax = curCentroidRightChild->maxDFSid;

    if(curCentroidNode->isGreen){
        ci.greenMin = ci.blueMax + 1;
        ci.greenMax = ci.blueMax + curCentroidNode->topNumLeafs - (ci.redMax - ci.redMin + 1) - (ci.blueMax - ci.blueMin + 1);
    }
    colors = ci;

    centroidTree[centroid].isAlive = false;
    if(curCentroidNode->treeID != prevCentroidTreeID){
        shared_triplets += findSgreensAreBlacks();
    }
    else{
        shared_triplets += findSgreensAreGreens();
    }

    //find the three centroid components

    int centroidLeftComp = centroidGetLeftComp();
    int centroidRightComp = centroidGetRightComp();
    int centroidParentComp = centroidGetParentComp(centroidRoot);

    int cS, cE;
    cS = curStart;
    cE = curEnd;

    if(centroidLeftComp != -1){
        if(curCentroidNode->treeID != prevCentroidTreeID)
            contractFindLeftCompGreensNotMaintained();
        else
            contractFindLeftCompGreensAreMaintained();
        computeTripletDistanceHelper(centroidLeftComp, curCentroidNode->treeID);
        contractStackTop = cE;
    }

    if(centroidRightComp != -1){
        centroidSize = 0;
        colors = ci;
        curStart = cS;
        curEnd = cE;
        contractFindRightComp();
        computeTripletDistanceHelper(centroidRightComp, curCentroidNode->treeID);
        contractStackTop = cE;
    }

    if(centroidParentComp != -1){
        centroidSize = curCentroidNode->size;
        colors = ci;
        curStart = cS;
        curEnd = cE;
        contractFindParentComp();
        computeTripletDistanceHelper(centroidParentComp, prevCentroidTreeID);
        contractStackTop = cE;
    }

}


unsigned long long int computeTripletDistance(){

    shared_triplets = 0;
    centroidSize = 0;
    computeTripletDistanceHelper(0, -1);

    unsigned long long int n1, n2;
    n1 = n-1;
    n2 = n-2;
    if(n%2==0) n = n/2;
    else n1 = n1/2;
    if(n2%3==0) n2 = n2/3;
    else if(n1%3==0) n1 = n1/3;
    else n = n/3;

    return n*n1*n2 - shared_triplets;

}

/*
 *
 * Finding the triplet distance -------------- END
 *
 *
 */

#endif //DTRIPLET_TRIPLET_DIST_H
