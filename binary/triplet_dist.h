#ifndef TRIPLETCACHE_TRIPLET_DIST_H
#define TRIPLETCACHE_TRIPLET_DIST_H

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
    contractStack = new contract_node[T2size*3];
    contractInit();
    countingStack = new cNode[curEnd - curStart];
    rContractStack = new rstNode[curEnd - curStart];
    plContractStack = new pstNode[curEnd - curStart];

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

void computeTripletDistanceHelper(int centroidRoot){

    //find centroid
    centroid = centroidFindCentroid(centroidRoot);
    //color the centroid component
    centroid_node *curCentroidNode = &centroidTree[centroid];
    centroidTree[centroid].isAlive = false;
    centroid_node *curCentroidLeftChild = &centroidTree[curCentroidNode->lpos];
    centroid_node *curCentroidRightChild = &centroidTree[curCentroidNode->rpos];
    colorInfo ci;
    ci.redMin = curCentroidLeftChild->minDFSid;
    ci.redMax = curCentroidLeftChild->maxDFSid;
    ci.blueMin = curCentroidRightChild->minDFSid;
    ci.blueMax = curCentroidRightChild->maxDFSid;

    //find s(centroid, T2)
    colors = ci;
    shared_triplets += findS();

    int centroidLeftComp = centroidGetLeftComp();
    int centroidRightComp = centroidGetRightComp();
    int centroidParentComp = centroidGetParentComp(centroidRoot);

    int cS, cE;
    cS = curStart;
    cE = curEnd;

    if(centroidLeftComp != -1){
        contractFindLeftComp();
        computeTripletDistanceHelper(centroidLeftComp);
        contractStackTop = cE;
    }

    if(centroidRightComp != -1){
        centroidSize = 0;
        colors = ci;
        curStart = cS;
        curEnd = cE;
        contractFindRightComp();
        computeTripletDistanceHelper(centroidRightComp);
        contractStackTop = cE;
    }

    if(centroidParentComp != -1){
        centroidSize = curCentroidNode->size;
        colors = ci;
        curStart = cS;
        curEnd = cE;
        contractFindParentComp();
        computeTripletDistanceHelper(centroidParentComp);
        contractStackTop = cE;
    }

}

unsigned long long int computeTripletDistance(){

    shared_triplets = 0;
    centroidSize = 0;
    computeTripletDistanceHelper(0);

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

#endif //TRIPLETCACHE_TRIPLET_DIST_H
