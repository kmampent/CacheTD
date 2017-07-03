#ifndef DTRIPLET_CONTRACT_COMPONENT_H
#define DTRIPLET_CONTRACT_COMPONENT_H
#include "contract_node.h"
#include "common.h"
#include "tree_component.h"

contract_node* contractStack; //stack that will hold all the components
rstNode *rContractStack;
plstNode *plContractStack;
cNode* countingStack;
int contractStackTop; //pointer to the first free element of the stack
int curStart, curEnd; //start and end pointers of the current component

/*
 *
 *
 * Finding first contract component ------------ START
 *
 *
 */

void contractInit(){

    contractStack = new contract_node[T2size*3];
    rContractStack = new rstNode[T2size];
    countingStack = new cNode[T2size];
    plContractStack = new plstNode[T2size];

    int i;
    for(i=0;i<T2size;i++){
        contract_node* nn = &contractStack[i];
        tree_node *tn = &T2[i];
        nn->degree = tn->children.size();
        if(nn->degree == 0) nn->id = dfsIDs[tn->id];
        else {
            nn->id = tn->id;
        }
        nn->aboveLevelCE.init();
        nn->sameLevelCE.init();
    }

    curStart = 0;
    curEnd = contractStackTop = T2size;

}

/*
 *
 *
 * Finding first contract component ------------ END
 *
 *
 */
bool isBlue(int id){

    return (id >= colors.blueMin && id <= colors.blueMax);

}


bool isRed(int id){

    return (id >= colors.redMin && id <= colors.redMax);

}

bool isGreen(int id){

    return (id >= colors.greenMin && id <= colors.greenMax);

}

bool hasColor(int id){

    return (id >= colors.blueMin && id <= colors.blueMax) || (id >= colors.redMin && id <= colors.redMax) || (id >= colors.greenMin && id <= colors.greenMax);

}

/*
 *
 *
 * FINDING SHARED TRIPLETS ------------ START
 *
 *
 */

ulli findSgreensAreGreens(){

    int stTop = 0;
    int i;
    unsigned long long int curShared = 0;
    int allBlacks = T2numLeafs;

    if(colors.redMin != -1)
        allBlacks -= colors.redMax - colors.redMin + 1;
    if(colors.blueMin != -1)
        allBlacks -= colors.blueMax - colors.blueMin + 1;
    if(colors.greenMin != -1)
        allBlacks -= colors.greenMax - colors.greenMin + 1;

    for(i=curStart;i<curEnd;i++){

        contract_node * cn = &contractStack[i];

        if(cn->degree == 0){

            cNode *sn = &countingStack[stTop++];
            sn->init();
            if(isRed(cn->id)){
                sn->reds = 1 + cn->aboveLevelCE.reds;
                sn->greens = cn->aboveLevelCE.greens;
                sn->blacks = cn->aboveLevelCE.blacks;
            }
            else if(isBlue(cn->id)){
                sn->blues = 1;
                sn->reds = cn->aboveLevelCE.reds;
                sn->greens = cn->aboveLevelCE.greens;
                sn->blacks = cn->aboveLevelCE.blacks;
                curShared += cn->aboveLevelCE.redBlackPairs + (ulli)cn->aboveLevelCE.reds*(allBlacks - cn->aboveLevelCE.blacks); //because you can use the blue leaf to form resolved triplet
                curShared += cn->aboveLevelCE.redGreenPairs;
            }
            else if(isGreen(cn->id)){
                sn->reds = cn->aboveLevelCE.reds;
                sn->greens = 1 + cn->aboveLevelCE.greens;
                sn->blacks = cn->aboveLevelCE.blacks;
            }
            else{
                sn->reds = cn->aboveLevelCE.reds;
                sn->greens = cn->aboveLevelCE.greens;
                sn->blacks = 1+cn->aboveLevelCE.blacks;
            }

        }
        else{

            ulli redBluePair, redGreenPair, blueGreenPair, redBlueGreenTriplet;
            int blacksFromChildren = cn->sameLevelCE.blacks;
            int redsFromChildren = cn->sameLevelCE.reds;
            int bluesFromChildren = 0;
            int greensFromChildren = cn->sameLevelCE.greens;
            redBluePair = blueGreenPair = redBlueGreenTriplet = 0;
            redGreenPair = cn->sameLevelCE.redGreenPairs;

            int numChildren = cn->degree;
            for(int c=0;c<numChildren;c++){
                cNode *child = &countingStack[stTop-c-1];
                redBlueGreenTriplet += redBluePair*child->greens + redGreenPair*child->blues + blueGreenPair*child->reds;
                redBluePair += (ulli)redsFromChildren*child->blues + (ulli)bluesFromChildren*child->reds;
                redGreenPair += (ulli)redsFromChildren*child->greens + (ulli)greensFromChildren*child->reds;
                blueGreenPair += (ulli)bluesFromChildren*child->greens + (ulli)greensFromChildren*child->blues;
                blacksFromChildren += child->blacks;
                bluesFromChildren += child->blues;
                redsFromChildren += child->reds;
                greensFromChildren += child->greens;
            }

            //update counters

            //unresolved

            curShared += redBlueGreenTriplet + bluesFromChildren*(cn->aboveLevelCE.redGreenPairs);

            //resolved

            curShared += redBluePair*(allBlacks - blacksFromChildren) + cn->aboveLevelCE.redBlackPairs*(bluesFromChildren) + (ulli)bluesFromChildren*cn->aboveLevelCE.reds*(allBlacks - blacksFromChildren - cn->aboveLevelCE.blacks);
            //update stack

            stTop-=numChildren-1;

            cNode *sn = &countingStack[stTop-1];

            sn->blacks = blacksFromChildren + cn->aboveLevelCE.blacks;
            sn->reds = redsFromChildren + cn->aboveLevelCE.reds;
            sn->greens = greensFromChildren + cn->aboveLevelCE.greens;
            sn->blues = bluesFromChildren;

        }

    }

    return curShared;

}

