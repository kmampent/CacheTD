#ifndef TRIPLETCACHE_CONTRACT_COMPONENT_H
#define TRIPLETCACHE_CONTRACT_COMPONENT_H
#include "newick_parser.h"
#include "contract_node.h"
#include "common.h"
#include <iostream>
using namespace std;

contract_node* contractStack; //stack that will hold all the components
int contractStackTop; //pointer to the first free element of the stack
int curStart, curEnd; //start and end pointers of the current component

//stacks used during contraction/counting
cNode* countingStack;
rstNode *rContractStack;
pstNode *plContractStack;

/*
 *
 *
 * Finding first contract component ------------ START
 *
 *
 */

void contractInit(){

    //T2 is in a post order form so creating a component is super easy, we just have a for loop

    int i;
    for(i = 0; i < T2size; i++){

        contract_node* nn = &contractStack[i];
        tree_node* tn = &T2[i];

        nn->isLeaf = tn->numChildren == 0;
        if(nn->isLeaf) nn->id = dfsIDs[tn->id];
        else nn->id = tn->id;
        nn->s = nn->st = 0;
        /*nn->lpos = tn->lpos;
        nn->rpos = tn->rpos;*/

    }

    curStart = 0;
    curEnd = contractStackTop = T2size;

}


/*
 *
 *
 * FINDING SHARED TRIPLETS ------------ START
 *
 *
 */

bool hasColor(int id){

    return (id >= colors.blueMin && id <= colors.blueMax) || (id >= colors.redMin && id <= colors.redMax);

}

bool isBlue(int id){

    return (id >= colors.blueMin && id <= colors.blueMax);

}

bool isRed(int id){

    return (id >= colors.redMin && id <= colors.redMax);

}

