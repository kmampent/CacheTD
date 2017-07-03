#ifndef TRIPLETCACHE_CONTRACT_NODE_H
#define TRIPLETCACHE_CONTRACT_NODE_H

struct contract_node{

    //counters
    bool isLeaf; //is the current node a leaf
    int id; //id of the node
    unsigned long long int s; //total number of leaf pairs that can be generated from all the red subtrees on the edge above the current node
            // , such that the two leafs forming a pair are in the same red subtree

    unsigned long long int st; //total number of red leafs on the edge above the current node

};

//node used in the stack when finding the right component
struct rstNode{
    int pos; //position in contractStack where the node is stored
    bool kill; //true if the node must be contracted, false otherwise
};

//node used in the stack when finding the parent and left component
struct pstNode{
    int pos; //position in contractStack where the node is stored
    bool kill; //true if the node must be contracted, false otherwise
    unsigned long long int s; //total number of leaf pairs that can be generated from all the red subtrees on the edge above the current node
    // , such that the two leafs forming a pair are in the same red subtree
    unsigned long long int st; //total number of red leafs on the edge above the current node

};

//node used in the stack when counting shared triplets
struct cNode{

    unsigned long long int reds;
    unsigned long long int blues;

};

#endif //TRIPLETCACHE_CONTRACT_NODE_H
