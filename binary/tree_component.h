#ifndef TRIPLETPARSER_TREE_COMPONENT_H
#define TRIPLETPARSER_TREE_COMPONENT_H
#include "tree.h"
#include <iostream>
#include <fstream>
using namespace std;

tree_node *T1;
int T1numLeafs;
int T1size;

tree_node *T2;
int T2numLeafs;
int T2size;


/*
 *
 *
 * DOT FORMAT OUTPUT --- BEGIN
 *
 *
 */

void tree1BstPrintDotAux(int cur, ofstream& treeFout) {

    if(T1[cur].numChildren == 0) return;

    treeFout<<"    "<<T1[cur].id<<" -> "<<T1[T1[cur].lpos].id<<";\n";
    tree1BstPrintDotAux(T1[cur].lpos, treeFout);

    treeFout<<"    "<<T1[cur].id<<" -> "<<T1[T1[cur].rpos].id<<";\n";
    tree1BstPrintDotAux(T1[cur].rpos, treeFout);

}

void tree1PrintDotHelper(int cur, ofstream& treeFout){
    treeFout<<"digraph BST {\n";
    treeFout<<"    node [fontname=\"Arial\"];\n";

    if(cur == -1){
        treeFout<<"\n";
    }
    else if(T1[cur].numChildren == 0){
        treeFout<<"    "<<T1[cur].id<<";\n";
    }
    else{
        tree1BstPrintDotAux(cur, treeFout);
    }

    treeFout<<"}\n";
}

void tree1PrintDot(char * file, int root){

    ofstream treeFout;
    treeFout.open(file, ios_base::out);
    tree1PrintDotHelper(root, treeFout);
    treeFout.close();

}

void tree2BstPrintDotAux(int cur, ofstream& treeFout) {

    if(T2[cur].numChildren == 0) return;

    treeFout<<"    "<<T2[cur].id<<" -> "<<T2[T2[cur].lpos].id<<";\n";
    tree2BstPrintDotAux(T2[cur].lpos, treeFout);

    treeFout<<"    "<<T2[cur].id<<" -> "<<T2[T2[cur].rpos].id<<";\n";
    tree2BstPrintDotAux(T2[cur].rpos, treeFout);

}

void tree2PrintDotHelper(int cur, ofstream& treeFout){
    treeFout<<"digraph BST {\n";
    treeFout<<"    node [fontname=\"Arial\"];\n";

    if(cur == -1){
        treeFout<<"\n";
    }
    else if(T2[cur].numChildren == 0){
        treeFout<<"    "<<T2[cur].id<<";\n";
    }
    else{
        tree2BstPrintDotAux(cur, treeFout);
    }

    treeFout<<"}\n";
}

void tree2PrintDot(char * file, int root){

    ofstream treeFout;
    treeFout.open(file, ios_base::out);
    tree2PrintDotHelper(root, treeFout);
    treeFout.close();

}

/*
 *
 *
 * DOT FORMAT OUTPUT --- END
 *
 *
 */

#endif //TRIPLETPARSER_TREE_COMPONENT_H
