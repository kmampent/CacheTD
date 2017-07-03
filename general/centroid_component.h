#ifndef DTRIPLET_CENTROID_COMPONENT_H
#define DTRIPLET_CENTROID_COMPONENT_H
#include "centroid_node.h"
#include "common.h"

centroid_node* centroidTree; //tree holding the nodes of the first centroid component
int centroid;
int centroidSize;//size of the subtree rooted on the centroid, used to avoid updating the sizes every time we generate the parent component

/*
 *
 * finding the first centroid component (corresponding to the entire tree)
 *
 */


int index;
int dfsCounter;
int treeCounter;
int greenID; //ID for the green nodes, used in the init phase of the centroid/contract components

//function that finds for every node u in T1 the size of the subtree rooted on u

int centroidFindSizes(int cur){

    int numChildren = T1[cur].children.size();
    if(numChildren == 0){
        T1[cur].size = 1;
        return 1;
    }
    int totChildrenSize = numChildren - 2; //numChildren - 2 = number of green nodes

    for(int i = 0; i < numChildren; i++){
        totChildrenSize += centroidFindSizes(T1[cur].children[i]);
    }

    T1[cur].size = 1 + totChildrenSize;
    return T1[cur].size;

}

void centroidInitHelper(int curT1){

    //we are at a new node in T1, create it in the centroid decomposition and update all the necessary variables
    centroid_node * curCentroidNode = &centroidTree[index];
    tree_node * curTreeNode = &T1[curT1];
    int curT1numChildren = curTreeNode->children.size();
    curCentroidNode->id = curTreeNode->id;
    curCentroidNode->size = curTreeNode->size;
    curCentroidNode->isAlive = true;
    curCentroidNode->onHP = false;
    curCentroidNode->isLeaf = false;
    curCentroidNode->isGreen = false;
    curCentroidNode->treeID = ++treeCounter;
    index++;

    if(curT1numChildren == 0){
        //new node is a leaf
        dfsIDs[curTreeNode->id] = dfsCounter;
        curCentroidNode->numLeafs = 1;
        curCentroidNode->minDFSid = dfsCounter;
        curCentroidNode->maxDFSid = dfsCounter;
        curCentroidNode->isLeaf = true;
        curCentroidNode->topNumLeafs = 1;
        dfsCounter++;
        return;

    }

    /*
     *
     * new node is not a leaf but internal node. We need to create curT1numChildren - 2 green nodes, and we also need to ensure that the first subtree expanded is the one corresponding
     * to the largest subtree (including the green nodes) defined by a child of curT1.
     *
     */

    int maxChildIndex = 0;
    int i;
    for(i=1;i<curT1numChildren;i++)
        if(T1[curTreeNode->children[i]].size > T1[curTreeNode->children[maxChildIndex]].size)
            maxChildIndex = i;

    /*
     *
     * update the left/right child positions of the green nodes + black root node
     *
     */

    int prevIndex = index;
    int lastGreenPos = index + curT1numChildren - 3;
    int j = 0; //to iterate over the children of curT1

    //node is binary so green nodes don't exist
    if(lastGreenPos < index){
        curCentroidNode->lpos = index;
        curCentroidNode->rpos = curCentroidNode->lpos + T1[curTreeNode->children[maxChildIndex]].size;
    }
    else {
        //node is not binary, green nodes exist
        centroid_node *cn = &centroidTree[lastGreenPos];
        cn->id = greenID++;
        cn->isAlive = true;
        cn->onHP = false;
        cn->isLeaf = false;
        cn->isGreen = true;
        cn->treeID = treeCounter;
        cn->lpos = lastGreenPos + 1;
        cn->rpos = cn->lpos + T1[curTreeNode->children[maxChildIndex]].size;
        if (j == maxChildIndex) j++;
        cn->size = 1 + T1[curTreeNode->children[maxChildIndex]].size + T1[curTreeNode->children[j]].size;
        for (i = lastGreenPos - 1; i >= index; i--) {
            cn = &centroidTree[i];
            cn->id = greenID++;
            cn->isAlive = true;
            cn->onHP = false;
            cn->isLeaf = false;
            cn->isGreen = true;
            cn->treeID = treeCounter;
            cn->lpos = i + 1;
            j++;
            if (j == maxChildIndex) j++;
            cn->rpos = cn->lpos + centroidTree[i + 1].size;
            cn->size = 1 + centroidTree[i + 1].size + T1[curTreeNode->children[j]].size;
        }
        //update the curCentroidNode;
        curCentroidNode->lpos = i + 1;
        j++;
        if (j == maxChildIndex) j++;
        curCentroidNode->rpos = curCentroidNode->lpos + centroidTree[i + 1].size;
        curCentroidNode->size = 1 + centroidTree[i + 1].size + T1[curTreeNode->children[j]].size;
    }

    index = lastGreenPos+1;

    /*
     * recurse to the subtrees
    */

    //first largest subtree
    centroidInitHelper(curTreeNode->children[maxChildIndex]);

    //then the rest (order determined already by how we found the pointers above)
    j = 0;
    while(j < curT1numChildren){
        if(j == maxChildIndex){
            j++;
            continue;
        }
        centroidInitHelper(curTreeNode->children[j]);
        j++;
    }

    //find numleafs and dfs ranges for every node
    centroid_node * cn;
    int numLeafs = (curCentroidNode->size + 1)/2;
    for(i=lastGreenPos; i>=prevIndex-1; i--){
        cn = &centroidTree[i];
        cn->numLeafs = centroidTree[cn->lpos].numLeafs + centroidTree[cn->rpos].numLeafs;
        cn->minDFSid = centroidTree[cn->lpos].minDFSid;
        cn->maxDFSid = centroidTree[cn->rpos].maxDFSid;
        cn->topNumLeafs = numLeafs;
    }

}

void centroidInit(){

    centroidFindSizes(0);
    //we know how many nodes we are going to need
    centroidTree = new centroid_node[T1[0].size];
    dfsIDs = new int[T1numLeafs];
    index = 0;
    dfsCounter = 0;
    treeCounter = 0;
    greenID = T1size;
    centroidInitHelper(0);

}


/*
 *
 *
 * FINDING THE CENTROID ------------ START
 *
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
 * returning the left component (the right is always good, by left we mean the one that can be both good and bad)
 * note that unlike in binary trees, the base case here for case of a good component is 2 since we might have two leafs rooted on a green parent node!
 *
 */

int centroidGetLeftComp() {

    int centroidLeftChild = centroidTree[centroid].lpos;
    centroid_node* leftChild = &centroidTree[centroidLeftChild];
    bool curType = !leftChild->onHP; //type of the current component

    if(!leftChild->isAlive || (curType == true && leftChild->numLeafs < 2)) return -1;

    return centroidLeftChild;

}


/*
 *
 *
 * returning the right component, which is always good by definition
 *
 * note that the case is the same as in binary trees!
 *
 */

int centroidGetRightComp() {

    int centroidRightChild = centroidTree[centroid].rpos;
    centroid_node* rightChild = &centroidTree[centroidRightChild];

    if(rightChild->numLeafs < 2 || !rightChild->isAlive) return -1;

    return centroidRightChild;

}


/*
 *
 * returning the parent component
 *
 * note that the case is the same as in binary trees!
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


#endif //DTRIPLET_CENTROID_COMPONENT_H
