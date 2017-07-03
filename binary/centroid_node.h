#ifndef TRIPLETCACHE_CENTROID_NODE_H
#define TRIPLETCACHE_CENTROID_NODE_H

struct centroid_node {

    //children
    int lpos; //left child
    int rpos; //right child

    //counters
    int size; //size of the subtree rooted at this node
    int id; //id of the node
    int numLeafs; //number of leafs rooted at the subtree of this node
    int minDFSid; //min dfs id stored in the subtree defined by the node
    int maxDFSid; //max dfs id stored in the subtree defined by the node

    bool isLeaf; //true if a leaf, false otherwise
    bool isAlive; //is this node alive
    bool onHP; //was this node marked on the heavy path of an ancestor component

};



#endif //TRIPLETCACHE_CENTROID_NODE_H
