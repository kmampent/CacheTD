#ifndef DTRIPLET_CENTROID_NODE_H
#define DTRIPLET_CENTROID_NODE_H

struct centroid_node{

    //children
    int lpos; //left child
    int rpos; //right child

    //counters
    int size; //size of the subtree rooted at this node (including the green nodes that will be created later in the centroid decomposition)
    int id; //id of the node
    int numLeafs; //number of leafs rooted at the subtree of this node
    int minDFSid; //min dfs id stored in the subtree defined by the node
    int maxDFSid; //max dfs id stored in the subtree defined by the node
    int treeID; //id of the green tree that this node is part of. If the node is black, then the green tree that this node is part of is assumed to be the one where this node
    //is the root
    int topNumLeafs; //topNumLeafs is how many leafs in total are rooted in the top most black node of the tree defined by treeID. We use this to help coloring the leafs green!

    bool isLeaf; //true if a leaf, false otherwise
    bool isAlive; //is this node alive
    bool onHP; //was this node marked on the heavy path of an ancestor component
    bool isGreen; //is the current node green or not

};

#endif //DTRIPLET_CENTROID_NODE_H