ulli findSgreensAreBlacks(){

    int stTop = 0;
    int i;
    unsigned long long int curShared = 0;
    int allBlacks = T2numLeafs;

    if(colors.redMin != -1)
        allBlacks -= colors.redMax - colors.redMin + 1;
    if(colors.blueMin != -1)
        allBlacks -= colors.blueMax - colors.blueMin + 1;
    if(colors.greenMin != -1)
        allBlacks -= colors.greenMax - colors.greenMin + 1;

    for(i=curStart;i<curEnd;i++){

        contract_node * cn = &contractStack[i];

        if(cn->degree == 0){

            cNode *sn = &countingStack[stTop++];
            sn->init();
            if(isRed(cn->id)){
                sn->reds = 1 + cn->aboveLevelCE.reds;
                sn->greens = 0;
                sn->blacks = cn->aboveLevelCE.blacks + cn->aboveLevelCE.greens;
            }
            else if(isBlue(cn->id)){
                sn->blues = 1;
                sn->reds = cn->aboveLevelCE.reds;
                sn->greens = 0;
                sn->blacks = cn->aboveLevelCE.blacks + cn->aboveLevelCE.greens;
                curShared += cn->aboveLevelCE.redBlackPairs + cn->aboveLevelCE.redGreenAbovePairs + (ulli)cn->aboveLevelCE.reds*(allBlacks - cn->aboveLevelCE.blacks - cn->aboveLevelCE.greens); //because you can use the blue leaf to form resolved triplet
            }
            else if(isGreen(cn->id)){
                sn->reds = cn->aboveLevelCE.reds;
                sn->greens = 1;
                sn->blacks = cn->aboveLevelCE.blacks + cn->aboveLevelCE.greens;
            }
            else{
                sn->reds = cn->aboveLevelCE.reds;
                sn->greens = 0;
                sn->blacks = 1+cn->aboveLevelCE.blacks+cn->aboveLevelCE.greens;
            }

        }
        else{

            ulli redBluePair, redGreenPair, blueGreenPair, redBlueGreenTriplet;
            int blacksFromChildren = cn->sameLevelCE.blacks + cn->sameLevelCE.greens;
            int redsFromChildren = cn->sameLevelCE.reds;
            int bluesFromChildren = 0;
            int greensFromChildren = 0;
            redBluePair = blueGreenPair = redBlueGreenTriplet = 0;
            redGreenPair = 0;

            int numChildren = cn->degree;
            for(int c=0;c<numChildren;c++){
                cNode *child = &countingStack[stTop-c-1];
                redBlueGreenTriplet += redBluePair*child->greens + redGreenPair*child->blues + blueGreenPair*child->reds;
                redBluePair += (ulli)redsFromChildren*child->blues + (ulli)bluesFromChildren*child->reds;
                redGreenPair += (ulli)redsFromChildren*child->greens + (ulli)greensFromChildren*child->reds;
                blueGreenPair += (ulli)bluesFromChildren*child->greens + (ulli)greensFromChildren*child->blues;
                blacksFromChildren += child->blacks;
                bluesFromChildren += child->blues;
                redsFromChildren += child->reds;
                greensFromChildren += child->greens;
            }

            //update counters

            //unresolved

            curShared += redBlueGreenTriplet;

            //resolved

            curShared += redBluePair*(allBlacks - blacksFromChildren) + (cn->aboveLevelCE.redGreenAbovePairs + cn->aboveLevelCE.redBlackPairs)*(bluesFromChildren) + (ulli)bluesFromChildren*cn->aboveLevelCE.reds*(allBlacks - blacksFromChildren - cn->aboveLevelCE.blacks - cn->aboveLevelCE.greens);
            //update stack

            stTop-=numChildren-1;

            cNode *sn = &countingStack[stTop-1];

            sn->blacks = blacksFromChildren + cn->aboveLevelCE.blacks + cn->aboveLevelCE.greens;
            sn->reds = redsFromChildren + cn->aboveLevelCE.reds;
            sn->greens = greensFromChildren;
            sn->blues = bluesFromChildren;

        }

    }

    return curShared;

}

