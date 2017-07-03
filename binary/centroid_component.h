#ifndef TRIPLETCACHE_CENTROID_COMPONENT_H
#define TRIPLETCACHE_CENTROID_COMPONENT_H
#include "newick_parser.h"
#include "centroid_node.h"
#include "common.h"

centroid_node * centroidTree; //tree holding the nodes of the first centroid component
int centroid;
int centroidSize; //size of the subtree rooted on the centroid, used to avoid updating the sizes every time we generate the parent component

/*
 *
 * finding the first centroid component (corresponding to the entire tree)
 *
 */

int index;
int dfsCounter;

void centroidFindSizes(int cur){

    if(T1[cur].numChildren == 0){
        T1[cur].size = 1;
        return;
    }

    int l = T1[cur].lpos;
    int r = T1[cur].rpos;
    centroidFindSizes(l);
    centroidFindSizes(r);

    T1[cur].size = 1 + T1[l].size + T1[r].size;

}

void centroidInitHelper(int curT1){

    centroid_node * curCentroidNode = &centroidTree[index];
    tree_node * curTreeNode = &T1[curT1];

    curCentroidNode->id = curTreeNode->id;
    curCentroidNode->size = curTreeNode->size;
    curCentroidNode->isAlive = true;
    curCentroidNode->onHP = false;
    curCentroidNode->isLeaf = false;
    index++;

    if(curTreeNode->numChildren == 0){
        dfsIDs[curTreeNode->id] = dfsCounter;
        curCentroidNode->numLeafs = 1;
        curCentroidNode->minDFSid = dfsCounter;
        curCentroidNode->maxDFSid = dfsCounter;
        curCentroidNode->isLeaf = true;
        dfsCounter++;
        return;
    }

    int lT1 = curTreeNode->lpos;
    int rT1 = curTreeNode->rpos;

    if(T1[lT1].size < T1[rT1].size){
        int tmp = lT1;
        lT1 = rT1;
        rT1 = tmp;
    }

    //make sure to visit the largest subtree first
    int sz = T1[lT1].size;

    curCentroidNode->lpos = index;
    curCentroidNode->rpos = curCentroidNode->lpos + sz;
    centroid_node * lChild = &centroidTree[curCentroidNode->lpos];
    centroid_node * rChild = &centroidTree[curCentroidNode->rpos];

    centroidInitHelper(lT1);
    centroidInitHelper(rT1);

    curCentroidNode->numLeafs = lChild->numLeafs + rChild->numLeafs;
    curCentroidNode->minDFSid = lChild->minDFSid;
    curCentroidNode->maxDFSid = rChild->maxDFSid;

}

void centroidInit(){

    centroidFindSizes(0);
    //we know how many nodes we are going to need
    centroidTree = new centroid_node[T1size];
    dfsIDs = new int[T1numLeafs];
    index = 0;
    dfsCounter = 0;

    centroidInitHelper(0);

}


/*
 *
 *
 * FINDING THE CENTROID ------------ START
 *
 *
 */

/*
 *
 *
 * function that finds the centroid according to the algorithm
 *
 */
int centroidFindCentroid(int curCentroidRoot) {

    int i;
    int curSize = centroidTree[curCentroidRoot].size;
    int threshold = curSize + centroidSize;
    i = curCentroidRoot;
    while(centroidTree[i].size*2>= threshold){
        centroidTree[i].onHP = true;
        i++;
    }
    return i-1;

}

/*
 *
 *
 * FINDING THE CENTROID ------------ END
 *
 *
 */


/*
 *
 *
 * GETTING NEW COMPONENTS ------------ START
 *
 *
 */

/*
 *
 * returning the left component (the right is always good, by left we mean the one that can be both good and bad)
 *
 *
 */
int centroidGetLeftComp() {

    int centroidLeftChild = centroidTree[centroid].lpos;
    centroid_node* leftChild = &centroidTree[centroidLeftChild];
    bool curType = !leftChild->onHP; //type of the current component

    if(!leftChild->isAlive || (curType == true && leftChild->numLeafs < 3)) return -1;

    return centroidLeftChild;

}

/*
 *
 *
 * returning the right component, which is always good by definition
 *
 */

int centroidGetRightComp() {

    int centroidRightChild = centroidTree[centroid].rpos;
    centroid_node* rightChild = &centroidTree[centroidRightChild];

    if(rightChild->numLeafs < 3 || !rightChild->isAlive) return -1;

    return centroidRightChild;

}

/*
 *
 * returning the parent component
 *
 */
int centroidGetParentComp(int curCentroidRoot) {

    if(centroid == curCentroidRoot) return -1;

    return curCentroidRoot;

}

/*
 *
 *
 * GETTING NEW COMPONENTS ------------ END
 *
 *
 */

#endif //TRIPLETCACHE_CENTROID_COMPONENT_H
