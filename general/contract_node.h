#ifndef DTRIPLET_CONTRACT_NODE_H
#define DTRIPLET_CONTRACT_NODE_H
#include <vector>
#include <string>
#include <fstream>
using namespace std;
#define ulli unsigned long long int

struct contractedEdgeSameLevel{

    int reds, greens, blacks;
    ulli redGreenPairs;

    string getS(){
        return  to_string(reds) + ", " + to_string(greens) + ", " + to_string(blacks) + ", " + to_string(redGreenPairs);
    }

    void init(){
        reds = greens = blacks = redGreenPairs = 0;
    }

    int getAll(){
        return reds + greens + blacks;
    }
};

struct contractedEdgeAboveLevel{

    int reds, greens, blacks;
    ulli redGreenPairs, redBlackPairs, redGreenAbovePairs;
    string getS(){
        return  to_string(reds) + ", " + to_string(greens) + ", " + to_string(blacks) + ", " + to_string(redGreenPairs) + ", " + to_string(redBlackPairs) + ", " + to_string(redGreenAbovePairs);
    }
    void init(){
        reds = greens = blacks = redGreenPairs = redBlackPairs = redGreenAbovePairs = 0;
    }

    int getAll(){
        return reds + greens + blacks;
    }

};

struct contract_node{

    //pointers to children
    int degree;
    //counters
    int id; //id of the node
    //contracted edge on the same level with the current node
    contractedEdgeSameLevel sameLevelCE;
    contractedEdgeAboveLevel aboveLevelCE;

};

struct plstNode{

    bool kill;
    contractedEdgeSameLevel sameLevelCE;
    contractedEdgeAboveLevel aboveLevelCE;

    void init(){
        kill = false;
        sameLevelCE.init();
        aboveLevelCE.init();
    }

};

struct rstNode{

    bool kill;
    int sameLevelBlacks;
    int aboveLevelBlacks;

    void init(){
        kill = false;
        sameLevelBlacks = aboveLevelBlacks = 0;
    }
};

struct cNode{
    int reds, blacks, blues, greens; //number of red/black/blue/green leafs in the subtree rooted on the current node

    void init(){
        reds = blacks = blues = greens = 0;
    }
};
#endif //DTRIPLET_CONTRACT_NODE_H