/*
 *
 *
 * FINDING SHARED TRIPLETS ------------ END
 *
 *
 */


/*
 *
 *
 * FINDING COMPONENTS ------------ START
 *
 *
 */

void contractFindRightComp() {

    int stTop = 0;
    int curNewStart = contractStackTop;
    int i;
    for(i=curStart;i<curEnd;i++){
        contract_node *cn = &contractStack[i];

        if(cn->degree == 0){

            //the current node is a leaf, so we must see whether we want to keep it or not.
            if(isBlue(cn->id)){
                /*
                 *
                 * pushing to stack
                 * we need to be careful that red leafs and green leafs that are potentially stored on the above level edge of this leaf
                 * must now become black leafs.
                 *
                 */
                //we want to keep the leaf so we give it to the output
                rstNode *sn = &rContractStack[stTop];
                sn->kill = false;
                //everything that exists in the above level contracted edge of this leaf will become black
                sn->aboveLevelBlacks = cn->aboveLevelCE.getAll();
                sn->sameLevelBlacks = 0;
                stTop++;

                /*
                 *
                 * give to the output
                 *
                 *
                 */
                contract_node *nn;
                nn = &contractStack[contractStackTop++];
                nn->degree = 0;
                nn->id = cn->id;
                nn->aboveLevelCE.init();
                nn->aboveLevelCE.blacks = sn->aboveLevelBlacks;
                nn->sameLevelCE.init();

            }
            else{

                //we do not want to keep the leaf, so we do not send it to the output, but we push it to the stack
                rstNode *sn = &rContractStack[stTop];
                sn->kill = true;
                //by default send everything to the same Level variable....
                sn->sameLevelBlacks = cn->aboveLevelCE.getAll() + 1; //plus 1 since the leaf will become black.
                stTop++;

            }

        }
        else{

            //we are at an internal node, we need to determine if we are going to kill this node or not. We only kill the node defined by cn IF d-1 of its children are
            //dead where d is the degree of the node.

            int numDeadChildren = 0;
            int numChildren = cn->degree;
            ulli blackFromDeadChildren = cn->sameLevelCE.getAll();
            rstNode *aliveChild = nullptr;
            for(int c=0;c<numChildren;c++){
                rstNode *child = &rContractStack[stTop-c-1];
                if(child->kill){
                    numDeadChildren++;
                    blackFromDeadChildren += child->sameLevelBlacks;
                }
                else{
                    aliveChild = child;
                }
            }

            if(numDeadChildren < numChildren - 1){
                //we keep this internal node, the number of dead black leafs go to current level of cn
                //print to output
                contract_node *nn = &contractStack[contractStackTop];
                nn->aboveLevelCE.init();
                nn->aboveLevelCE.blacks = cn->aboveLevelCE.getAll();
                nn->sameLevelCE.init();
                nn->sameLevelCE.blacks = blackFromDeadChildren;
                nn->id = cn->id;
                nn->degree = numChildren-numDeadChildren;
                contractStackTop++;

                //push to stack
                stTop-=numChildren-1;
                rstNode *newNode = &rContractStack[stTop-1];
                newNode->sameLevelBlacks = nn->sameLevelCE.blacks;
                newNode->aboveLevelBlacks = nn->aboveLevelCE.blacks;
                newNode->kill = false;

            }
            else{
                //we do not keep this internal node
                if(numDeadChildren == numChildren - 1){
                    //there is one alive child, the contracted edge of which we will need to update
                    /*everything from the dead children will go to above level blacks of the alive child!
                      but also what the cn node already has in the SAME level AND above level*/
                    int tmp = aliveChild->aboveLevelBlacks;
                    int tmp2 = aliveChild->sameLevelBlacks;
                    stTop-=numChildren-1;
                    rstNode *newNode = &rContractStack[stTop-1];
                    newNode->kill = false;
                    newNode->aboveLevelBlacks = tmp + blackFromDeadChildren + cn->aboveLevelCE.getAll();
                    newNode->sameLevelBlacks = tmp2;
                    contractStack[contractStackTop-1].aboveLevelCE.blacks = newNode->aboveLevelBlacks;
                    contractStack[contractStackTop-1].sameLevelCE.blacks = tmp2;
                }
                else{
                    //there is no alive child, so we have to  remove the entire subtree defined by the node cn
                    //remove all children from stack
                    stTop-=numChildren-1;
                    rstNode *newNode = &rContractStack[stTop-1];
                    newNode->sameLevelBlacks = blackFromDeadChildren+ cn->aboveLevelCE.getAll();
                    newNode->kill = true;
                }
            }
        }
    }

    curStart = curNewStart;
    curEnd = contractStackTop;

}

