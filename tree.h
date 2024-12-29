#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <vector>
#include <stack>

using namespace std;

struct Node {
    string tip;
    int line;
    string value;

    vector<Node*> childrenNodes;
    Node(string nodeName)
    {
        int spaceIndex = nodeName.find_first_of(' ');
        // Nije naso niti jedan razmak znaci da je cvor, a ne list
        if(spaceIndex == -1){
            tip = nodeName;
            line = -1;
            return;
        }

        tip = nodeName.substr(0, spaceIndex);
        nodeName = nodeName.substr(spaceIndex+1);

        spaceIndex = nodeName.find_first_of(' ');

        string lineString = nodeName.substr(0, spaceIndex);
        line = atoi(lineString.c_str());
        nodeName = nodeName.substr(spaceIndex+1);

        value = nodeName;
    }
};

class Tree {
private:
    Node* root;
    stack<pair<Node*, int>> dfsStack; // int je za indeks cvora
    void recPrint(Node* parentNode, int depth);

public:
    Tree();
    Node* getRoot();
    Node* next();
    void print();
};

#endif