unsigned long long int findS() {

    int stTop = 0;

    int i;
    unsigned long long int curShared = 0;

    for(i=curStart; i<curEnd; i++){

        contract_node* cur = &contractStack[i];
        if(cur->isLeaf){

            countingStack[stTop].reds = 0;
            countingStack[stTop].blues = 0;
            if(isRed(cur->id))
                countingStack[stTop].reds = 1;
            else if(isBlue(cur->id)) {
                countingStack[stTop].blues = 1;
                curShared += cur->s;
            }

            countingStack[stTop].reds += cur->st;
            stTop++;

        }
        else{

            cNode * left = &countingStack[stTop-2];
            cNode * right = &countingStack[stTop-1];

            unsigned long long int totBlues = left->blues + right->blues;
            curShared += left->reds*(left->reds-1)/2*right->blues +
                         left->blues*(left->blues-1)/2*right->reds +
                         left->reds*right->blues*(right->blues-1)/2 +
                         left->blues*right->reds*(right->reds-1)/2 +
                         cur->s*totBlues + cur->st*(totBlues)*(totBlues-1)/2;

            left->reds = left->reds + right->reds + cur->st;
            left->blues = left->blues + right->blues;
            stTop--;

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
 * Finding first contract component ------------ END
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

void contractFindRightComp(){

    int stTop = 0;
    int curNewStart = contractStackTop;
    int i;
    for(i=curStart;i<curEnd;i++){
        contract_node * cn = &contractStack[i];
        if(cn->isLeaf){
            //the current node is a leaf, so we must see whether we want to keep it or not.
            if(isBlue(cn->id)){
                //we want to keep the leaf so we give it to the output.
                rContractStack[stTop].pos = contractStackTop;
                rContractStack[stTop++].kill = false;
                contract_node * nn;
                nn = &contractStack[contractStackTop++];
                nn->isLeaf = true;
                nn->id = cn->id;
                nn->s = nn->st = 0;
            }
            else{
                //we do not want to keep the leaf, so we do not send it to the output, but we push it to the stack.
                rContractStack[stTop++].kill = true;
            }
        }
        else{
            //we are at an internal node, we have two cases, delete that node or not.
            rstNode *last = &rContractStack[stTop-1];
            rstNode *secondLast = &rContractStack[stTop-2];
            stTop--;
            if(last->kill || secondLast->kill){
                //we do not want to keep this internal node
                if(last->kill && secondLast->kill){
                    secondLast->kill = true;
                }
                else{
                    if(!last->kill)
                        secondLast->pos = last->pos;
                    secondLast->kill = false;
                }
            }
            else{
                //we want to keep it
                contract_node * nn;
                nn = &contractStack[contractStackTop];
                nn->isLeaf = false;
                nn->id = cn->id;
                nn->s = nn->st = 0;
                /*nn->lpos = secondLast->pos;
                nn->rpos = last->pos;*/
                secondLast->kill = false;
                secondLast->pos = contractStackTop++;
            }
        }
    }
    curStart = curNewStart;
    curEnd = contractStackTop;

}

void contractFindParentComp(){

    int stTop = 0;
    int curNewStart = contractStackTop;
    int i;
    //read old component
    for(i=curStart;i<curEnd;i++){
        contract_node * cn = &contractStack[i]; //get next node in old component
        if(cn->isLeaf){
            //the current node is a leaf, so we must see whether we want to keep it or not.
            if(hasColor(cn->id)){
                //we do not want to keep the leaf, so we do not send it to the output, but we push it to the stack.
                plContractStack[stTop].st = cn->st + 1; //the colored leaf becomes a red node.
                plContractStack[stTop].s = plContractStack[stTop].st*(plContractStack[stTop].st-1)/2;
                plContractStack[stTop++].kill = true;
            }
            else{
                //we want to keep the leaf so we want to both push it to the stack and give it to the output.
                //push to stack
                plContractStack[stTop].pos = contractStackTop;
                plContractStack[stTop].s = cn->s;
                plContractStack[stTop].st = cn->st;
                plContractStack[stTop++].kill = false;
                //give to output
                contract_node * nn;
                nn = &contractStack[contractStackTop++];
                nn->isLeaf = true;
                nn->id = cn->id;
                nn->s = cn->s;
                nn->st = cn->st;
            }
        }
        else{
            //we are at an internal node, we have two cases, delete that node or not.
            pstNode *last = &plContractStack[stTop-1];
            pstNode *secondLast = &plContractStack[stTop-2];
            stTop--;
            if(last->kill || secondLast->kill){
                //we do not want to keep this internal node
                if(last->kill && secondLast->kill){
                    secondLast->kill = true;
                    secondLast->st = secondLast->st + last->st + cn->st;
                    secondLast->s = secondLast->st*(secondLast->st-1)/2;
                }
                else{
                    if(!last->kill) {
                        secondLast->pos = last->pos;
                    }
                    secondLast->kill = false;
                    secondLast->s = secondLast->s + last->s + cn->s;
                    secondLast->st = secondLast->st + last->st + cn->st;
                    contractStack[secondLast->pos].s = secondLast->s;
                    contractStack[secondLast->pos].st = secondLast->st;
                }
            }
            else{
                //we want to keep it
                //print to output
                contract_node * nn;
                nn = &contractStack[contractStackTop];
                nn->isLeaf = false;
                nn->id = cn->id;
                nn->s = cn->s;
                nn->st = cn->st;
                /*nn->lpos = secondLast->pos;
                nn->rpos = last->pos;*/
                //push to stack
                secondLast->kill = false;
                secondLast->pos = contractStackTop++;
                secondLast->s = nn->s;
                secondLast->st = nn->st;
            }
        }
    }
    curStart = curNewStart;
    curEnd = contractStackTop;

}

void contractFindLeftComp(){
    int stTop = 0;
    int curNewStart = contractStackTop;
    int i;
    //read old component
    for(i=curStart;i<curEnd;i++){
        contract_node * cn = &contractStack[i]; //get next node in old component
        if(cn->isLeaf){
            //the current node is a leaf, so we must see whether we want to keep it or not.
            if(!isRed(cn->id)){
                //we do not want to keep the leaf, so we do not send it to the output, but we push it to the stack.
                plContractStack[stTop].s = cn->st*(cn->st-1)/2;
                plContractStack[stTop].st = cn->st;
                plContractStack[stTop++].kill = true;
            }
            else{
                //we want to keep the leaf so we want to both push it to the stack and give it to the output.
                //push to stack
                plContractStack[stTop].pos = contractStackTop;
                plContractStack[stTop].s = cn->s;
                plContractStack[stTop].st = cn->st;
                plContractStack[stTop++].kill = false;
                //give to output
                contract_node * nn;
                nn = &contractStack[contractStackTop++];
                nn->isLeaf = true;
                nn->id = cn->id;
                nn->s = cn->s;
                nn->st = cn->st;
            }
        }
        else{
            //we are at an internal node, we have two cases, delete that node or not.
            pstNode *last = &plContractStack[stTop-1];
            pstNode *secondLast = &plContractStack[stTop-2];
            stTop--;
            if(last->kill || secondLast->kill){
                //we do not want to keep this internal node
                if(last->kill && secondLast->kill){
                    secondLast->kill = true;
                    secondLast->st = secondLast->st + last->st + cn->st;
                    secondLast->s = secondLast->st*(secondLast->st-1)/2;
                }
                else{
                    if(!last->kill) {
                        secondLast->pos = last->pos;
                    }
                    secondLast->kill = false;
                    secondLast->s = secondLast->s + last->s + cn->s;
                    secondLast->st = secondLast->st + last->st + cn->st;
                    contractStack[secondLast->pos].s = secondLast->s;
                    contractStack[secondLast->pos].st = secondLast->st;
                }
            }
            else{
                //we want to keep it
                //print to output
                contract_node * nn;
                nn = &contractStack[contractStackTop];
                nn->isLeaf = false;
                nn->id = cn->id;
                nn->s = cn->s;
                nn->st = cn->st;
                /*nn->lpos = secondLast->pos;
                nn->rpos = last->pos;*/
                //push to stack
                secondLast->kill = false;
                secondLast->pos = contractStackTop++;
                secondLast->s = nn->s;
                secondLast->st = nn->st;
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



#endif //TRIPLETCACHE_CONTRACT_COMPONENT_H