void contractFindParentComp() {

    int stTop = 0;
    int curNewStart = contractStackTop;
    int i;
    for(i=curStart;i<curEnd;i++){
        contract_node *cn = &contractStack[i];

        if(cn->degree == 0){

            //the current node is a leaf, so we must see whether we want to keep it or not.
            if(!(isRed(cn->id) || isBlue(cn->id))){
                //the leaf is neither red nor blue, so we must keep it
                /*
                 *
                 * pushing to stack
                 *
                 */
                //we want to keep the leaf so we give it to the output
                plstNode *sn = &plContractStack[stTop];
                sn->kill = false;
                sn->aboveLevelCE = cn->aboveLevelCE;
                sn->sameLevelCE = cn->sameLevelCE;
                stTop++;

                /*
                 *
                 * give to the output
                 *
                 *
                 */
                contract_node *nn;
                nn = &contractStack[contractStackTop++];
                nn->degree = 0;
                nn->id = cn->id;
                nn->aboveLevelCE = cn->aboveLevelCE;
                nn->sameLevelCE = cn->sameLevelCE;

            }
            else{

                //we do not want to keep the leaf, so we do not send it to the output, but we push it to the stack
                plstNode *sn = &plContractStack[stTop];
                sn->kill = true;
                //by default send everything to the same Level variable....
                sn->sameLevelCE.blacks = cn->sameLevelCE.blacks + cn->aboveLevelCE.blacks;
                sn->sameLevelCE.reds = cn->sameLevelCE.reds + cn->aboveLevelCE.reds + 1;
                sn->sameLevelCE.greens = cn->sameLevelCE.greens + cn->aboveLevelCE.greens;
                stTop++;
            }
        }
        else{

            //we are at an internal node, we need to determine if we are going to kill this node or not. We only kill the node defined by cn IF d-1 of its children are
            //dead where d is the degree of the node.

            int numDeadChildren = 0;
            int numChildren = cn->degree;
            int redsFromDeadChildren = cn->sameLevelCE.reds;
            int blacksFromDeadChildren = cn->sameLevelCE.blacks;
            int greensFromDeadChildren = cn->sameLevelCE.greens;
            ulli redGreenPairsFromDeadChildren = cn->sameLevelCE.redGreenPairs;
            plstNode *aliveChild = nullptr;
            for(int c=0;c<numChildren;c++){
                plstNode *child = &plContractStack[stTop-c-1];
                if(child->kill){
                    numDeadChildren++;
                    redGreenPairsFromDeadChildren = redGreenPairsFromDeadChildren + (ulli)redsFromDeadChildren*child->sameLevelCE.greens + (ulli)greensFromDeadChildren*child->sameLevelCE.reds;
                    redsFromDeadChildren += child->sameLevelCE.reds;
                    blacksFromDeadChildren += child->sameLevelCE.blacks;
                    greensFromDeadChildren += child->sameLevelCE.greens;
                }
                else{
                    aliveChild = child;
                }
            }

            if(numDeadChildren < numChildren - 1){
                //we keep this internal node, the number of dead black leafs go to current level of cn
                //print to output
                contract_node *nn = &contractStack[contractStackTop];
                nn->aboveLevelCE = cn->aboveLevelCE;
                nn->sameLevelCE.reds = redsFromDeadChildren;
                nn->sameLevelCE.blacks = blacksFromDeadChildren;
                nn->sameLevelCE.greens = greensFromDeadChildren;
                nn->sameLevelCE.redGreenPairs = redGreenPairsFromDeadChildren;
                nn->id = cn->id;
                nn->degree = numChildren-numDeadChildren;
                contractStackTop++;

                //push to stack
                stTop-=numChildren-1;
                plstNode *sn = &plContractStack[stTop-1];
                sn->sameLevelCE = nn->sameLevelCE;
                sn->aboveLevelCE = nn->aboveLevelCE;
                sn->kill = false;
            }
            else{
                //we do not keep this internal node
                if(numDeadChildren == numChildren - 1){
                    //there is one alive child, the contracted edge of which we will need to update
                    /*everything from the dead children will go to above level blacks of the alive child!
                      but also what the cn node already has in the SAME level AND above level*/
                    contractedEdgeAboveLevel tmp = aliveChild->aboveLevelCE;
                    contractedEdgeSameLevel tmp2 = aliveChild->sameLevelCE;
                    stTop-=numChildren-1;
                    plstNode *sn = &plContractStack[stTop-1];
                    sn->kill = false;
                    sn->aboveLevelCE = tmp;
                    sn->sameLevelCE = tmp2;
                    sn->aboveLevelCE.redGreenPairs += redGreenPairsFromDeadChildren + cn->aboveLevelCE.redGreenPairs;
                    sn->aboveLevelCE.redBlackPairs += (ulli)sn->aboveLevelCE.reds*(blacksFromDeadChildren + cn->aboveLevelCE.blacks) + cn->aboveLevelCE.redBlackPairs + (ulli)(redsFromDeadChildren)*cn->aboveLevelCE.blacks;
                    sn->aboveLevelCE.redGreenAbovePairs += (ulli)sn->aboveLevelCE.reds*(greensFromDeadChildren + cn->aboveLevelCE.greens) + cn->aboveLevelCE.redGreenAbovePairs + (ulli)(redsFromDeadChildren)*cn->aboveLevelCE.greens;
                    sn->aboveLevelCE.reds += redsFromDeadChildren + cn->aboveLevelCE.reds;
                    sn->aboveLevelCE.blacks += blacksFromDeadChildren + cn->aboveLevelCE.blacks;
                    sn->aboveLevelCE.greens += greensFromDeadChildren + cn->aboveLevelCE.greens;
                    contractStack[contractStackTop-1].aboveLevelCE = sn->aboveLevelCE;
                    contractStack[contractStackTop-1].sameLevelCE = sn->sameLevelCE;
                }
                else{
                    //there is no alive child, so we have to  remove the entire subtree defined by the node cn
                    //remove all children from stack
                    stTop-=numChildren-1;
                    plstNode *sn = &plContractStack[stTop-1];
                    sn->kill = true;
                    //by default send everything to the same Level variable....
                    sn->sameLevelCE.blacks = blacksFromDeadChildren + cn->aboveLevelCE.blacks;
                    sn->sameLevelCE.reds = redsFromDeadChildren + cn->aboveLevelCE.reds;
                    sn->sameLevelCE.greens = greensFromDeadChildren + cn->aboveLevelCE.greens;
                }
            }
        }
    }

    curStart = curNewStart;
    curEnd = contractStackTop;

}


void contractFindLeftCompGreensNotMaintained() {

    int stTop = 0;
    int curNewStart = contractStackTop;
    int i;
    for(i=curStart;i<curEnd;i++){
        contract_node *cn = &contractStack[i];

        if(cn->degree == 0){

            //the current node is a leaf, so we must see whether we want to keep it or not.
            if(isRed(cn->id)){
                //the leaf is red so we keep it
                /*
                 *
                 * pushing to stack
                 *
                 */
                //we want to keep the leaf so we give it to the output
                plstNode *sn = &plContractStack[stTop];
                sn->kill = false;
                sn->aboveLevelCE = cn->aboveLevelCE;
                sn->aboveLevelCE.blacks += sn->aboveLevelCE.greens;
                sn->aboveLevelCE.greens = 0;
                sn->aboveLevelCE.redGreenPairs = 0;
                sn->aboveLevelCE.redBlackPairs += sn->aboveLevelCE.redGreenAbovePairs;
                sn->aboveLevelCE.redGreenAbovePairs = 0;
                sn->sameLevelCE = cn->sameLevelCE;
                sn->sameLevelCE.blacks += sn->sameLevelCE.greens;
                sn->sameLevelCE.greens = 0;
                sn->sameLevelCE.redGreenPairs = 0;
                stTop++;

                /*
                 *
                 * give to the output
                 *
                 *
                 */
                contract_node *nn;
                nn = &contractStack[contractStackTop++];
                nn->degree = 0;
                nn->id = cn->id;
                nn->aboveLevelCE = sn->aboveLevelCE;
                nn->sameLevelCE = sn->sameLevelCE;

            }
            else if(isGreen(cn->id) || isBlue(cn->id)){

                //the leaf is green or blue so it stored as green
                plstNode *sn = &plContractStack[stTop];
                sn->kill = true;
                //by default send everything to the same Level variable....
                sn->sameLevelCE.blacks = cn->sameLevelCE.blacks + cn->aboveLevelCE.blacks + cn->sameLevelCE.greens + cn->aboveLevelCE.greens;
                sn->sameLevelCE.reds = cn->sameLevelCE.reds + cn->aboveLevelCE.reds;
                sn->sameLevelCE.greens = 1;
                stTop++;

            }
            else{

                //the leaf is black so it is stored as black
                plstNode *sn = &plContractStack[stTop];
                sn->kill = true;
                //by default send everything to the same Level variable....
                sn->sameLevelCE.blacks = cn->sameLevelCE.blacks + cn->aboveLevelCE.blacks + cn->sameLevelCE.greens + cn->aboveLevelCE.greens + 1;
                sn->sameLevelCE.reds = cn->sameLevelCE.reds + cn->aboveLevelCE.reds;
                sn->sameLevelCE.greens = 0;
                stTop++;

            }

        }
        else{

            //we are at an internal node, we need to determine if we are going to kill this node or not. We only kill the node defined by cn IF d-1 of its children are
            //dead where d is the degree of the node.

            int numDeadChildren = 0;
            int numChildren = cn->degree;
            int redsFromDeadChildren = cn->sameLevelCE.reds;
            int blacksFromDeadChildren = cn->sameLevelCE.blacks + cn->sameLevelCE.greens;
            int greensFromDeadChildren = 0;
            ulli redGreenPairsFromDeadChildren = 0;
            plstNode *aliveChild = nullptr;
            for(int c=0;c<numChildren;c++){
                plstNode *child = &plContractStack[stTop-c-1];
                if(child->kill){
                    numDeadChildren++;
                    redGreenPairsFromDeadChildren = redGreenPairsFromDeadChildren + (ulli)redsFromDeadChildren*child->sameLevelCE.greens + (ulli)greensFromDeadChildren*child->sameLevelCE.reds;
                    redsFromDeadChildren += child->sameLevelCE.reds;
                    blacksFromDeadChildren += child->sameLevelCE.blacks;
                    greensFromDeadChildren += child->sameLevelCE.greens;
                }
                else{
                    aliveChild = child;
                }
            }

            if(numDeadChildren < numChildren - 1){
                //we keep this internal node, the number of dead black leafs go to current level of cn
                //print to output
                contract_node *nn = &contractStack[contractStackTop];
                nn->aboveLevelCE = cn->aboveLevelCE;
                nn->aboveLevelCE.blacks += nn->aboveLevelCE.greens;
                nn->aboveLevelCE.greens = 0;
                nn->aboveLevelCE.redGreenPairs = 0;
                nn->aboveLevelCE.redBlackPairs += nn->aboveLevelCE.redGreenAbovePairs;
                nn->aboveLevelCE.redGreenAbovePairs = 0;
                nn->sameLevelCE.reds = redsFromDeadChildren;
                nn->sameLevelCE.blacks = blacksFromDeadChildren;
                nn->sameLevelCE.greens = greensFromDeadChildren;
                nn->sameLevelCE.redGreenPairs = redGreenPairsFromDeadChildren;
                nn->id = cn->id;
                nn->degree = numChildren-numDeadChildren;
                contractStackTop++;

                //push to stack
                stTop-=numChildren-1;
                plstNode *sn = &plContractStack[stTop-1];
                sn->sameLevelCE = nn->sameLevelCE;
                sn->aboveLevelCE = nn->aboveLevelCE;
                sn->kill = false;

            }
            else{
                //we do not keep this internal node
                if(numDeadChildren == numChildren - 1){
                    //there is one alive child, the contracted edge of which we will need to update
                    /*everything from the dead children will go to above level blacks of the alive child!
                      but also what the cn node already has in the SAME level AND above level*/
                    contractedEdgeAboveLevel tmp = aliveChild->aboveLevelCE;
                    contractedEdgeSameLevel tmp2 = aliveChild->sameLevelCE;
                    stTop-=numChildren-1;
                    plstNode *sn = &plContractStack[stTop-1];
                    sn->kill = false;
                    sn->aboveLevelCE = tmp;
                    sn->sameLevelCE = tmp2;
                    sn->aboveLevelCE.redGreenPairs += redGreenPairsFromDeadChildren;
                    sn->aboveLevelCE.redBlackPairs += (ulli)sn->aboveLevelCE.reds*(blacksFromDeadChildren + cn->aboveLevelCE.blacks + cn->aboveLevelCE.greens) + cn->aboveLevelCE.redBlackPairs + (ulli)(redsFromDeadChildren)*(cn->aboveLevelCE.greens + cn->aboveLevelCE.blacks) + cn->aboveLevelCE.redGreenAbovePairs;
                    sn->aboveLevelCE.redGreenAbovePairs += (ulli)sn->aboveLevelCE.reds*(greensFromDeadChildren);
                    sn->aboveLevelCE.reds += redsFromDeadChildren + cn->aboveLevelCE.reds;
                    sn->aboveLevelCE.blacks += blacksFromDeadChildren + cn->aboveLevelCE.blacks + cn->aboveLevelCE.greens;
                    sn->aboveLevelCE.greens += greensFromDeadChildren;
                    contractStack[contractStackTop-1].aboveLevelCE = sn->aboveLevelCE;
                    contractStack[contractStackTop-1].sameLevelCE = sn->sameLevelCE;
                }
                else{
                    //there is no alive child, so we have to  remove the entire subtree defined by the node cn
                    //remove all children from stack
                    stTop-=numChildren-1;
                    plstNode *sn = &plContractStack[stTop-1];
                    sn->kill = true;
                    //by default send everything to the same Level variable....
                    sn->sameLevelCE.blacks = blacksFromDeadChildren + cn->aboveLevelCE.blacks + cn->aboveLevelCE.greens;
                    sn->sameLevelCE.reds = redsFromDeadChildren + cn->aboveLevelCE.reds;
                    sn->sameLevelCE.greens = greensFromDeadChildren;
                }
            }
        }
    }

    curStart = curNewStart;
    curEnd = contractStackTop;

}

void contractFindLeftCompGreensAreMaintained() {

    int stTop = 0;
    int curNewStart = contractStackTop;
    int i;
    for(i=curStart;i<curEnd;i++){
        contract_node *cn = &contractStack[i];

        if(cn->degree == 0){

            //the current node is a leaf, so we must see whether we want to keep it or not.
            if(isRed(cn->id)){
                //the leaf is red so we keep it
                /*
                 *
                 * pushing to stack
                 *
                 */
                //we want to keep the leaf so we give it to the output
                plstNode *sn = &plContractStack[stTop];
                sn->kill = false;
                sn->aboveLevelCE = cn->aboveLevelCE;
                sn->sameLevelCE = cn->sameLevelCE;
                stTop++;

                /*
                 *
                 * give to the output
                 *
                 *
                 */
                contract_node *nn;
                nn = &contractStack[contractStackTop++];
                nn->degree = 0;
                nn->id = cn->id;
                nn->aboveLevelCE = cn->aboveLevelCE;
                nn->sameLevelCE = cn->sameLevelCE;

            }
            else if(isGreen(cn->id) || isBlue(cn->id)){

                //the leaf is green or blue so it stored as green
                plstNode *sn = &plContractStack[stTop];
                sn->kill = true;
                //by default send everything to the same Level variable....
                sn->sameLevelCE.blacks = cn->sameLevelCE.blacks + cn->aboveLevelCE.blacks;
                sn->sameLevelCE.reds = cn->sameLevelCE.reds + cn->aboveLevelCE.reds;
                sn->sameLevelCE.greens = cn->sameLevelCE.greens + cn->aboveLevelCE.greens + 1;
                stTop++;

            }
            else{

                //the leaf is black so it is stored as black
                plstNode *sn = &plContractStack[stTop];
                sn->kill = true;
                //by default send everything to the same Level variable....
                sn->sameLevelCE.blacks = cn->sameLevelCE.blacks + cn->aboveLevelCE.blacks + 1;
                sn->sameLevelCE.reds = cn->sameLevelCE.reds + cn->aboveLevelCE.reds;
                sn->sameLevelCE.greens = cn->sameLevelCE.greens + cn->aboveLevelCE.greens;
                stTop++;

            }

        }
        else{

            //we are at an internal node, we need to determine if we are going to kill this node or not. We only kill the node defined by cn IF d-1 of its children are
            //dead where d is the degree of the node.

            int numDeadChildren = 0;
            int numChildren = cn->degree;
            int redsFromDeadChildren = cn->sameLevelCE.reds;
            int blacksFromDeadChildren = cn->sameLevelCE.blacks;
            int greensFromDeadChildren = cn->sameLevelCE.greens;
            ulli redGreenPairsFromDeadChildren = cn->sameLevelCE.redGreenPairs;
            plstNode *aliveChild = nullptr;
            for(int c=0;c<numChildren;c++){
                plstNode *child = &plContractStack[stTop-c-1];
                if(child->kill){
                    numDeadChildren++;
                    redGreenPairsFromDeadChildren = redGreenPairsFromDeadChildren + (ulli)redsFromDeadChildren*child->sameLevelCE.greens + (ulli)greensFromDeadChildren*child->sameLevelCE.reds;
                    redsFromDeadChildren += child->sameLevelCE.reds;
                    blacksFromDeadChildren += child->sameLevelCE.blacks;
                    greensFromDeadChildren += child->sameLevelCE.greens;
                }
                else{
                    aliveChild = child;
                }
            }

            if(numDeadChildren < numChildren - 1){
                //we keep this internal node, the number of dead black leafs go to current level of cn
                //print to output
                contract_node *nn = &contractStack[contractStackTop];
                nn->aboveLevelCE = cn->aboveLevelCE;
                nn->sameLevelCE.reds = redsFromDeadChildren;
                nn->sameLevelCE.blacks = blacksFromDeadChildren;
                nn->sameLevelCE.greens = greensFromDeadChildren;
                nn->sameLevelCE.redGreenPairs = redGreenPairsFromDeadChildren;
                nn->id = cn->id;
                nn->degree = numChildren-numDeadChildren;
                contractStackTop++;

                //push to stack
                stTop-=numChildren-1;
                plstNode *sn = &plContractStack[stTop-1];
                sn->sameLevelCE = nn->sameLevelCE;
                sn->aboveLevelCE = nn->aboveLevelCE;
                sn->kill = false;

            }
            else{
                //we do not keep this internal node
                if(numDeadChildren == numChildren - 1){
                    //there is one alive child, the contracted edge of which we will need to update
                    /*everything from the dead children will go to above level blacks of the alive child!
                      but also what the cn node already has in the SAME level AND above level*/
                    contractedEdgeAboveLevel tmp = aliveChild->aboveLevelCE;
                    contractedEdgeSameLevel tmp2 = aliveChild->sameLevelCE;
                    stTop-=numChildren-1;
                    plstNode *sn = &plContractStack[stTop-1];
                    sn->kill = false;
                    sn->aboveLevelCE = tmp;
                    sn->sameLevelCE = tmp2;
                    sn->aboveLevelCE.redGreenPairs += redGreenPairsFromDeadChildren + cn->aboveLevelCE.redGreenPairs;
                    sn->aboveLevelCE.redBlackPairs += (ulli)sn->aboveLevelCE.reds*(blacksFromDeadChildren + cn->aboveLevelCE.blacks) + cn->aboveLevelCE.redBlackPairs + (ulli)(redsFromDeadChildren)*cn->aboveLevelCE.blacks;
                    sn->aboveLevelCE.redGreenAbovePairs += (ulli)sn->aboveLevelCE.reds*(greensFromDeadChildren + cn->aboveLevelCE.greens) + cn->aboveLevelCE.redGreenAbovePairs + (ulli)(redsFromDeadChildren)*cn->aboveLevelCE.greens;
                    sn->aboveLevelCE.reds += redsFromDeadChildren + cn->aboveLevelCE.reds;
                    sn->aboveLevelCE.blacks += blacksFromDeadChildren + cn->aboveLevelCE.blacks;
                    sn->aboveLevelCE.greens += greensFromDeadChildren + cn->aboveLevelCE.greens;
                    contractStack[contractStackTop-1].aboveLevelCE = sn->aboveLevelCE;
                    contractStack[contractStackTop-1].sameLevelCE = sn->sameLevelCE;
                }
                else{
                    //there is no alive child, so we have to  remove the entire subtree defined by the node cn
                    //remove all children from stack
                    stTop-=numChildren-1;
                    plstNode *sn = &plContractStack[stTop-1];
                    sn->kill = true;
                    //by default send everything to the same Level variable....
                    sn->sameLevelCE.blacks = blacksFromDeadChildren + cn->aboveLevelCE.blacks;
                    sn->sameLevelCE.reds = redsFromDeadChildren + cn->aboveLevelCE.reds;
                    sn->sameLevelCE.greens = greensFromDeadChildren + cn->aboveLevelCE.greens;
                }
            }
        }
    }

    curStart = curNewStart;
    curEnd = contractStackTop;

}


/*
 *
 *
 * FINDING COMPONENTS ------------ END
 *
 *
 */


#endif //DTRIPLET_CONTRACT_COMPONENT_H
